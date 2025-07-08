import 'package:flutter/material.dart';
import 'package:flutter_nearby_connections/flutter_nearby_connections.dart';
import 'dart:async';
import 'dart:convert';
import 'dart:io';
import 'package:device_info_plus/device_info_plus.dart';
import 'package:flutter_styled_toast/flutter_styled_toast.dart';

void main() {
  runApp(SneakerNetApp());
}

class SneakerNetApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(onGenerateRoute: generateRoute, initialRoute: '/');
  }
}

Route<dynamic> generateRoute(RouteSettings settings) {
  switch (settings.name) {
    case '/':
      return MaterialPageRoute(builder: (_) => Home());
    case 'browser':
      return MaterialPageRoute(
        builder: (_) => DevicesListScreen(deviceType: DeviceType.browser),
      );
    case 'advertiser':
      return MaterialPageRoute(
        builder: (_) => DevicesListScreen(deviceType: DeviceType.advertiser),
      );
    default:
      return MaterialPageRoute(
        builder: (_) => Scaffold(
          body: Center(child: Text('No route defined for ${settings.name}')),
        ),
      );
  }
}

class Home extends StatelessWidget {
  const Home({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Column(
        children: [
          Expanded(
            child: InkWell(
              onTap: () {
                Navigator.pushNamed(context, 'browser');
              },
              child: Container(
                color: Colors.red,
                child: Center(
                  child: Text(
                    'BROWSER',
                    style: TextStyle(color: Colors.white, fontSize: 40),
                  ),
                ),
              ),
            ),
          ),
          Expanded(
            child: InkWell(
              onTap: () {
                Navigator.pushNamed(context, 'advertiser');
              },
              child: Container(
                color: Colors.green,
                child: Center(
                  child: Text(
                    'ADVERTISER',
                    style: TextStyle(color: Colors.white, fontSize: 40),
                  ),
                ),
              ),
            ),
          ),
        ],
      ),
    );
  }
}

enum DeviceType { advertiser, browser }

class DevicesListScreen extends StatefulWidget {
  const DevicesListScreen({required this.deviceType});

  final DeviceType deviceType;

  @override
  _DevicesListScreenState createState() => _DevicesListScreenState();
}

class _DevicesListScreenState extends State<DevicesListScreen> {
  List<Device> devices = [];
  List<Device> connectedDevices = [];
  late NearbyService nearbyService;
  late StreamSubscription subscription;
  late StreamSubscription receivedDataSubscription;

  bool isInit = false;

  @override
  void initState() {
    super.initState();
    init();
  }

