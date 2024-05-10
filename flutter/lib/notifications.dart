import 'package:flutter_local_notifications/flutter_local_notifications.dart';

final flutterLocalNotificationsPlugin = FlutterLocalNotificationsPlugin();
const AndroidInitializationSettings initializationSettingsAndroid =
    AndroidInitializationSettings('@drawable/ic_launcher_foreground');
final InitializationSettings initializationSettings =
    InitializationSettings(android: initializationSettingsAndroid);

const channelId = 'sneakernet';
const channelName = 'sneakernet';
const AndroidNotificationDetails androidNotificationDetails =
    AndroidNotificationDetails(channelId, channelName,
        channelDescription: 'sneakernet notifications',
        importance: Importance.max,
        priority: Priority.high,
        ticker: 'ticker');
const NotificationDetails notificationDetails =
    NotificationDetails(android: androidNotificationDetails);

const notificationFound = 0;
const notificationSync = 1;