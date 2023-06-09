import 'dart:async';
import 'dart:io';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:auto_start_flutter/auto_start_flutter.dart';
import 'package:permission_handler/permission_handler.dart';
import 'package:wifi_scan/wifi_scan.dart';
import 'package:workmanager/workmanager.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'pages/root.dart';

void main() {
  // TODO decide which page to start based upon how we were launched
  // final NotificationAppLaunchDetails? notificationAppLaunchDetails = !kIsWeb &&
  //     Platform.isLinux
  //     ? null
  //     : await flutterLocalNotificationsPlugin.getNotificationAppLaunchDetails();
  // String initialRoute = MyApp.routeName;
  // if (notificationAppLaunchDetails?.didNotificationLaunchApp ?? false) {
  //   selectedNotificationPayload =
  //       notificationAppLaunchDetails!.notificationResponse?.payload;
  //   initialRoute = SecondPage.routeName;
  // }
  runApp(const MyApp());

  WidgetsFlutterBinding.ensureInitialized();
  // FIXME prompt user to give app auto-start permissions
  // getAutoStartPermission();

  _initializeNotificationServices();

  // setup background wifi scan for sneakernet nodes
  // FIXME
  // if (await Permission.locationWhenInUse.request().isGranted) {
  //   await Permission.locationAlways.request().isGranted;
  // }
  final void scanSubscription =
      WiFiScan.instance.onScannedResultsAvailable.listen((results) async {
    const SNEAKER_NET_SSID = "SneakerNet";
    var sneakerNets = results
        .where((_) => _.ssid.startsWith(SNEAKER_NET_SSID))
        .toList(growable: false);

    if (sneakerNets.isNotEmpty) {
      var ssids = sneakerNets.map((_) => _.ssid).toList();
      await flutterLocalNotificationsPlugin.show(
          1, 'SneakerNets found', ssids.join(','), notificationDetails);
    }
  });
  Workmanager().initialize(
      callbackDispatcher, // The top level function, aka callbackDispatcher
      isInDebugMode:
          true // If enabled it will post a notification whenever the task is running. Handy for debugging tasks
      );
  int taskId = 0;
  Workmanager().registerPeriodicTask((++taskId).toString(), scanTask_name,
      frequency: const Duration(minutes: 1));

}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
        title: 'Flutter Demo',
        theme: ThemeData(
          colorScheme: ColorScheme.fromSeed(seedColor: Colors.indigo),
          useMaterial3: true,
        ),
        home: root(title: "Sneaker Net"));
  }
}