  @override
  void dispose() {
    subscription.cancel();
    receivedDataSubscription.cancel();
    nearbyService.stopBrowsingForPeers();
    nearbyService.stopAdvertisingPeer();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.deviceType.toString().substring(11).toUpperCase()),
      ),
      backgroundColor: Colors.white,
      body: ListView.builder(
        itemCount: getItemCount(),
        itemBuilder: (context, index) {
          final device = widget.deviceType == DeviceType.advertiser
              ? connectedDevices[index]
              : devices[index];
          return Container(
            margin: EdgeInsets.all(8.0),
            child: Column(
              children: [
                Row(
                  children: [
                    Expanded(
                      child: GestureDetector(
                        onTap: () => _onTabItemListener(device),
                        child: Column(
                          children: [
                            Text(device.deviceName),
                            Text(
                              getStateName(device.state),
                              style: TextStyle(
                                color: getStateColor(device.state),
                              ),
                            ),
                          ],
                          crossAxisAlignment: CrossAxisAlignment.start,
                        ),
                      ),
                    ),
                    // Request connect
                    GestureDetector(
                      onTap: () => _onButtonClicked(device),
                      child: Container(
                        margin: EdgeInsets.symmetric(horizontal: 8.0),
                        padding: EdgeInsets.all(8.0),
                        height: 35,
                        width: 100,
                        color: getButtonColor(device.state),
                        child: Center(
                          child: Text(
                            getButtonStateName(device.state),
                            style: TextStyle(
                              color: Colors.white,
                              fontWeight: FontWeight.bold,
                            ),
                          ),
                        ),
                      ),
                    ),
                  ],
                ),
                SizedBox(height: 8.0),
                Divider(height: 1, color: Colors.grey),
              ],
            ),
          );
        },
      ),
    );
  }

  String getStateName(SessionState state) {
    switch (state) {
      case SessionState.notConnected:
        return "disconnected";
      case SessionState.connecting:
        return "waiting";
      default:
        return "connected";
    }
  }

  String getButtonStateName(SessionState state) {
    switch (state) {
      case SessionState.notConnected:
      case SessionState.connecting:
        return "Connect";
      default:
        return "Disconnect";
    }
  }

  Color getStateColor(SessionState state) {
    switch (state) {
      case SessionState.notConnected:
        return Colors.black;
      case SessionState.connecting:
        return Colors.grey;
      default:
        return Colors.green;
    }
  }

  Color getButtonColor(SessionState state) {
    switch (state) {
      case SessionState.notConnected:
      case SessionState.connecting:
        return Colors.green;
      default:
        return Colors.red;
    }
  }

  _onTabItemListener(Device device) {
    if (device.state == SessionState.connected) {
      showDialog(
        context: context,
        builder: (BuildContext context) {
          final myController = TextEditingController();
          return AlertDialog(
            title: Text("Send message"),
            content: TextField(controller: myController),
            actions: [
              TextButton(
                child: Text("Cancel"),
                onPressed: () {
                  Navigator.of(context).pop();
                },
              ),
              TextButton(
                child: Text("Send"),
                onPressed: () {
                  nearbyService.sendMessage(device.deviceId, myController.text);
                  myController.text = '';
                },
              ),
            ],
          );
        },
      );
    }
  }

  int getItemCount() {
    if (widget.deviceType == DeviceType.advertiser) {
      return connectedDevices.length;
    } else {
      return devices.length;
    }
  }

  _onButtonClicked(Device device) {
    switch (device.state) {
      case SessionState.notConnected:
        nearbyService.invitePeer(
          deviceID: device.deviceId,
          deviceName: device.deviceName,
        );
        break;
      case SessionState.connected:
        nearbyService.disconnectPeer(deviceID: device.deviceId);
        break;
      case SessionState.connecting:
        break;
    }
  }

  void init() async {
    nearbyService = NearbyService();
    DeviceInfoPlugin deviceInfo = DeviceInfoPlugin();
    String devInfo = '';
    if (Platform.isAndroid) {
      AndroidDeviceInfo androidInfo = await deviceInfo.androidInfo;
      devInfo = androidInfo.model;
    }
    if (Platform.isIOS) {
      IosDeviceInfo iosInfo = await deviceInfo.iosInfo;
      devInfo = iosInfo.localizedModel;
    }
    await nearbyService.init(
      serviceType: 'SneakerNet',
      deviceName: devInfo,
      strategy: Strategy.P2P_POINT_TO_POINT,
      callback: (isRunning) async {
        if (isRunning) {
          if (widget.deviceType == DeviceType.browser) {
            await nearbyService.stopBrowsingForPeers();
            await Future.delayed(Duration(microseconds: 200));
            await nearbyService.startBrowsingForPeers();
          } else {
            await nearbyService.stopAdvertisingPeer();
            await nearbyService.stopBrowsingForPeers();
            await Future.delayed(Duration(microseconds: 200));
            await nearbyService.startAdvertisingPeer();
            await nearbyService.startBrowsingForPeers();
          }
        }
      },
    );
    subscription = nearbyService.stateChangedSubscription(
      callback: (devicesList) {
        devicesList.forEach((element) {
          print(
            " deviceId: ${element.deviceId} | deviceName: ${element.deviceName} | state: ${element.state}",
          );

          if (Platform.isAndroid) {
            if (element.state == SessionState.connected) {
              nearbyService.stopBrowsingForPeers();
            } else {
              nearbyService.startBrowsingForPeers();
            }
          }
        });

        setState(() {
          devices.clear();
          devices.addAll(devicesList);
          connectedDevices.clear();
          connectedDevices.addAll(
            devicesList
                .where((d) => d.state == SessionState.connected)
                .toList(),
          );
        });
      },
    );

    receivedDataSubscription = nearbyService.dataReceivedSubscription(
      callback: (data) {
        print("dataReceivedSubscription: ${jsonEncode(data)}");
        showToast(
          jsonEncode(data),
          context: context,
          axis: Axis.horizontal,
          alignment: Alignment.center,
          position: StyledToastPosition.bottom,
        );
      },
    );
  }
}

