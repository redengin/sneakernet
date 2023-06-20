import 'package:flutter_local_notifications/flutter_local_notifications.dart';

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

