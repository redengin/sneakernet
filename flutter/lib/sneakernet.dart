import 'dart:collection';
import 'dart:io';
import 'package:flutter/services.dart';
import 'package:http/src/response.dart';
import 'package:sneakernet/library.dart';
import 'package:wifi_scan/wifi_scan.dart';
import 'package:plugin_wifi_connect/plugin_wifi_connect.dart';
import 'package:sneakernet/rest/lib/api.dart';
import 'package:http/http.dart';
import 'package:path/path.dart' as p;

import '../notifications.dart';

SplayTreeSet<WiFiAccessPoint> localSneakerNets =
    SplayTreeSet<WiFiAccessPoint>((a, b) => a.bssid.compareTo(b.bssid));

class SneakerNet {
  static void sync(WiFiAccessPoint sneakerNet) async {
    flutterLocalNotificationsPlugin.show(
        SNEAKERNET_SYNC_ID, sneakerNet.ssid, 'syncing...', notificationDetails);

    // memo the current connection
    String? originalSsid = await PluginWifiConnect.ssid;
    if (await PluginWifiConnect.connect(sneakerNet.ssid) ?? false) {
      // sync with sneakernet
      var handled = await _sync(sneakerNet.ssid);
      // TODO if handled defer another sync

      // restore connection
      if (originalSsid != null) {
        if (await PluginWifiConnect.connect(originalSsid) ?? false) {
          flutterLocalNotificationsPlugin.show(
              SNEAKERNET_WIFI_ERROR_ID,
              'SneakerNet unable to restore WIFI connection',
              originalSsid,
              notificationDetails);
        }
      }
    }
  }

  /// Perform RUST operations to sync with SneakerNet node
  static Future<bool> _sync(String ssid) async {
    final restClient = DefaultApi();

    // update firmware
    final sha256 =
        await rootBundle.loadString('firmware/sha256.txt', cache: false);
    var firmware = await restClient.firmwareGet();
    if (firmware != null) {
      if (firmware.sha256 != sha256) {
        flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID, ssid,
            "updating firmware...", notificationDetails);
        final firmwareData =
            await rootBundle.loadBuffer('firmware/sneakernet.bin') as List<int>;
        var response = await restClient.firmwarePutWithHttpInfo(
            body: MultipartFile.fromBytes('', firmwareData));
        if (response.statusCode == 200) {
          flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID, ssid,
              "firmware update successful", notificationDetails);
          return false;
        }
        flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID, ssid,
            "firmware update failed", notificationDetails);
      }
    }

    final remoteCatalog = await restClient.catalogGet();
    if (remoteCatalog == null) return true;
    final List<String> remoteFilenames =
        remoteCatalog.map((e) => e.filename).toList(growable: false);

    final List<File> localCatalog = library.files();

    // receive new files
    final List<String> localFilenames =
        localCatalog.map((e) => p.basename(e.path)).toList(growable: false);
    for (var entry in remoteCatalog) {
      if (localFilenames.contains(entry.filename) == false) {
        flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID, ssid,
            "downloading $entry.filename", notificationDetails);
        final Response get =
            await restClient.catalogFilenameGetWithHttpInfo(entry.filename);
        if (get.statusCode == 200) library.add(entry.filename, get.bodyBytes);
      }
    }

    // send our files
    for (var file in localCatalog) {
      final filename = p.basename(file.path);
      if (remoteFilenames.contains(filename) == false) {
        flutterLocalNotificationsPlugin.show(
            SNEAKERNET_SYNC_ID, ssid, "sending $filename", notificationDetails);
        final body = MultipartFile.fromBytes(filename, file.readAsBytesSync());
        await restClient.catalogFilenamePutWithHttpInfo(filename, body: body);
      }
    }

    // TODO update stored App

    return true;
  }
}
