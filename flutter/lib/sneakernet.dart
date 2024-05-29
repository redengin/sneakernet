import 'dart:async';
import 'dart:io';
import 'package:flutter/services.dart';
import 'package:http/http.dart';
import 'package:path/path.dart' as p;
import 'package:version/version.dart';
import 'package:wifi_iot/wifi_iot.dart';
import 'package:sneakernet/library.dart';
import 'package:sneakernet/rest/lib/api.dart';
import 'package:wifi_scan/wifi_scan.dart';

class SneakerNet {
  static const ssidPrefix = "SneakerNet";

  static List<String> apsToSneakerNets(List<WiFiAccessPoint> aps)
  {
    return aps
      .where((_) => _.ssid.startsWith(SneakerNet.ssidPrefix))
      .toList(growable: false)
      .map((_) => _.ssid)
      .toList(growable: false);
  }

  static Future<String> synchronize(ssid, library) async {
    var message = "unable to connect to $ssid";

    if (await WiFiForIoTPlugin.connect(ssid, timeoutInSeconds: 60)) {
      if (await WiFiForIoTPlugin.forceWifiUsage(true)) {
        final restClient = DefaultApi();
        // attempt to update the firmware first
        if (await _syncFirmware(ssid, restClient)) {
          message = "updated firmware of $ssid, rebooting device";
        } else {
          message = await _syncFiles(ssid, library, restClient);
        }
        WiFiForIoTPlugin.forceWifiUsage(false);
      }
      WiFiForIoTPlugin.disconnect();
    }

    return message;
  }

  static Future<bool> _syncFirmware(ssid, restClient) async {
    final remoteFirmware = await restClient.firmwareGet();
    if (remoteFirmware == null) return false;

    // see if we have newer firmware
    ByteData? newFirmwareData;
    switch (remoteFirmware.hardware) {
      case "esp32":

        /// TODO update each time new esp32 firmware is added as an asset
        final esp32FirmwareVersion = Version.parse("1.0.1");
        if (esp32FirmwareVersion > Version.parse(remoteFirmware.version)) {
          newFirmwareData =
              await rootBundle.load('firmware/esp32-sneakernet.bin');
        }
    }
    // no firmware found
    if (newFirmwareData == null) return false;

    // update the firmware
    //   if the firmware is accepted, the device will reboot before responding
    //      causing await to throw an ApiException
    HttpResponse response;
    try {
      response = await restClient.firmwarePutWithHttpInfo(
          body: MultipartFile.fromBytes(
              '', newFirmwareData.buffer.asUint8List()));
    } on ApiException catch (e) {
      return true;
    }
    return false;
  }

  static Future<String> _syncFiles(
      ssid, Library library, DefaultApi restClient) async {
    final remoteCatalog = await restClient.catalogGet();
    if (remoteCatalog != null) {
      var files_received = 0;
      var files_removed = 0;
      var files_added = 0;

      // remove the flagged content
      for (var entry in remoteCatalog) {
        if (library.isFlagged(entry.filename)) {
          restClient.catalogFilenameDelete(entry.filename);
          files_removed++;
        }
      }

      // download new files
      for (var entry in remoteCatalog) {
        final timestamp = DateTime.parse(entry.timestamp);
        if (library.isWanted(entry.filename, timestamp)) {
          final Response get =
              await restClient.catalogFilenameGetWithHttpInfo(entry.filename);
          if (get.statusCode == 200) {
            await library.add(entry.filename, get.bodyBytes, timestamp);
            files_received++;
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
          final timestamp = file.lastModifiedSync().toUtc().toIso8601String();
          final body = await MultipartFile.fromPath('', file.path);
          await restClient.catalogFilenamePutWithHttpInfo(filename, timestamp,
              body: body);
          files_added++;
        }
      }
      // notify of the changes
      return "Synchronized $ssid\n" +
          (files_received > 0 ? "$files_received new files. " : "") +
          (files_added > 0 ? "$files_added pushed. " : "") +
          (files_removed > 0 ? "$files_removed removed." : "");
    } else {
      return "$ssid Failed";
    }
  }
}
