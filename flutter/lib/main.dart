import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:wifi_scan/wifi_scan.dart';
import 'package:permission_handler/permission_handler.dart';
import 'package:workmanager/workmanager.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:path_provider/path_provider.dart';
import 'package:path/path.dart' as p;
import 'dart:async';
import 'dart:io';

import 'library.dart';
import 'settings.dart';
import 'notifications.dart';
import 'sneakernet.dart';
import 'pages/settings.dart';
import 'pages/library.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

  // request Location access
  if(await Permission.locationWhenInUse.isDenied) {
    Permission.locationWhenInUse.request();
  }

  // initialize persistent settings
  final SharedPreferences preferences = await SharedPreferences.getInstance();
  settings = Settings(preferences: preferences);

  // use a non-backed up storage for library content
  final storageDir = await getTemporaryDirectory();
  final libraryDir = Directory(p.join(storageDir.path, 'library'));
  library = Library(libraryDir);

  // initialize local notifications
  await flutterLocalNotificationsPlugin.initialize(initializationSettings);

  // subscribe to wifi scans
  final scanSubscription =
      WiFiScan.instance.onScannedResultsAvailable.listen((results) {
    var sneakerNetNodes = results
        .where((_) => _.ssid.startsWith(sneakerNetPrefix))
        .toList(growable: false);

    if (sneakerNetNodes.isNotEmpty) {
      var ssids = sneakerNetNodes.map((_) => _.ssid).toList();

      if (settings.getDoNotify()) {
        flutterLocalNotificationsPlugin.show(SNEAKERNETS_FOUND_ID,
            'SneakerNets found', ssids.join(','), notificationDetails);
      }

      if (settings.getAutoSync()) {
        Workmanager().registerOneOffTask(
            SNEAKERNET_SYNC_ID.toString(), syncTaskName,
            inputData: {
              syncTaskParamSneakernets: ssids,
              syncTaskParamLibraryPath: libraryDir.path,
            });
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
  Workmanager().registerPeriodicTask((++taskId).toString(), scanTaskName);

  // decide which page to start based upon how we were launched
  final NotificationAppLaunchDetails? notificationAppLaunchDetails = !kIsWeb &&
          Platform.isLinux
      ? null
      : await flutterLocalNotificationsPlugin.getNotificationAppLaunchDetails();

  // FIXME support routing to other page upon notification
  // if (notificationAppLaunchDetails?.didNotificationLaunchApp ?? false) {
  //   selectedNotificationPayload =
  //       notificationAppLaunchDetails!.notificationResponse?.payload;
  //   initialRoute = SecondPage.routeName;
  // }

  runApp(
    MaterialApp(
      themeMode: ThemeMode.system,
      theme: ThemeData.light(),
      darkTheme: ThemeData.dark(),
      initialRoute: LibraryPage.routeName,
      routes: <String, WidgetBuilder>{
        LibraryPage.routeName: (_) => const LibraryPage(),
        SettingsPage.routeName: (_) => const SettingsPage(),
        // HomePage.routeName: (_) => HomePage(notificationAppLaunchDetails),
      },
    ),
  );
}

/*------------------------------------------------------------------------------
WorkManager Helpers
------------------------------------------------------------------------------*/
const scanTaskName = 'wifi-scan';
const syncTaskName = 'sneakernet-sync';
const syncTaskParamSneakernets = 'sneakernets';
const syncTaskParamLibraryPath = 'libraryPath';

@pragma(
    'vm:entry-point') // Mandatory if the App is obfuscated or using Flutter 3.1+
void callbackDispatcher() {
  Workmanager().executeTask((taskName, inputData) async {
    switch (taskName) {
      case scanTaskName:
        switch (await WiFiScan.instance.canStartScan(askPermissions: true)) {
          case CanStartScan.yes:
            return WiFiScan.instance.startScan();
          default:
            return false;
        }

      case syncTaskName:
        // validate invocation
        final sneakernets = inputData?[syncTaskParamSneakernets];
        if(sneakernets == null) return Future.error("<$syncTaskParamSneakernets> param not found");
        final libraryPath = inputData?[syncTaskParamLibraryPath];
        if(libraryPath == null) return Future.error("<$syncTaskParamLibraryPath> param not found");

        final library = Library(Directory(libraryPath));
        await SneakerNet.syncAll(sneakernets, library);
        return true;

      default:
        return Future.error("unknown task");
    }
  });
}
