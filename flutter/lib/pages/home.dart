import 'dart:io';
import 'package:flutter/material.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'package:wifi_scan/wifi_scan.dart';
import 'package:flutter_file_dialog/flutter_file_dialog.dart';

import '../notifications.dart';
import '../library.dart';
// pages
import 'settings.dart';
import 'library.dart';

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

  @override
  void initState() {
    super.initState();
    _requestNotificationPermissions();
    _requestWifiScanPermission();
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
            const SizedBox(
              height: 100,
              child: DrawerHeader(
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
            mainAxisAlignment: MainAxisAlignment.start,
            children: <Widget>[
              Card(
                  child: Column(
                children: [
                  const ListTile(
                    leading: Icon(Icons.library_books),
                    title: Text('Library'),
                    subtitle: Text('SneakerNet content stored on your phone'),
                  ),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.end,
                    children: [
                      TextButton(
                        child: Text('Browse'),
                        onPressed: () {
                          Navigator.pushNamed(context, LibraryPage.routeName);
                        },
                      ),
                      TextButton(
                        child: Icon(Icons.add_circle),
                        onPressed: () {
                          // add book from Downloads
                          const params = OpenFileDialogParams(
                            mimeTypesFilter: ['application/epub+zip'],
                          );
                          final path = FlutterFileDialog.pickFile(params: params);
                            Library.import(path);
                        },
                      ),
                    ],
                  ),
                ],
              )),
            ],
          ),
        ),
      );

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

      await androidImplementation?.requestPermission();
    }
  }

  Future<void> _requestWifiScanPermission() async {
    // https://pub.dev/documentation/wifi_scan/latest/wifi_scan/CanStartScan.html
    await WiFiScan.instance.canStartScan(askPermissions: true);
  }
}
