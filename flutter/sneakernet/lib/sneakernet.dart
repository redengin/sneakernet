import 'dart:collection';
import 'package:wifi_scan/wifi_scan.dart';
import 'package:sneakernet/notifications.dart';

SplayTreeSet<WiFiAccessPoint> localSneakerNets =
    SplayTreeSet<WiFiAccessPoint>((a, b) => a.bssid.compareTo(b.bssid));

class SneakerNet {
  static void sync(WiFiAccessPoint sneakerNet) async {
    flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID,
        'SneakerNets syncing...', sneakerNet.ssid, notificationDetails);
    // if (!await WifiConnector.connectToWifi(ssid: sneakerNet.ssid)) {
    //   flutterLocalNotificationsPlugin.show(SNEAKERNET_SYNC_ID, 'SneakerNets',
    //       "Unable to connect to $sneakerNet.ssid", notificationDetails);
    //   return;
    // }
  }
}
