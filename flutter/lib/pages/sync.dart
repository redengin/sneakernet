import 'dart:async';

import 'package:flutter/material.dart';
import 'package:sneakernet/main.dart';
import 'package:sneakernet/sneakernet.dart';

import '../library.dart';

class SyncPage extends StatefulWidget {
  static const String routeName = '/sync';

  const SyncPage({super.key});

  @override
  createState() => _SyncPageState();
}

class _SyncPageState extends State<SyncPage> {
  @override
  Widget build(BuildContext context) => Scaffold(
        appBar: AppBar(
          title: const Text('Found SneakerNets'),
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
                              style: Theme.of(context).textTheme.headlineLarge,
                            ),
                            trailing: Icon(Icons.sync_alt),
                            onTap: () {
                              sync(foundSneakerNets[index]);
                            }),
                      )),
          onRefresh: () async {
            setState(() {});
          },
        ),
      );

  Future<void> sync(String ssid) async {
    final dialogContextCompleter = Completer<BuildContext>();
    showDialog(
        barrierDismissible: false,
        context: context,
        builder: (BuildContext dialogContext) {
          dialogContextCompleter.complete(dialogContext);
          return AlertDialog(title: Text("Syncing ${ssid}"));
        });
    await SneakerNet.sync(ssid, library);
    await dialogContextCompleter.future;
    final dialogContext = await dialogContextCompleter.future;
    Navigator.pop(dialogContext);
  }
}
