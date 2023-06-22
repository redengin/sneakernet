import 'dart:io';
import 'package:flutter/material.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'package:sneakernet/notifications.dart';
import 'package:wifi_scan/wifi_scan.dart';
import 'package:sneakernet/pages/SettingsPage.dart';

class HomePage extends StatefulWidget {
  static const String routeName = '/';

  final NotificationAppLaunchDetails? notificationAppLaunchDetails;
  const HomePage(this.notificationAppLaunchDetails, {super.key});

  bool get didNotificationLaunchApp =>
      notificationAppLaunchDetails?.didNotificationLaunchApp ?? false;

  @override
  _HomePageState createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  final TextEditingController _linuxIconPathController =
      TextEditingController();

  bool _notificationsEnabled = false;
  bool _wifiScanEnabled = false;

  @override
  void initState() {
    super.initState();
    _isAndroidNotificationPermissionGranted();
    _requestNotificationPermissions();
    _isWifiScanPermissionGranted();
    // _configureDidReceiveLocalNotificationSubject();
    // _configureSelectNotificationSubject();
  }

  @override
  Widget build(BuildContext context) => Scaffold(
        appBar: AppBar(
          centerTitle: true,
          title: const Text('SneakerNet'),
          backgroundColor: Theme.of(context).colorScheme.primary,
        ),
        drawer: Drawer(
          child: ListView(padding: EdgeInsets.zero, children: [
            SizedBox(
              height: 100,
              child: const DrawerHeader(
                decoration: BoxDecoration(color: Colors.blue),
                child: Text('SneakerNet',
                    style: TextStyle(fontSize: 30, color: Colors.white)),
              ),
            ),
            ListTile(
                leading: const Icon(Icons.settings),
                title: const Text('Settings'),
                onTap: () {
                  Navigator.pop(context);
                  Navigator.pushNamed(context, SettingsPage.routeName);
                })
          ]),
        ),
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
            ],
          ),
        ),
      );

  Future<void> _isAndroidNotificationPermissionGranted() async {
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

  Future<void> _requestNotificationPermissions() async {
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

  Future<void> _isWifiScanPermissionGranted() async {
    // https://pub.dev/documentation/wifi_scan/latest/wifi_scan/CanStartScan.html
    final bool granted =
        await WiFiScan.instance.canStartScan(askPermissions: true) ==
            CanStartScan.yes;
    setState(() {
      _wifiScanEnabled = granted;
    });
  }
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
