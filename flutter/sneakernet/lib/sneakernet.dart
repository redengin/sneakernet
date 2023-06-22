import 'dart:collection';
import 'package:wifi_scan/wifi_scan.dart';
import 'package:sneakernet/notifications.dart';

SplayTreeSet<WiFiAccessPoint> localSneakerNets =
    SplayTreeSet<WiFiAccessPoint>((a, b) => a.bssid.compareTo(b.bssid));

class SneakerNet {
    static void sync(WiFiAccessPoint sneakerNet) {
        flutterLocalNotificationsPlugin.show(
            SNEAKERNET_SYNC_ID, 'SneakerNets syncing...', sneakerNet.ssid, notificationDetails);
    }
}