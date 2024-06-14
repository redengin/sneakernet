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

  static Set<String> apsToSneakerNets(List<WiFiAccessPoint> aps) {
    return aps
        .where((_) => _.ssid.startsWith(SneakerNet.ssidPrefix))
        .toList(growable: false)
        .map((_) => _.ssid)
        .toSet();
  }

  static Future<String> synchronize(ssid, library) async {
    var message = "connection failed";

    if (await WiFiForIoTPlugin.connect(ssid, timeoutInSeconds: 120)) {
      if (await WiFiForIoTPlugin.forceWifiUsage(true)) {
        try {
          final restClient = DefaultApi();
          // attempt to update the firmware first
          if (await _syncFirmware(restClient)) {
            message = "updated firmware, rebooting device";
          } else {
            message = await _syncFiles(library, restClient);
          }
        } catch (_) {}
        await WiFiForIoTPlugin.forceWifiUsage(false);
      }
      await WiFiForIoTPlugin.disconnect();
      // fix for old Android that automatically registers SneakerNets
      await WiFiForIoTPlugin.removeWifiNetwork(ssid);
    }

    return message;
  }

  static Future<bool> _syncFirmware(restClient) async {
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
    try {
      await restClient.firmwarePutWithHttpInfo(
          body: MultipartFile.fromBytes(
              '', newFirmwareData.buffer.asUint8List()));
    } on ApiException catch (_) {
      return true;
    }
    return false;
  }

  static Future<String> _syncFiles(
      Library library, DefaultApi restClient) async {
    final remoteCatalog = await restClient.catalogGet();
    if (remoteCatalog == null) {
      return "Unable to synchronize files";
    }
    var filesReceived = 0;
    var filesRemoved = 0;
    var filesAdded = 0;

    // remove the flagged content
    for (var entry in remoteCatalog) {
      if (library.isFlagged(entry.filename)) {
        restClient.catalogFilenameDelete(entry.filename);
        filesRemoved++;
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
          filesReceived++;
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
        filesAdded++;
      }
    }
    // notify of the changes
    var status = (filesReceived > 0 ? "$filesReceived new files. " : "") +
        (filesAdded > 0 ? "$filesAdded pushed. " : "") +
        (filesRemoved > 0 ? "$filesRemoved removed." : "");
    if (status.isEmpty) {
      return "Synchronized";
    } else {
      return status;
    }
  }
}
