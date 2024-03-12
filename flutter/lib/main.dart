import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';

import 'package:shared_preferences/shared_preferences.dart';
import 'package:path_provider/path_provider.dart';

import 'package:permission_handler/permission_handler.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'package:wifi_scan/wifi_scan.dart';

import 'package:workmanager/workmanager.dart';

import 'library.dart';
import 'settings.dart';
import 'notifications.dart';
import 'sneakernet.dart';

// import 'pages/settings.dart';
import 'pages/about.dart';
import 'pages/library.dart';
import 'pages/location_permissions_request.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

  // initialize persistent settings
  final SharedPreferences preferences = await SharedPreferences.getInstance();
  settings = Settings(preferences: preferences);

  // use a non-backed up storage for library content
  final libraryDir = await getTemporaryDirectory();
  library = Library(libraryDir);

  // enable notifications
  await flutterLocalNotificationsPlugin
      .resolvePlatformSpecificImplementation<
          AndroidFlutterLocalNotificationsPlugin>()
      ?.requestNotificationsPermission();
  await flutterLocalNotificationsPlugin.initialize(initializationSettings,
      onDidReceiveNotificationResponse: onDidReceiveNotificationResponse);

  // create background tasks
  await Workmanager().initialize(callbackDispatcher);

  // subscribe to wifi scans
  switch (await WiFiScan.instance.canGetScannedResults()) {
    case CanGetScannedResults.yes:
      final scanSubscription =
          WiFiScan.instance.onScannedResultsAvailable.listen((results) {
        var sneakerNetNodes = results
            .where((_) => _.ssid.startsWith(sneakerNetPrefix))
            .toList(growable: false);

        if (sneakerNetNodes.isNotEmpty) {
          var ssids = sneakerNetNodes.map((_) => _.ssid).toList();
          // display a notification
          flutterLocalNotificationsPlugin.show(notificationFoundSneakerNetsId,
              'Found Sneakernet(s)', ssids.join("\n"), notificationDetails);

          // queue up background sync tasks
          ssids.forEach((ssid) async {
            await Workmanager().registerOneOffTask(ssid, syncTaskName,
                existingWorkPolicy: ExistingWorkPolicy.append,
                inputData: {
                  syncTaskParamSsid: ssid,
                  syncTaskParamLibraryPath: libraryDir.path,
                });
          });
        }
      });
      WiFiScan.instance.startScan();
      break;
    default: /* do nothing */
  }

  // decide which page to start based upon how we were launched
  // final NotificationAppLaunchDetails? notificationAppLaunchDetails = !kIsWeb &&
  //         Platform.isLinux
  //     ? null
  //     : await flutterLocalNotificationsPlugin.getNotificationAppLaunchDetails();

  var initialRoute = LibraryPage.routeName;

  // request Location access if not granted
  if (await Permission.locationAlways.isDenied) {
    initialRoute = LocationPermissionsRequest.routeName;
  }
  // TODO support routing to other page upon notification
  // if (notificationAppLaunchDetails?.didNotificationLaunchApp ?? false) {
  //   selectedNotificationPayload =
  //       notificationAppLaunchDetails!.notificationResponse?.payload;
  //   initialRoute = SecondPage.routeName;
  // }

  runApp(MaterialApp(
    themeMode: ThemeMode.system,
    theme: ThemeData.light(),
    darkTheme: ThemeData.dark(),
    initialRoute: initialRoute,
    routes: <String, WidgetBuilder>{
      LibraryPage.routeName: (_) => const LibraryPage(),
      // SettingsPage.routeName: (_) => const SettingsPage(),
      AboutPage.routeName: (_) => const AboutPage(),
      LocationPermissionsRequest.routeName: (_) => LocationPermissionsRequest(),
      // HomePage.routeName: (_) => HomePage(notificationAppLaunchDetails),
    },
  ));
}

void onDidReceiveNotificationResponse(NotificationResponse details) {}

/*------------------------------------------------------------------------------
WorkManager Helpers
------------------------------------------------------------------------------*/
const syncTaskName = 'sneakernet-sync';
const syncTaskParamSsid = 'sneakernet-ssid';
const syncTaskParamLibraryPath = 'libraryDir';

@pragma(
    'vm:entry-point') // Mandatory if the App is obfuscated or using Flutter 3.1+
void callbackDispatcher() {
  Workmanager().executeTask((taskName, inputData) async {
    switch (taskName) {
      case syncTaskName:
        // validate invocation
        final ssid = inputData?[syncTaskParamSsid];
        if (ssid == null) {
          // abort the task with error
          return Future.error("<$syncTaskParamSsid> param not found");
        }

        final libraryPath = inputData?[syncTaskParamLibraryPath];
        if (libraryPath == null) {
          // abort the task with error
          return Future.error("<$syncTaskParamLibraryPath> param not found");
        }
        final library = Library(Directory(libraryPath));
        await SneakerNet.sync(ssid, library);
        return true;

      default:
        // abort the task with error
        return Future.error("unknown task ${taskName}");
    }
  });
}
