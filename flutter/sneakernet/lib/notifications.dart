import 'package:flutter_local_notifications/flutter_local_notifications.dart';

// unique notification ids
const SNEAKERNETS_FOUND_ID = 1;
const SNEAKERNET_SYNC_ID = 2;

// default notification details
const NotificationDetails notificationDetails = NotificationDetails(
  android: _androidNotificationDetails,
  iOS: _iosNotificationDetails,
  macOS: _macOSNotificationDetails,
  linux: _linuxNotificationDetails,
);

final FlutterLocalNotificationsPlugin flutterLocalNotificationsPlugin =
FlutterLocalNotificationsPlugin();

final InitializationSettings initializationSettings = InitializationSettings(
  android: _initializationSettingsAndroid,
  linux: _initializationSettingsLinux,
  iOS: _initializationSettingsDarwin,
  macOS: _initializationSettingsDarwin,
);

const AndroidInitializationSettings _initializationSettingsAndroid =
    AndroidInitializationSettings('@mipmap/launcher_icon');

final LinuxInitializationSettings _initializationSettingsLinux =
    LinuxInitializationSettings(
  defaultActionName: 'Open notification',
  defaultIcon: AssetsLinuxIcon('icon.png'),
);

/// A notification action which triggers a App navigation event
const String navigationActionId = 'id_3';

/// Defines a iOS/MacOS notification category for text input actions.
const String darwinNotificationCategoryText = 'textCategory';

/// Defines a iOS/MacOS notification category for plain actions.
const String darwinNotificationCategoryPlain = 'plainCategory';
final List<DarwinNotificationCategory> _darwinNotificationCategories =
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

final DarwinInitializationSettings _initializationSettingsDarwin =
    DarwinInitializationSettings(
  requestAlertPermission: false,
  requestBadgePermission: false,
  requestSoundPermission: false,
  // FIXME
  // onDidReceiveLocalNotification:
  //     (int id, String? title, String? body, String? payload) async {
  //   didReceiveLocalNotificationStream.add(
  //     ReceivedNotification(
  //       id: id,
  //       title: title,
  //       body: body,
  //       payload: payload,
  //     ),
  //   );
  // },
  notificationCategories: _darwinNotificationCategories,
);

const AndroidNotificationDetails _androidNotificationDetails =
    AndroidNotificationDetails(
  'your channel id',
  'your channel name',
  channelDescription: 'your channel description',
  importance: Importance.max,
  priority: Priority.high,
  ticker: 'ticker',
  // FIXME provide actions
  // actions: <AndroidNotificationAction>[
  //   AndroidNotificationAction(
  //     urlLaunchActionId,
  //     'Action 1',
  //     icon: DrawableResourceAndroidBitmap('food'),
  //     contextual: true,
  //   ),
  //   AndroidNotificationAction(
  //     'id_2',
  //     'Action 2',
  //     titleColor: Color.fromARGB(255, 255, 0, 0),
  //     icon: DrawableResourceAndroidBitmap('secondary_icon'),
  //   ),
  //   AndroidNotificationAction(
  //     navigationActionId,
  //     'Action 3',
  //     icon: DrawableResourceAndroidBitmap('secondary_icon'),
  //     showsUserInterface: true,
  //     // By default, Android plugin will dismiss the notification when the
  //     // user tapped on a action (this mimics the behavior on iOS).
  //     cancelNotification: false,
  //   ),
  // ],
);

const LinuxNotificationDetails _linuxNotificationDetails =
LinuxNotificationDetails(
  actions: <LinuxNotificationAction>[
    // FIXME provide actions
    // LinuxNotificationAction(
    //   key: urlLaunchActionId,
    //   label: 'Action 1',
    // ),
    // LinuxNotificationAction(
    //   key: navigationActionId,
    //   label: 'Action 2',
    // ),
  ],
);

const DarwinNotificationDetails _iosNotificationDetails =
DarwinNotificationDetails(
  categoryIdentifier: darwinNotificationCategoryPlain,
);

const DarwinNotificationDetails _macOSNotificationDetails =
DarwinNotificationDetails(
  categoryIdentifier: darwinNotificationCategoryPlain,
);