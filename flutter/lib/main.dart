import 'dart:io';
import 'dart:isolate';

import 'package:flutter/material.dart';

import 'package:permission_handler/permission_handler.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:path_provider/path_provider.dart';

import 'package:sneakernet/pages/sync.dart';
import 'package:wifi_scan/wifi_scan.dart';


import 'library.dart';
import 'settings.dart';
import 'notifications.dart';
import 'sneakernet.dart';

import 'pages/settings.dart';
import 'pages/about.dart';
import 'pages/library.dart';
import 'pages/location_permissions_request.dart';

final navigatorKey = GlobalKey<NavigatorState>();
var foundSneakerNets = List<String>.empty();

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

  // Disabled, doesn't appear you can change wifi connection from background
  // create background tasks
  // await Workmanager().initialize(callbackDispatcher);

  // DISABLED doesn't work in background
  // // subscribe to wifi scans
  // switch (await WiFiScan.instance.canGetScannedResults()) {
  //   case CanGetScannedResults.yes:
  //     final scanSubscription =
  //         WiFiScan.instance.onScannedResultsAvailable.listen((results) {
  //       var sneakerNetNodes = results
  //           .where((_) => _.ssid.startsWith(sneakerNetPrefix))
  //           .toList(growable: false);
  //
  //       if (sneakerNetNodes.isNotEmpty) {
  //         foundSneakerNets = sneakerNetNodes.map((_) => _.ssid).toList();
  //         // display a notification
  //         flutterLocalNotificationsPlugin.show(
  //             notificationFound,
  //             'Found Sneakernet(s)',
  //             foundSneakerNets.join("\n"),
  //             notificationDetails);
  //
  //         // Disabled, doesn't appear you can change wifi connection from background
  //         // queue up background sync tasks
  //         // foundSneakerNets.forEach((ssid) async {
  //         //   await Workmanager().registerOneOffTask(ssid, syncTaskName,
  //         //       existingWorkPolicy: ExistingWorkPolicy.keep,
  //         //       inputData: {
  //         //         syncTaskParamSsid: ssid,
  //         //         syncTaskParamLibraryPath: libraryDir.path,
  //         //       });
  //         // });
  //       }
  //     });
  //     WiFiScan.instance.startScan();
  //     break;
  //   default:
  //   /* do nothing */
  // }

  var initialRoute = LibraryPage.routeName;
  if (await Permission.locationAlways.isDenied) {
    // request "Location Always" access
    initialRoute = LocationPermissionsRequest.routeName;
  }
  runApp(MaterialApp(
    themeMode: ThemeMode.system,
    theme: ThemeData.light(),
    darkTheme: ThemeData.dark(),
    initialRoute: initialRoute,
    routes: <String, WidgetBuilder>{
      LibraryPage.routeName: (_) => const LibraryPage(),
      SettingsPage.routeName: (_) => const SettingsPage(),
      AboutPage.routeName: (_) => const AboutPage(),
      LocationPermissionsRequest.routeName: (_) => const LocationPermissionsRequest(),
      SyncPage.routeName: (_) => const SyncPage(),
    },
  ));
}

void onDidReceiveNotificationResponse(NotificationResponse details) {
  switch(details.id)
  {
    case notificationFound:
      navigatorKey.currentState?.pushReplacementNamed(SyncPage.routeName);
      break;
    default:
      navigatorKey.currentState?.pushReplacementNamed(LibraryPage.routeName);
  }
}

//------------------------------------------------------------------------------
// Parameters for background tasks
//------------------------------------------------------------------------------
const syncTaskName = 'sneakernet-sync';
const syncTaskParamSsid = 'sneakernet-ssid';
const syncTaskParamLibraryPath = 'libraryDir';


// Disabled, doesn't appear you can change wifi connection from background
// /*------------------------------------------------------------------------------
// WorkManager Helpers
// ------------------------------------------------------------------------------*/
//
// @pragma(
//     'vm:entry-point') // Mandatory if the App is obfuscated or using Flutter 3.1+
// void callbackDispatcher() {
//   Workmanager().executeTask((taskName, inputData) async {
//     switch (taskName) {
//       case syncTaskName:
//         // validate invocation
//         final ssid = inputData?[syncTaskParamSsid];
//         if (ssid == null) {
//           // abort the task with error
//           return Future.error("<$syncTaskParamSsid> param not found");
//         }
//
//         final libraryPath = inputData?[syncTaskParamLibraryPath];
//         if (libraryPath == null) {
//           // abort the task with error
//           return Future.error("<$syncTaskParamLibraryPath> param not found");
//         }
//         final library = Library(Directory(libraryPath));
//
//         await SneakerNet.sync(ssid, library);
//         return true;
//
//       default:
//         // abort the task with error
//         return Future.error("unknown task ${taskName}");
//     }
//   });
// }
