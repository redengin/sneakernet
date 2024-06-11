import 'dart:async';

import 'package:flutter/material.dart';
import 'package:sneakernet/main.dart';
import 'package:sneakernet/sneakernet.dart';
import 'package:wifi_scan/wifi_scan.dart';

class SyncPage extends StatefulWidget {
  static const String routeName = '/sync';

  const SyncPage({super.key});

  @override
  createState() => _SyncPageState();
}

class _SyncPageState extends State<SyncPage> {
  List<String> foundSneakerNets = [];

  _SyncPageState() {
    WiFiScan.instance.onScannedResultsAvailable.listen((aps) {
      setState(() => foundSneakerNets = SneakerNet.apsToSneakerNets(aps));
    });

    WiFiScan.instance.startScan();
  }

  @override
  Widget build(BuildContext context) => Scaffold(
        appBar: AppBar(
          title: const Text('SneakerNet Nodes'),
        ),
        body: RefreshIndicator(
          child: (foundSneakerNets.isEmpty)
              ? ListView(children: [
                  Center(
                      child: Text('No SneakerNets Found',
                          style: Theme.of(context).textTheme.headlineLarge))
                ])
              : ListView.builder(
                  itemExtent: 100,
                  itemCount: foundSneakerNets.length,
                  itemBuilder: (BuildContext context, int index) => Card(
                        child: ListTile(
                            title: Text(
                              foundSneakerNets[index],
                              style: Theme.of(context).textTheme.headlineSmall,
                            ),
                            trailing: const Icon(Icons.sync_alt),
                            onTap: () {
                              sync(foundSneakerNets[index]);
                            }),
                      )),
          onRefresh: () async {
            setState(() { WiFiScan.instance.startScan(); });
          },
        ),
      );

  Future<void> sync(String ssid) async {
    var message = await SneakerNet.synchronize(ssid, library);
    showDialog(
        context: context,
        builder: (BuildContext context) {
          return AlertDialog(
              title: Text("Syncing $ssid"),
              content: Text(message),
          );
        });
  }
}
