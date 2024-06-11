import 'dart:async';
import 'dart:io';
import 'dart:isolate';
import 'package:flutter/widgets.dart';
import 'package:flutter_foreground_task/flutter_foreground_task.dart';
import 'package:path_provider/path_provider.dart';
import 'package:wifi_scan/wifi_scan.dart';

import 'package:sneakernet/library.dart';
import 'package:sneakernet/settings.dart';
import 'package:sneakernet/sneakernet.dart';



//------------------------------------------------------------------------------
// Parameters for tasks
//------------------------------------------------------------------------------
const taskTitle = "Monitoring SneakerNet";
const syncTaskName = 'sneakernet-sync';
const syncTaskParamSsid = 'sneakernet-ssid';
const syncTaskParamLibraryPath = 'libraryDir';

Future<void> startForegroundTask() async {
  await _acquireForegroundPermissions();
  _initForegroundTask();
  await _createForegroundTask();
}

Future<void> _acquireForegroundPermissions() async {
  if (Platform.isAndroid) {
    // "android.permission.SYSTEM_ALERT_WINDOW" permission must be granted for
    // onNotificationPressed function to be called.
    // FIXME this is a shitty method
    // if (!await FlutterForegroundTask.canDrawOverlays) {
    //   // This function requires `android.permission.SYSTEM_ALERT_WINDOW` permission.
    //   await FlutterForegroundTask.openSystemAlertWindowSettings();
    // }
    // Android 12 or higher, there are restrictions on starting a foreground service.
    // To restart the service on device reboot or unexpected problem, you need to allow below permission.
    if (!await FlutterForegroundTask.isIgnoringBatteryOptimizations) {
      // This function requires `android.permission.REQUEST_IGNORE_BATTERY_OPTIMIZATIONS` permission.
      await FlutterForegroundTask.requestIgnoreBatteryOptimization();
    }
    // Android 13 and higher, you need to allow notification permission to expose foreground service notification.
    final NotificationPermission notificationPermissionStatus =
    await FlutterForegroundTask.checkNotificationPermission();
    if (notificationPermissionStatus != NotificationPermission.granted) {
      await FlutterForegroundTask.requestNotificationPermission();
    }
  }
}

void _initForegroundTask() {
  // period for sync with sneakernets
  // const syncInterval_ms = 15 * 60 * 1000; // 15 minutes
  const syncInterval_ms = 60 * 1000; // TEST USE ONLY
  FlutterForegroundTask.init(
    androidNotificationOptions: AndroidNotificationOptions(
      foregroundServiceType: AndroidForegroundServiceType.DATA_SYNC,
      channelId: syncTaskName,
      channelName: syncTaskName,
      channelDescription: taskTitle,
      channelImportance: NotificationChannelImportance.MAX,
      priority: NotificationPriority.MAX,
      playSound: true,
      enableVibration: true,
      iconData: const NotificationIconData(
        resType: ResourceType.drawable,
        resPrefix: ResourcePrefix.ic,
        name: 'notification',
      ),
    ),
    iosNotificationOptions: const IOSNotificationOptions(
      showNotification: true,
      playSound: true,
    ),
    foregroundTaskOptions: const ForegroundTaskOptions(
      isOnceEvent: false,
      interval: syncInterval_ms,
      autoRunOnBoot: true,
      allowWakeLock: true,
      allowWifiLock: true,
    ),
  );
}

// ReceivePort? _receivePort;

Future<bool> _createForegroundTask() async {
  // You can save data using the saveData function.
  // await FlutterForegroundTask.saveData(key: 'customData', value: 'hello');

  // Register the receivePort before starting the service.
  // final ReceivePort? receivePort = FlutterForegroundTask.receivePort;
  // final bool isRegistered = _registerReceivePort(receivePort);
  // if (!isRegistered) {
  //   print('Failed to register receivePort!');
  //   return false;
  // }

  if (await FlutterForegroundTask.isRunningService) {
    return FlutterForegroundTask.restartService();
  } else {
    return FlutterForegroundTask.startService(
      notificationTitle: 'Monitoring for SneakerNet nodes',
      notificationText: '',
      callback: foregroundCallback,
    );
  }
}

@pragma('vm:entry-point')
void foregroundCallback() {
  // The setTaskHandler function must be called to handle the task in the background.
  FlutterForegroundTask.setTaskHandler(SneakernetTaskHandler());
}

class SneakernetTaskHandler extends TaskHandler {
  Settings? settings;
  Library? library;
  StreamSubscription<List<WiFiAccessPoint>>? _scanSubscription;
  List<String> _foundSneakerNets = [];

  @override
  Future<void> onStart(DateTime timestamp, SendPort? sendPort) async {
    WidgetsFlutterBinding.ensureInitialized();

    // initialize persistent settings
    // final SharedPreferences preferences = await SharedPreferences.getInstance();
    // settings = Settings(preferences: preferences);

    // use a non-backed up storage for library content
    final libraryDir = await getTemporaryDirectory();
    library = Library(libraryDir);

    _scanSubscription = WiFiScan.instance.onScannedResultsAvailable
        .listen((results) => _handleWifiScans(results));
  }

  @override
  Future<void> onRepeatEvent(DateTime timestamp, SendPort? sendPort) async {
    if (_foundSneakerNets.isNotEmpty) {
      var foundSneakerNets = _foundSneakerNets;
      // synchronize SneakerNet
      for (var ssid in foundSneakerNets!) {
        FlutterForegroundTask.updateService(
            notificationTitle: taskTitle,
            notificationText: await SneakerNet.synchronize(ssid, library)
        );
      }
    } else {
      FlutterForegroundTask.updateService(notificationTitle: taskTitle);
    }

    // keep scanning
    WiFiScan.instance.startScan();
  }

  void _handleWifiScans(List<WiFiAccessPoint> results) {
    _foundSneakerNets = SneakerNet.apsToSneakerNets(results);
    if(_foundSneakerNets.isNotEmpty)
    {
      FlutterForegroundTask.updateService(
          notificationTitle: taskTitle,
          notificationText: "Found: ${_foundSneakerNets.join(",")}");
    }
    else {
      FlutterForegroundTask.updateService(notificationTitle: taskTitle);
    }
    // keep scanning
    WiFiScan.instance.startScan();
  }

  @override
  void onDestroy(DateTime timestamp, SendPort? sendPort) {
    _scanSubscription?.cancel();
  }
}
