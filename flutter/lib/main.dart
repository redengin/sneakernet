import 'dart:io';
import 'dart:isolate';

import 'package:flutter/material.dart';

import 'package:path_provider/path_provider.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:permission_handler/permission_handler.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'package:flutter_foreground_task/flutter_foreground_task.dart';

import 'sneakernet.dart';
import 'library.dart';
import 'settings.dart';
import 'notifications.dart';

import 'pages/settings.dart';
import 'pages/about.dart';
import 'pages/library.dart';
import 'pages/location_permissions_request.dart';
// import 'pages/sync.dart';

final navigatorKey = GlobalKey<NavigatorState>();

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

  // start foreground task
  await _startForegroundTask();

  // use a page to guide permission access
  var initialRoute = LibraryPage.routeName;
  if (await Permission.locationAlways.isDenied) {
    // request "Location Always" access
    initialRoute = LocationPermissionsRequest.routeName;
  }

  // start the app
  runApp(MaterialApp(
    themeMode: ThemeMode.system,
    theme: ThemeData.light(),
    darkTheme: ThemeData.dark(),
    initialRoute: initialRoute,
    routes: <String, WidgetBuilder>{
      LibraryPage.routeName: (_) => const LibraryPage(),
      SettingsPage.routeName: (_) => const SettingsPage(),
      AboutPage.routeName: (_) => const AboutPage(),
      LocationPermissionsRequest.routeName: (_) =>
          const LocationPermissionsRequest(),
      // SyncPage.routeName: (_) => const SyncPage(),
    },
  ));
}

void onDidReceiveNotificationResponse(NotificationResponse details) {
  switch (details.id) {
    case notificationFound:
      // FIXME
      // navigatorKey.currentState?.pushReplacementNamed(SyncPage.routeName);
      break;
    default:
      navigatorKey.currentState?.pushReplacementNamed(LibraryPage.routeName);
  }
}

//------------------------------------------------------------------------------
// Parameters for tasks
//------------------------------------------------------------------------------
const syncTaskName = 'sneakernet-sync';
const syncTaskParamSsid = 'sneakernet-ssid';
const syncTaskParamLibraryPath = 'libraryDir';

Future<void> _startForegroundTask() async {
  await _acquireForegroundPermissions();
  _initForegroundTask();
  await _createForegroundTask();
}

Future<void> _acquireForegroundPermissions() async {
  if (Platform.isAndroid) {
    // "android.permission.SYSTEM_ALERT_WINDOW" permission must be granted for // // onNotificationPressed function to be called.
    // if (!await FlutterForegroundTask.canDrawOverlays) {
    //   // This function requires `android.permission.SYSTEM_ALERT_WINDOW` permission.
    //   await FlutterForegroundTask.openSystemAlertWindowSettings();
    // }
    // Android 12 or higher, there are restrictions on starting a foreground service.
    // To restart the service on device reboot or unexpected problem, you need to allow below permission.
    if (!await FlutterForegroundTask.isIgnoringBatteryOptimizations) {
      // This function requires `android.permission.REQUEST_IGNORE_BATTERY_OPTIMIZATIONS` permission.
      await FlutterForegroundTask.requestIgnoreBatteryOptimization();
    }
    // Android 13 and higher, you need to allow notification permission to expose foreground service notification.
    final NotificationPermission notificationPermissionStatus =
        await FlutterForegroundTask.checkNotificationPermission();
    if (notificationPermissionStatus != NotificationPermission.granted) {
      await FlutterForegroundTask.requestNotificationPermission();
    }
  }
}

void _initForegroundTask() {
  FlutterForegroundTask.init(
    androidNotificationOptions: AndroidNotificationOptions(
      foregroundServiceType: AndroidForegroundServiceType.DATA_SYNC,
      channelId: 'foreground_service',
      channelName: 'Foreground Service Notification',
      channelDescription:
      'This notification appears when the foreground service is running.',
      channelImportance: NotificationChannelImportance.LOW,
      priority: NotificationPriority.LOW,
      iconData: const NotificationIconData(
        resType: ResourceType.drawable,
        resPrefix: ResourcePrefix.ic,
        name: 'launcher_foreground',
        // backgroundColor: Colors.orange,
      ),
    ),
    iosNotificationOptions: const IOSNotificationOptions(
      showNotification: true,
      playSound: false,
    ),
    foregroundTaskOptions: const ForegroundTaskOptions(
      interval: 5000,
      isOnceEvent: false,
      autoRunOnBoot: true,
      allowWakeLock: true,
      allowWifiLock: true,
    ),
  );
}
ReceivePort? _receivePort;
Future<bool> _createForegroundTask() async {
  // You can save data using the saveData function.
  await FlutterForegroundTask.saveData(key: 'customData', value: 'hello');

  // Register the receivePort before starting the service.
  final ReceivePort? receivePort = FlutterForegroundTask.receivePort;
  final bool isRegistered = _registerReceivePort(receivePort);
  if (!isRegistered) {
    print('Failed to register receivePort!');
    return false;
  }

  if (await FlutterForegroundTask.isRunningService) {
    return FlutterForegroundTask.restartService();
  } else {
    return FlutterForegroundTask.startService(
      notificationTitle: 'Monitoring for SneakerNet nodes',
      notificationText: '',
      callback: foregroundCallback,
    );
  }
}

bool _registerReceivePort(ReceivePort? newReceivePort) {
  if (newReceivePort == null) {
    return false;
  }

  _closeReceivePort();

  _receivePort = newReceivePort;
  _receivePort?.listen((data) {
    // if (data is int) {
    //   print('eventCount: $data');
    // } else if (data is String) {
    //   if (data == 'onNotificationPressed') {
    //     Navigator.of(context).pushNamed('/resume-route');
    //   }
    // } else if (data is DateTime) {
    //   print('timestamp: ${data.toString()}');
    // }
  });

  return _receivePort != null;
}

void _closeReceivePort() {
  _receivePort?.close();
  _receivePort = null;
}


@pragma('vm:entry-point')
void foregroundCallback() {
  // The setTaskHandler function must be called to handle the task in the background.
  FlutterForegroundTask.setTaskHandler(SneakernetTaskHandler());
}

class SneakernetTaskHandler extends TaskHandler {
  @override
  void onRepeatEvent(DateTime timestamp, SendPort? sendPort) {
    // TODO: implement onRepeatEvent
  }

  @override
  void onStart(DateTime timestamp, SendPort? sendPort) {
    // TODO: implement onStart
  }

  @override
  void onDestroy(DateTime timestamp, SendPort? sendPort) {
    // TODO: implement onDestroy
  }
}

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
