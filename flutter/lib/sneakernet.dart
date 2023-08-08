import 'dart:collection';
import 'package:wifi_scan/wifi_scan.dart';
import 'package:plugin_wifi_connect/plugin_wifi_connect.dart';

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
      _sync();
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
  static void _sync() {
    // TODO
  }
}

