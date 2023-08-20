import 'dart:collection';
import 'dart:io';
import 'package:flutter/services.dart';
import 'package:sneakernet/library.dart';
import 'package:wifi_scan/wifi_scan.dart';
import 'package:plugin_wifi_connect/plugin_wifi_connect.dart';
import 'package:sneakernet/rest/lib/api.dart';
import 'package:http/http.dart';
import 'package:path/path.dart' as p;
import 'package:version/version.dart';

import '../notifications.dart';

// initialized by main
late final SneakerNet sneakernet;

const sneakerNetPrefix = "SneakerNet";

class SneakerNet {
  final SplayTreeSet<WiFiAccessPoint> _foundNodes =
      SplayTreeSet<WiFiAccessPoint>((a, b) => a.bssid.compareTo(b.bssid));

  bool get hasNodes => _foundNodes.isNotEmpty;

  /// add found nodes to the syncable list
  void addAll(List<WiFiAccessPoint> sneakerNets) {
    // TODO deprioritize/delay successfully synced nodes
    _foundNodes.addAll(sneakerNets);
  }

  syncNext() async {
    if (_foundNodes.isEmpty) return;

    // pop the first node
    final node = _foundNodes.first;
    _foundNodes.remove(node);

    // connect to the node
    // memo the current connection
    String? originalSsid = await PluginWifiConnect.ssid;
    if (await PluginWifiConnect.connect(node.ssid) ?? false) {
      // sync with remote sneakernet node
      flutterLocalNotificationsPlugin.show(
          SNEAKERNET_SYNC_ID, node.ssid, 'syncing...', notificationDetails);

      var handled = await _sync(node.ssid);
      // TODO if handled, delay future sync

      // restore previous connection
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

  Future<bool> _sync(String ssid) async {
    final restClient = DefaultApi();

    // sync firmware
    if(await _syncFirmware(ssid, restClient)) {
      // if firmware is updated, files have not been synced
      return false;
    }

    // sync files
    await _syncFiles(ssid, restClient);

    return true;
  }

  /// TODO update each time new esp32 firmware is added as an asset
  final esp32FirmwareVersion = Version.parse("0.0.2");

  Future<bool> _syncFirmware(String ssid, restClient) async {
    final remoteFirmware = await restClient.firmwareGet();
    if(remoteFirmware == null) return false;

    List<int>? newFirmwareData;
    switch(remoteFirmware.filename) {
      case "esp32-sneakernet.bin" :
        if(esp32FirmwareVersion > Version.parse(remoteFirmware.version)) {
          newFirmwareData = await rootBundle.loadBuffer('firmware/esp32-sneakernet.bin') as List<int>;
        }
    }
    if(newFirmwareData == null) return false;

    // update the firmware
    var response = await restClient.firmwarePutWithHttpInfo(
        body: MultipartFile.fromBytes('', newFirmwareData));
    if (response.statusCode == 200) {
      flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID, ssid,
          "updated firmware", notificationDetails);
      return true;
    }
    flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID, ssid,
        "firmware update failed", notificationDetails);
    return false;
  }

  _syncFiles(String ssid, DefaultApi restClient) async {
    final List<File> localCatalog = library.files();
    final List<String> localFilenames =
        localCatalog.map((e) => p.basename(e.path)).toList(growable: false);

    // download new files
    // TODO ignore locally deleted files
    final remoteCatalog = await restClient.catalogGet();
    if (remoteCatalog == null) return true;
    final List<String> remoteFilenames =
        remoteCatalog.map((e) => e.filename).toList(growable: false);
    for (var entry in remoteCatalog) {
      if (localFilenames.contains(entry.filename) == false) {
        flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID, ssid,
            "downloading $entry.filename", notificationDetails);
        final Response get =
            await restClient.catalogFilenameGetWithHttpInfo(entry.filename);
        if (get.statusCode == 200) library.add(entry.filename, get.bodyBytes);
      }
    }

    // send files
    for (var file in localCatalog) {
      final filename = p.basename(file.path);
      if (remoteFilenames.contains(filename) == false) {
        flutterLocalNotificationsPlugin.show(
            SNEAKERNET_SYNC_ID, ssid, "sending $filename", notificationDetails);
        final body = MultipartFile.fromBytes(filename, file.readAsBytesSync());
        await restClient.catalogFilenamePutWithHttpInfo(filename, body: body);
      }
    }
  }
}