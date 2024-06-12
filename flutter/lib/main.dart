import 'package:path_provider/path_provider.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:permission_handler/permission_handler.dart';
import 'package:sneakernet/task.dart';


// import 'package:flutter_local_notifications/flutter_local_notifications.dart';
// import 'notifications.dart';

import 'library.dart';
import 'settings.dart';

import 'package:flutter/material.dart';
import 'pages/settings.dart';
import 'pages/about.dart';
import 'pages/library.dart';
import 'pages/sync.dart';
// import 'pages/location_permissions_request.dart';

final navigatorKey = GlobalKey<NavigatorState>();
late Settings settings;
late Library library;

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

  // ask nicely for location awareness
  await Permission.locationWhenInUse.request();

  // initialize persistent settings
  final SharedPreferences preferences = await SharedPreferences.getInstance();
  settings = Settings(preferences: preferences);

  // use a non-backed up storage for library content
  final libraryDir = await getTemporaryDirectory();
  library = Library(libraryDir);

  // enable notifications
  // flutterLocalNotificationsPlugin = FlutterLocalNotificationsPlugin();
  // await flutterLocalNotificationsPlugin
  //     .resolvePlatformSpecificImplementation<
  //         AndroidFlutterLocalNotificationsPlugin>()
  //     ?.requestNotificationsPermission();
  // await flutterLocalNotificationsPlugin.initialize(initializationSettings,
  //     onDidReceiveNotificationResponse: onDidReceiveNotificationResponse);

  // start task
  // await startForegroundTask();

  // start the app
  var initialRoute = LibraryPage.routeName;
  runApp(MaterialApp(
    themeMode: ThemeMode.system,
    theme: ThemeData.light(),
    darkTheme: ThemeData.dark(),
    initialRoute: initialRoute,
    routes: <String, WidgetBuilder>{
      LibraryPage.routeName: (_) => const LibraryPage(),
      SettingsPage.routeName: (_) => const SettingsPage(),
      AboutPage.routeName: (_) => const AboutPage(),
      SyncPage.routeName: (_) => const SyncPage(),
      // LocationPermissionsRequest.routeName: (_) =>
      //     const LocationPermissionsRequest(),
    },
  ));
}

// handle entry via notification
// void onDidReceiveNotificationResponse(NotificationResponse details) {
//   switch (details.id) {
//     case notificationFound:
//       // FIXME
//       // navigatorKey.currentState?.pushReplacementNamed(SyncPage.routeName);
//       break;
//     default:
//       navigatorKey.currentState?.pushReplacementNamed(LibraryPage.routeName);
//   }
// }
