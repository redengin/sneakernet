import 'dart:io';
import 'package:flutter/services.dart';
import 'package:sneakernet/library.dart';
import 'package:plugin_wifi_connect/plugin_wifi_connect.dart';
import 'package:sneakernet/rest/lib/api.dart';
import 'package:http/http.dart';
import 'package:path/path.dart' as p;
import 'package:version/version.dart';

import '../notifications.dart';

const sneakerNetPrefix = "SneakerNet";

class SneakerNet {
  static sync(ssid) async {
    if (await PluginWifiConnect.connect(ssid, saveNetwork: false) ?? false) {
      final notificationLabel = ssid;
      flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID,
          notificationLabel, 'syncing...', notificationDetails);

      final restClient = DefaultApi();

      // attempt to update the firmware first
      if (false == await _syncFirmware(notificationLabel, restClient)) {
        // if firmware is up to date, sync files
        _syncFiles(notificationLabel, restClient);
      }

      PluginWifiConnect.disconnect();
    }
  }

  static Future<bool> _syncFirmware(
      String notificationLabel, restClient) async {
    final remoteFirmware = await restClient.firmwareGet();
    if (remoteFirmware == null) return false;

    // see if we have newer firmware
    ByteData? newFirmwareData;
    switch (remoteFirmware.filename) {
      case "esp32-sneakernet.bin":

        /// TODO update each time new esp32 firmware is added as an asset
        final esp32FirmwareVersion = Version.parse("1.0.0");
        if (esp32FirmwareVersion > Version.parse(remoteFirmware.version)) {
          newFirmwareData =
              await rootBundle.load('firmware/esp32-sneakernet.bin');
        }
    }
    // no firmware found
    if (newFirmwareData == null) return false;

    // update the firmware
    var response = await restClient.firmwarePutWithHttpInfo(
        body:
            MultipartFile.fromBytes('', newFirmwareData.buffer.asUint8List()));
    if (response.statusCode == 200) {
      flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID,
          notificationLabel, "updated firmware", notificationDetails);
      return true;
    }
    flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID, notificationLabel,
        "firmware update failed", notificationDetails);
    return false;
  }

  static _syncFiles(String notificationLabel, DefaultApi restClient) async {
    final List<File> localCatalog = library.files();
    final List<String> localFilenames =
        localCatalog.map((e) => p.basename(e.path)).toList(growable: false);
    final unwantedFilenames = library.unwantedFiles.list();
    final flaggedFilenames = library.flaggedFiles.list();

    final remoteCatalog = await restClient.catalogGet();
    if (remoteCatalog != null) {
      // remove the flagged content
      for (var entry in remoteCatalog) {
        if (flaggedFilenames.contains(entry.filename)) {
          // announce the deletion
          flutterLocalNotificationsPlugin.show(
              SNEAKERNET_SYNC_ID,
              notificationLabel,
              "removing $entry.filename",
              notificationDetails);
          restClient.catalogFilenameDelete(entry.filename);
        }
      }

      // download new files
      for (var entry in remoteCatalog) {
        if (localFilenames.contains(entry.filename) == false) {
          // announce the download
          flutterLocalNotificationsPlugin.show(
              SNEAKERNET_SYNC_ID,
              notificationLabel,
              "downloading $entry.filename",
              notificationDetails);
          final Response get =
              await restClient.catalogFilenameGetWithHttpInfo(entry.filename);
          if (get.statusCode == 200) {
            // find the entry to retrieve the timestamp
            var entry = remoteCatalog
                .firstWhere((entry) => entry.filename == entry.filename);
            final timestamp = (entry.timestamp != null)
                ? DateTime(entry.timestamp!)
                : DateTime.now();
            library.add(entry.filename, get.bodyBytes, timestamp);
          }
        }
      }

      // send local files
      final remoteFilenames = remoteCatalog.map((e) => e.filename).toList(growable: false);
      for (var file in localCatalog) {
        final filename = p.basename(file.path);
        if (remoteFilenames.contains(filename) == false) {
          // announce the upload
          flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID,
              notificationLabel, "sending $filename", notificationDetails);
          final body = await MultipartFile.fromPath('', file.path);
          await restClient.catalogFilenamePutWithHttpInfo(filename, body: body);
        }
      }
    }
  }
}
