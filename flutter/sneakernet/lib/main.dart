import 'dart:async';
import 'dart:io';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:sneakernet/notifications.dart';
import 'package:sneakernet/sneakernet.dart';
import 'package:wifi_scan/wifi_scan.dart';
import 'package:workmanager/workmanager.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:sneakernet/settings.dart';
import 'package:sneakernet/pages/HomePage.dart';
import 'package:sneakernet/pages/SettingsPage.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

  // initialize persistent settings
  SharedPreferences preferences = await SharedPreferences.getInstance();
  settings = Settings(preferences: preferences);

  // initialize local notifications
  await flutterLocalNotificationsPlugin.initialize(initializationSettings);

  // subscribe to wifi scans
  final scanSubscription =
      WiFiScan.instance.onScannedResultsAvailable.listen((results) {
    const SNEAKER_NET_SSID = "SneakerNet";
    var sneakerNets = results
        .where((_) => _.ssid.startsWith(SNEAKER_NET_SSID))
        .toList(growable: false);

    if (sneakerNets.isNotEmpty) {
      if(settings.getDoNotify()) {
        var ssids = sneakerNets.map((_) => _.ssid).toList();
        flutterLocalNotificationsPlugin.show(
            SNEAKERNETS_FOUND_ID, 'SneakerNets found', ssids.join(','), notificationDetails);
      }
      if(settings.getAutoSync()) {
        localSneakerNets.addAll(sneakerNets);
        // var ssids = sneakerNets.map((_) => _.ssid).toList();
        // flutterLocalNotificationsPlugin.show(
        //     SNEAKERNET_SYNC_ID, 'SneakerNets syncing...', ssids.join(','), notificationDetails);
      }
    }
  });

  // setup background tasks
  Workmanager().initialize(
    callbackDispatcher,
    // If enabled it will post a notification upon each callback event
    // isInDebugMode: true
  );
  int taskId = 0;
  // for Android, the minimum period is 15 minutes
  Workmanager().registerPeriodicTask((++taskId).toString(), scanTask_name);

  // decide which page to start based upon how we were launched
  final NotificationAppLaunchDetails? notificationAppLaunchDetails = !kIsWeb &&
          Platform.isLinux
      ? null
      : await flutterLocalNotificationsPlugin.getNotificationAppLaunchDetails();
  String initialRoute = HomePage.routeName;
  // FIXME support routing to other page upon notification
  // if (notificationAppLaunchDetails?.didNotificationLaunchApp ?? false) {
  //   selectedNotificationPayload =
  //       notificationAppLaunchDetails!.notificationResponse?.payload;
  //   initialRoute = SecondPage.routeName;
  // }
  runApp(
    MaterialApp(
      initialRoute: initialRoute,
      routes: <String, WidgetBuilder>{
        HomePage.routeName: (_) => HomePage(notificationAppLaunchDetails),
        SettingsPage.routeName: (_) => const SettingsPage(),
      },
    ),
  );
}

/*------------------------------------------------------------------------------
WorkManager Helpers
------------------------------------------------------------------------------*/
const scanTask_name = "sneakernet-wifi-scan";

@pragma(
    'vm:entry-point') // Mandatory if the App is obfuscated or using Flutter 3.1+
void callbackDispatcher() {
  Workmanager().executeTask((taskName, inputData) async {
    switch (taskName) {
      case scanTask_name:
        return WiFiScan.instance.startScan();
      default:
        return Future.error("unknown task");
    }
  });
}