/*------------------------------------------------------------------------------
Notification Helpers
------------------------------------------------------------------------------*/
void _initializeNotificationServices() async {
  const AndroidInitializationSettings initializationSettingsAndroid =
  AndroidInitializationSettings('@mipmap/launcher_icon');
  final List<DarwinNotificationCategory> darwinNotificationCategories =
  <DarwinNotificationCategory>[
    DarwinNotificationCategory(
      darwinNotificationCategoryText,
      actions: <DarwinNotificationAction>[
        DarwinNotificationAction.text(
          'text_1',
          'Action 1',
          buttonTitle: 'Send',
          placeholder: 'Placeholder',
        ),
      ],
    ),
    DarwinNotificationCategory(
      darwinNotificationCategoryPlain,
      actions: <DarwinNotificationAction>[
        DarwinNotificationAction.plain('id_1', 'Action 1'),
        DarwinNotificationAction.plain(
          'id_2',
          'Action 2 (destructive)',
          options: <DarwinNotificationActionOption>{
            DarwinNotificationActionOption.destructive,
          },
        ),
        DarwinNotificationAction.plain(
          navigationActionId,
          'Action 3 (foreground)',
          options: <DarwinNotificationActionOption>{
            DarwinNotificationActionOption.foreground,
          },
        ),
        DarwinNotificationAction.plain(
          'id_4',
          'Action 4 (auth required)',
          options: <DarwinNotificationActionOption>{
            DarwinNotificationActionOption.authenticationRequired,
          },
        ),
      ],
      options: <DarwinNotificationCategoryOption>{
        DarwinNotificationCategoryOption.hiddenPreviewShowTitle,
      },
    )
  ];
  /// Note: permissions aren't requested here just to demonstrate that can be done later
  final DarwinInitializationSettings initializationSettingsDarwin =
  DarwinInitializationSettings(
    requestAlertPermission: false,
    requestBadgePermission: false,
    requestSoundPermission: false,
    onDidReceiveLocalNotification:
        (int id, String? title, String? body, String? payload) async {
      didReceiveLocalNotificationStream.add(
        ReceivedNotification(
          id: id,
          title: title,
          body: body,
          payload: payload,
        ),
      );
    },
    notificationCategories: darwinNotificationCategories,
  );
  final LinuxInitializationSettings initializationSettingsLinux =
  LinuxInitializationSettings(
    defaultActionName: 'Open notification',
    defaultIcon: AssetsLinuxIcon('icons/app_icon.png'),
  );
  final InitializationSettings initializationSettings = InitializationSettings(
    android: initializationSettingsAndroid,
    iOS: initializationSettingsDarwin,
    macOS: initializationSettingsDarwin,
    linux: initializationSettingsLinux,
  );
  await flutterLocalNotificationsPlugin.initialize(
    initializationSettings,
    onDidReceiveNotificationResponse:
        (NotificationResponse notificationResponse) {
      switch (notificationResponse.notificationResponseType) {
        case NotificationResponseType.selectedNotification:
          selectNotificationStream.add(notificationResponse.payload);
          break;
        case NotificationResponseType.selectedNotificationAction:
          if (notificationResponse.actionId == navigationActionId) {
            selectNotificationStream.add(notificationResponse.payload);
          }
          break;
      }
    },
    onDidReceiveBackgroundNotificationResponse: notificationTapBackground,
  );
}
@pragma('vm:entry-point')
void notificationTapBackground(NotificationResponse notificationResponse) {
  // ignore: avoid_print
  print('notification(${notificationResponse.id}) action tapped: '
      '${notificationResponse.actionId} with'
      ' payload: ${notificationResponse.payload}');
  if (notificationResponse.input?.isNotEmpty ?? false) {
    // ignore: avoid_print
    print(
        'notification action tapped with input: ${notificationResponse.input}');
  }
}
final FlutterLocalNotificationsPlugin flutterLocalNotificationsPlugin =
    FlutterLocalNotificationsPlugin();
/// Defines a iOS/MacOS notification category for text input actions.
const String darwinNotificationCategoryText = 'textCategory';
/// Defines a iOS/MacOS notification category for plain actions.
const String darwinNotificationCategoryPlain = 'plainCategory';
/// A notification action which triggers a App navigation event
const String navigationActionId = 'id_2';
/// Streams are created so that app can respond to notification-related events
/// since the plugin is initialised in the `main` function
final StreamController<ReceivedNotification> didReceiveLocalNotificationStream =
    StreamController<ReceivedNotification>.broadcast();
final StreamController<String?> selectNotificationStream =
    StreamController<String?>.broadcast();

class ReceivedNotification {
  ReceivedNotification({
    required this.id,
    required this.title,
    required this.body,
    required this.payload,
  });

  final int id;
  final String? title;
  final String? body;
  final String? payload;
}
const AndroidNotificationDetails androidNotificationDetails =
    AndroidNotificationDetails(
      'your channel id',
      'your channel name',
      channelDescription: 'your channel description',
      importance: Importance.max,
      priority: Priority.high,
      ticker: 'ticker',
      actions: <AndroidNotificationAction>[
        AndroidNotificationAction(
          'text_id_1',
          'Enter Text',
          icon: DrawableResourceAndroidBitmap('food'),
          inputs: <AndroidNotificationActionInput>[
            AndroidNotificationActionInput(
              label: 'Enter a message',
            ),
          ],
        ),
      ],
    );

const DarwinNotificationDetails darwinNotificationDetails =
    DarwinNotificationDetails(categoryIdentifier: darwinNotificationCategoryText);

const NotificationDetails notificationDetails = NotificationDetails(
  android: androidNotificationDetails,
  iOS: darwinNotificationDetails,
  macOS: darwinNotificationDetails,
);

/*------------------------------------------------------------------------------
WorkManager Helpers
------------------------------------------------------------------------------*/
const scanTask_name = "sneakernet-wifi-scan";

@pragma('vm:entry-point') // Mandatory if the App is obfuscated or using Flutter 3.1+
void callbackDispatcher() {
  Workmanager().executeTask((task_name, inputData) async {
    switch (task_name) {
      case scanTask_name:
        final can = await WiFiScan.instance.canStartScan(askPermissions: true);
        switch (can) {
          case CanStartScan.yes:
            return WiFiScan.instance.startScan();
          default:
            // keep on trying no matter what
            return Future.value(true);
        }
      default:
        return Future.error("unknown task");
    }
  });
}