// class MyApp extends StatelessWidget {
//   const MyApp({super.key});
//
//   // This widget is the root of your application.
//   @override
//   Widget build(BuildContext context) {
//     return MaterialApp(
//       title: 'Flutter Demo',
//       theme: ThemeData(
//         // This is the theme of your application.
//         //
//         // TRY THIS: Try running your application with "flutter run". You'll see
//         // the application has a purple toolbar. Then, without quitting the app,
//         // try changing the seedColor in the colorScheme below to Colors.green
//         // and then invoke "hot reload" (save your changes or press the "hot
//         // reload" button in a Flutter-supported IDE, or press "r" if you used
//         // the command line to start the app).
//         //
//         // Notice that the counter didn't reset back to zero; the application
//         // state is not lost during the reload. To reset the state, use hot
//         // restart instead.
//         //
//         // This works for code too, not just values: Most code changes can be
//         // tested with just a hot reload.
//         colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
//       ),
//       home: const MyHomePage(title: 'Flutter Demo Home Page'),
//     );
//   }
// }
//
// class MyHomePage extends StatefulWidget {
//   const MyHomePage({super.key, required this.title});
//
//   // This widget is the home page of your application. It is stateful, meaning
//   // that it has a State object (defined below) that contains fields that affect
//   // how it looks.
//
//   // This class is the configuration for the state. It holds the values (in this
//   // case the title) provided by the parent (in this case the App widget) and
//   // used by the build method of the State. Fields in a Widget subclass are
//   // always marked "final".
//
//   final String title;
//
//   @override
//   State<MyHomePage> createState() => _MyHomePageState();
// }
//
// class _MyHomePageState extends State<MyHomePage> {
//   int _counter = 0;
//
//   void _incrementCounter() {
//     setState(() {
//       // This call to setState tells the Flutter framework that something has
//       // changed in this State, which causes it to rerun the build method below
//       // so that the display can reflect the updated values. If we changed
//       // _counter without calling setState(), then the build method would not be
//       // called again, and so nothing would appear to happen.
//       _counter++;
//     });
//   }
//
//   @override
//   Widget build(BuildContext context) {
//     // This method is rerun every time setState is called, for instance as done
//     // by the _incrementCounter method above.
//     //
//     // The Flutter framework has been optimized to make rerunning build methods
//     // fast, so that you can just rebuild anything that needs updating rather
//     // than having to individually change instances of widgets.
//     return Scaffold(
//       appBar: AppBar(
//         // TRY THIS: Try changing the color here to a specific color (to
//         // Colors.amber, perhaps?) and trigger a hot reload to see the AppBar
//         // change color while the other colors stay the same.
//         backgroundColor: Theme.of(context).colorScheme.inversePrimary,
//         // Here we take the value from the MyHomePage object that was created by
//         // the App.build method, and use it to set our appbar title.
//         title: Text(widget.title),
//       ),
//       body: Center(
//         // Center is a layout widget. It takes a single child and positions it
//         // in the middle of the parent.
//         child: Column(
//           // Column is also a layout widget. It takes a list of children and
//           // arranges them vertically. By default, it sizes itself to fit its
//           // children horizontally, and tries to be as tall as its parent.
//           //
//           // Column has various properties to control how it sizes itself and
//           // how it positions its children. Here we use mainAxisAlignment to
//           // center the children vertically; the main axis here is the vertical
//           // axis because Columns are vertical (the cross axis would be
//           // horizontal).
//           //
//           // TRY THIS: Invoke "debug painting" (choose the "Toggle Debug Paint"
//           // action in the IDE, or press "p" in the console), to see the
//           // wireframe for each widget.
//           mainAxisAlignment: MainAxisAlignment.center,
//           children: <Widget>[
//             const Text('You have pushed the button this many times:'),
//             Text(
//               '$_counter',
//               style: Theme.of(context).textTheme.headlineMedium,
//             ),
//           ],
//         ),
//       ),
//       floatingActionButton: FloatingActionButton(
//         onPressed: _incrementCounter,
//         tooltip: 'Increment',
//         child: const Icon(Icons.add),
//       ), // This trailing comma makes auto-formatting nicer for build methods.
//     );
//   }
// }
