import 'dart:async';
import 'dart:io';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:sneakernet/notification_settings.dart';
import 'package:wifi_scan/wifi_scan.dart';
import 'package:workmanager/workmanager.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'pages/HomePage.dart';
import 'pages/SettingsPage.dart';

final FlutterLocalNotificationsPlugin flutterLocalNotificationsPlugin =
FlutterLocalNotificationsPlugin();

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

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
      var ssids = sneakerNets.map((_) => _.ssid).toList();
      // await flutterLocalNotificationsPlugin.show(
      //     1, 'SneakerNets found', ssids.join(','), notificationDetails);
    }
  });

  // setup background tasks
  Workmanager().initialize(
      callbackDispatcher, // The top level function, aka callbackDispatcher
      isInDebugMode: true // If enabled it will post a notification
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
        SettingsPage.routeName: (_) => SettingsPage(),
      },
    ),
  );
}

// /*------------------------------------------------------------------------------
// Notification Helpers
// ------------------------------------------------------------------------------*/
//   await flutterLocalNotificationsPlugin.initialize(
//     initializationSettings,
//     onDidReceiveNotificationResponse:
//         (NotificationResponse notificationResponse) {
//       switch (notificationResponse.notificationResponseType) {
//         case NotificationResponseType.selectedNotification:
//           selectNotificationStream.add(notificationResponse.payload);
//           break;
//         case NotificationResponseType.selectedNotificationAction:
//           if (notificationResponse.actionId == navigationActionId) {
//             selectNotificationStream.add(notificationResponse.payload);
//           }
//           break;
//       }
//     },
//     onDidReceiveBackgroundNotificationResponse: notificationTapBackground,
//   );
// }
// @pragma('vm:entry-point')
// void notificationTapBackground(NotificationResponse notificationResponse) {
//   // ignore: avoid_print
//   print('notification(${notificationResponse.id}) action tapped: '
//       '${notificationResponse.actionId} with'
//       ' payload: ${notificationResponse.payload}');
//   if (notificationResponse.input?.isNotEmpty ?? false) {
//     // ignore: avoid_print
//     print(
//         'notification action tapped with input: ${notificationResponse.input}');
//   }
// }
// final FlutterLocalNotificationsPlugin flutterLocalNotificationsPlugin =
//     FlutterLocalNotificationsPlugin();
// /// Defines a iOS/MacOS notification category for text input actions.
// const String darwinNotificationCategoryText = 'textCategory';
// /// Defines a iOS/MacOS notification category for plain actions.
// const String darwinNotificationCategoryPlain = 'plainCategory';
// /// A notification action which triggers a App navigation event
// const String navigationActionId = 'id_2';
// /// Streams are created so that app can respond to notification-related events
// /// since the plugin is initialised in the `main` function
// final StreamController<ReceivedNotification> didReceiveLocalNotificationStream =
//     StreamController<ReceivedNotification>.broadcast();
// final StreamController<String?> selectNotificationStream =
//     StreamController<String?>.broadcast();
//
// class ReceivedNotification {
//   ReceivedNotification({
//     required this.id,
//     required this.title,
//     required this.body,
//     required this.payload,
//   });
//
//   final int id;
//   final String? title;
//   final String? body;
//   final String? payload;
// }
// const AndroidNotificationDetails androidNotificationDetails =
//     AndroidNotificationDetails(
//       'your channel id',
//       'your channel name',
//       channelDescription: 'your channel description',
//       importance: Importance.max,
//       priority: Priority.high,
//       ticker: 'ticker',
//       actions: <AndroidNotificationAction>[
//         AndroidNotificationAction(
//           'text_id_1',
//           'Enter Text',
//           icon: DrawableResourceAndroidBitmap('food'),
//           inputs: <AndroidNotificationActionInput>[
//             AndroidNotificationActionInput(
//               label: 'Enter a message',
//             ),
//           ],
//         ),
//       ],
//     );
//
// const DarwinNotificationDetails darwinNotificationDetails =
//     DarwinNotificationDetails(categoryIdentifier: darwinNotificationCategoryText);
//
// const NotificationDetails notificationDetails = NotificationDetails(
//   android: androidNotificationDetails,
//   iOS: darwinNotificationDetails,
//   macOS: darwinNotificationDetails,
// );
//

/*------------------------------------------------------------------------------
WorkManager Helpers
------------------------------------------------------------------------------*/
  const scanTask_name = "sneakernet-wifi-scan";

  Future<bool> do_scanTask() async {
    final can = await WiFiScan.instance.canStartScan(askPermissions: true);
    switch (can) {
      case CanStartScan.yes:
        return WiFiScan.instance.startScan();
      default:
      // keep on trying no matter what
        return Future.value(true);
    }
  }

  @pragma(
      'vm:entry-point') // Mandatory if the App is obfuscated or using Flutter 3.1+
  void callbackDispatcher() {
    Workmanager().executeTask((taskName, inputData) async {
      switch (taskName) {
        case scanTask_name:
          return do_scanTask();
        default:
          return Future.error("unknown task");
      }
    });
  }