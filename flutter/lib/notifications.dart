import 'package:flutter_local_notifications/flutter_local_notifications.dart';

final InitializationSettings initializationSettings = InitializationSettings(
  android: initializationSettingsAndroid,
  // iOS: initializationSettingsDarwin
);

const AndroidInitializationSettings initializationSettingsAndroid =
    AndroidInitializationSettings('@drawable/ic_launcher_foreground');

// const DarwinInitializationSettings initializationSettingsDarwin =
// DarwinInitializationSettings(
//     onDidReceiveLocalNotification: onDidReceiveLocalNotification);
// void onDidReceiveLocalNotification(
//     int id, String? title, String? body, String? payload) async {
//     // display a dialog with the notification details, tap ok to go to another page
//     showDialog(
//         context: context,
//         builder: (BuildContext context) => CupertinoAlertDialog(
//             title: Text(title??''),
//             content: Text(body??''),
//             actions: [
//                 CupertinoDialogAction(
//                     isDefaultAction: true,
//                     child: Text('Ok'),
//                     onPressed: () async {
//                         Navigator.of(context, rootNavigator: true).pop();
//                         await Navigator.push(
//                             context,
//                             MaterialPageRoute(
//                                 builder: (context) => SecondScreen(payload),
//                             ),
//                         );
//                     },
//                 )
//             ],
//         ),
//     );
// }

const channelId = 'sneakernet-notification';
const channelName = channelId;

AndroidNotificationDetails androidNotificationDetails =
    AndroidNotificationDetails(channelId, channelName,
        channelDescription: 'sneakernet notifications',
        importance: Importance.max,
        priority: Priority.high,
        enableVibration: true,
        // vibrationPattern: Int64List.fromList([0, 1000, 5000, 2000]),
        ticker: 'ticker');
NotificationDetails notificationDetails =
    NotificationDetails(android: androidNotificationDetails);

const notificationFound = 0;
const notificationSync = 1;
