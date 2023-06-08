import 'dart:async';
import 'package:flutter/material.dart';
import 'package:auto_start_flutter/auto_start_flutter.dart';
import 'package:permission_handler/permission_handler.dart';
import 'package:wifi_scan/wifi_scan.dart';
import 'package:workmanager/workmanager.dart';
import 'pages/root.dart';

void main() async {
  var sneakerNets;

  WidgetsFlutterBinding.ensureInitialized();
  getAutoStartPermission();

  // setup background wifi scan for sneakernet nodes
  if(await Permission.locationWhenInUse.request().isGranted) {
    await Permission.locationAlways.request().isGranted;
  }
  final scanSubscription = WiFiScan.instance.onScannedResultsAvailable.listen((results) {
    const SNEAKER_NET_SSID = "SneakerNet";
    sneakerNets = results.where((_) => _.ssid.startsWith(SNEAKER_NET_SSID)).toList(growable: false);
  });
  Workmanager().initialize(
      callbackDispatcher, // The top level function, aka callbackDispatcher
      isInDebugMode: false // If enabled it will post a notification whenever the task is running. Handy for debugging tasks
  );
  int taskId = 0;
  Workmanager().registerPeriodicTask((++taskId).toString(), scanTask_name,
    frequency: const Duration(minutes: 1));

  runApp(const MyApp());
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
      home: root(title: "Sneaker Net")
    );
  }
}

/*------------------------------------------------------------------------------
Background Tasks
------------------------------------------------------------------------------*/
const scanTask_name = "sneakernet-wifi-scan";

@pragma('vm:entry-point') // Mandatory if the App is obfuscated or using Flutter 3.1+
void callbackDispatcher() {
  Workmanager().executeTask((task_name, inputData) async {
    switch(task_name) {
      case scanTask_name:
        final can = await WiFiScan.instance.canStartScan(askPermissions: true);
        switch(can) {
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


