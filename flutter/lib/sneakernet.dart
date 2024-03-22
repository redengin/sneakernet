import 'dart:io';
import 'package:flutter/services.dart';
import 'package:http/http.dart';
import 'package:path/path.dart' as p;
import 'package:version/version.dart';
import 'package:wifi_iot/wifi_iot.dart';
import 'package:sneakernet/library.dart';
import 'package:sneakernet/rest/lib/api.dart';

import '../notifications.dart';

const sneakerNetPrefix = "SneakerNet";

class SneakerNet {
  static sync(ssid, library) async {
    if (await WiFiForIoTPlugin.connect(ssid, timeoutInSeconds: 15)) {
      if (await WiFiForIoTPlugin.forceWifiUsage(true)) {
        final restClient = DefaultApi();
        // attempt to update the firmware first
        if (false == await _syncFirmware(restClient)) {
          await _syncFiles(library, restClient);
        }
        WiFiForIoTPlugin.forceWifiUsage(false);
      }
      WiFiForIoTPlugin.disconnect();
    }
  }

  static Future<bool> _syncFirmware(restClient) async {
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
      return true;
    }
    return false;
  }

  static _syncFiles(Library library, DefaultApi restClient) async {
    final remoteCatalog = await restClient.catalogGet();
    if (remoteCatalog != null) {
      // remove the flagged content
      for (var entry in remoteCatalog) {
        if (library.isFlagged(entry.filename)) {
          restClient.catalogFilenameDelete(entry.filename);
        }
      }

      // download new files
      for (var entry in remoteCatalog) {
        final timestamp = DateTime.fromMillisecondsSinceEpoch(entry.timestampMs, isUtc: true);
        if (library.isWanted(entry.filename, timestamp)) {
          final Response get =
              await restClient.catalogFilenameGetWithHttpInfo(entry.filename);
          if (get.statusCode == 200) {
            await library.add(entry.filename, get.bodyBytes, timestamp);
          }
        }
      }

      // send local files
      final List<File> localCatalog = library.files();
      final remoteFilenames =
          remoteCatalog.map((e) => e.filename).toList(growable: false);
      for (var file in localCatalog) {
        final filename = p.basename(file.path);
        if (remoteFilenames.contains(filename) == false) {
          final timestampMs = file.lastModifiedSync().toUtc().millisecondsSinceEpoch;
          final body = await MultipartFile.fromPath('', file.path);
          await restClient.catalogFilenamePutWithHttpInfo(filename, timestampMs, body: body);
        }
      }
    }
  }
}
