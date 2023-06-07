import 'package:flutter/material.dart';
import 'dart:async';
import 'package:location_permissions/location_permissions.dart';
import 'package:wifi_scan/wifi_scan.dart';
import 'package:workmanager/workmanager.dart';
import 'pages/root.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();


  // setup background wifi scan for sneakernet nodes
  PermissionStatus permission = await LocationPermissions().requestPermissions(permissionLevel: LocationPermissionLevel.locationAlways);
  StreamSubscription<List<WiFiAccessPoint>> subscription = WiFiScan.instance.onScannedResultsAvailable.listen((results) => _handleScanResults(results));
  Workmanager().initialize(
      callbackDispatcher, // The top level function, aka callbackDispatcher
      isInDebugMode: true // If enabled it will post a notification whenever the task is running. Handy for debugging tasks
  );
  int taskId = 0;
  Workmanager().registerPeriodicTask((++taskId).toString(), scanTask_name,
    backoffPolicy: BackoffPolicy.exponential, backoffPolicyDelay: Duration(seconds: 10));

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
            return Future.error("unable to scan wifi");
        }
      default:
        return Future.error("unknown task");
    }
  });
}

const SNEAKER_NET_SSID = "SneakerNet";
_handleScanResults(results) {
  results.removeWhere((wifiNode) => ! wifiNode.ssid.startsWith(SNEAKER_NET_SSID));
}

