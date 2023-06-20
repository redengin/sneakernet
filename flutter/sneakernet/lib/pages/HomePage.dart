import 'dart:io';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'package:flutter/material.dart';
import '../main.dart';
import 'SettingsPage.dart';

class HomePage extends StatefulWidget {
  static const String routeName = '/';

  const HomePage(this.notificationAppLaunchDetails, {Key? key})
      : super(key: key);

  final NotificationAppLaunchDetails? notificationAppLaunchDetails;

  bool get didNotificationLaunchApp =>
      notificationAppLaunchDetails?.didNotificationLaunchApp ?? false;

  @override
  _HomePageState createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  final TextEditingController _linuxIconPathController =
      TextEditingController();

  bool _notificationsEnabled = false;

  @override
  void initState() {
    super.initState();
    _isAndroidPermissionGranted();
    _requestPermissions();
    // _configureDidReceiveLocalNotificationSubject();
    // _configureSelectNotificationSubject();
  }

  @override
  Widget build(BuildContext context) => Scaffold(
        appBar: AppBar(
            backgroundColor: Theme.of(context).colorScheme.inversePrimary,
            title: const Text('SneakerNet'),
            actions: [
              IconButton(
                icon: const Icon(Icons.settings),
                tooltip: 'settings',
                onPressed: () {
                  Navigator.push(context,
                      MaterialPageRoute(builder: (context) => SettingsPage()));
                },
              )
            ]),
        body: Center(
          // Center is a layout widget. It takes a single child and positions it
          // in the middle of the parent.
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              PaddedElevatedButton(
                buttonText: 'Show plain notification with payload',
                onPressed: () async {
                  await _showNotification();
                },
              ),
              // const Text(
              //   'You have pushed the button this many times:',
              // ),
              // Text(
              //   '$_counter',
              //   style: Theme.of(context).textTheme.headlineMedium,
              // ),
            ],
          ),
        ),
        // floatingActionButton: FloatingActionButton(
        //   // onPressed: _incrementCounter,
        //   // tooltip: 'Increment',
        //   // child: const Icon(Icons.add),
        // ), // This trailing comma makes auto-formatting nicer for build methods.
      );

  Future<void> _isAndroidPermissionGranted() async {
    if (Platform.isAndroid) {
      final bool granted = await flutterLocalNotificationsPlugin
              .resolvePlatformSpecificImplementation<
                  AndroidFlutterLocalNotificationsPlugin>()
              ?.areNotificationsEnabled() ??
          false;

      setState(() {
        _notificationsEnabled = granted;
      });
    }
  }

  Future<void> _requestPermissions() async {
    if (Platform.isIOS || Platform.isMacOS) {
      await flutterLocalNotificationsPlugin
          .resolvePlatformSpecificImplementation<
              IOSFlutterLocalNotificationsPlugin>()
          ?.requestPermissions(
            alert: true,
            badge: true,
            sound: true,
          );
      await flutterLocalNotificationsPlugin
          .resolvePlatformSpecificImplementation<
              MacOSFlutterLocalNotificationsPlugin>()
          ?.requestPermissions(
            alert: true,
            badge: true,
            sound: true,
          );
    } else if (Platform.isAndroid) {
      final AndroidFlutterLocalNotificationsPlugin? androidImplementation =
          flutterLocalNotificationsPlugin.resolvePlatformSpecificImplementation<
              AndroidFlutterLocalNotificationsPlugin>();

      final bool? granted = await androidImplementation?.requestPermission();
      setState(() {
        _notificationsEnabled = granted ?? false;
      });
    }
  }

// void _configureDidReceiveLocalNotificationSubject() {
//   didReceiveLocalNotificationStream.stream
//       .listen((ReceivedNotification receivedNotification) async {
//     await showDialog(
//       context: context,
//       builder: (BuildContext context) => CupertinoAlertDialog(
//         title: receivedNotification.title != null
//             ? Text(receivedNotification.title!)
//             : null,
//         content: receivedNotification.body != null
//             ? Text(receivedNotification.body!)
//             : null,
//         actions: <Widget>[
//           CupertinoDialogAction(
//             isDefaultAction: true,
//             onPressed: () async {
//               Navigator.of(context, rootNavigator: true).pop();
//               await Navigator.of(context).push(
//                 MaterialPageRoute<void>(
//                   builder: (BuildContext context) =>
//                       SecondPage(receivedNotification.payload),
//                 ),
//               );
//             },
//             child: const Text('Ok'),
//           )
//         ],
//       ),
//     );
//   });
// }
//
// void _configureSelectNotificationSubject() {
//   selectNotificationStream.stream.listen((String? payload) async {
//     await Navigator.of(context).push(MaterialPageRoute<void>(
//       builder: (BuildContext context) => SecondPage(payload),
//     ));
//   });
// }
//
// @override
// void dispose() {
//   didReceiveLocalNotificationStream.close();
//   selectNotificationStream.close();
//   super.dispose();
// }
}

// FIXME test use only
//------------------------------------------------------------------------------
int id = 1;
class PaddedElevatedButton extends StatelessWidget {
  const PaddedElevatedButton({
    required this.buttonText,
    required this.onPressed,
    Key? key,
  }) : super(key: key);

  final String buttonText;
  final VoidCallback onPressed;

  @override
  Widget build(BuildContext context) => Padding(
    padding: const EdgeInsets.fromLTRB(0, 0, 0, 8),
    child: ElevatedButton(
      onPressed: onPressed,
      child: Text(buttonText),
    ),
  );
}
Future<void> _showNotification() async {
  const AndroidNotificationDetails androidNotificationDetails =
  AndroidNotificationDetails('your channel id', 'your channel name',
      channelDescription: 'your channel description',
      importance: Importance.max,
      priority: Priority.high,
      ticker: 'ticker');
  const NotificationDetails notificationDetails =
  NotificationDetails(android: androidNotificationDetails);
  await flutterLocalNotificationsPlugin.show(
      id++, 'plain title', 'plain body', notificationDetails,
      payload: 'item x');
}
//------------------------------------------------------------------------------