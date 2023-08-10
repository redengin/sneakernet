import 'dart:collection';
import 'dart:io';
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
    flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID,
        'SneakerNets syncing...', sneakerNet.ssid, notificationDetails);

    // memo the current connection
    String? originalSsid = await PluginWifiConnect.ssid;
    if (await PluginWifiConnect.connect(sneakerNet.ssid) ?? false) {
      // sync with sneakernet
      await _sync();
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
  static Future<void> _sync() async {
    final restClient = DefaultApi();
    final remoteCatalog = await restClient.catalogGet();
    if (remoteCatalog == null) return;
    final List<String> remoteFilenames =
        remoteCatalog.map((e) => e.filename).toList(growable: false);

    final List<File> localCatalog = library.files();

    // receive new files
    final List<String> localFilenames =
      localCatalog.map((e) => p.basename(e.path)).toList(growable: false);
    for (var entry in remoteCatalog) {
      if (localFilenames.contains(entry.filename) == false) {
        final Response get =
            await restClient.catalogFilenameGetWithHttpInfo(entry.filename);
        if (get.statusCode == 200) library.add(entry.filename, get.bodyBytes);
      }
    }

    // send our files
    for(var file in localCatalog) {
      final filename = p.basename(file.path);
      if(remoteFilenames.contains(filename) == false) {
        final body = MultipartFile.fromBytes(filename, file.readAsBytesSync());
        await restClient.catalogFilenamePutWithHttpInfo(filename, body: body);
      }
    }

    // TODO update firmware

    // TODO update stored App
  }
}
