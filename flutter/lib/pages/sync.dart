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
        body: (foundSneakerNets.isEmpty)
            ? Center(
                child: Text('no SneakerNets found - try refresh)',
                    style: Theme.of(context).textTheme.headlineLarge))
            : RefreshIndicator(
                child: ListView.builder(
                    itemExtent: 120,
                    itemCount: foundSneakerNets.length,
                    itemBuilder: (BuildContext context, int index) =>
                        OutlinedButton(
                            child: Row(
                              children: [
                                ConstrainedBox(
                                  constraints: const BoxConstraints.expand(
                                      height: 75, width: 300),
                                  child: Text(foundSneakerNets[index],
                                      style: Theme.of(context)
                                          .textTheme
                                          .headlineMedium),
                                ),
                                Align(
                                  alignment: Alignment.bottomRight,
                                  child: const Icon(Icons.refresh, size: 50),
                                ),
                                // const Icon(Icons.refresh, size: 50),
                              ],
                            ),
                            onPressed: () async {
                              final dialogContextCompleter =
                                  Completer<BuildContext>();
                              showDialog(
                                  barrierDismissible: false,
                                  context: context,
                                  builder: (BuildContext dialogContext) {
                                    dialogContextCompleter
                                        .complete(dialogContext);
                                    return AlertDialog(
                                        title: Text(
                                            "Syncing ${foundSneakerNets[index]}"));
                                  });
                              await SneakerNet.sync(
                                  foundSneakerNets[index], library);
                              await dialogContextCompleter.future;
                              final dialogContext =
                                  await dialogContextCompleter.future;
                              Navigator.pop(dialogContext);
                            })),
                onRefresh: () async {
                  setState(() {});
                },
              ),
        floatingActionButton: FloatingActionButton(
          child: const Icon(Icons.refresh),
          onPressed: () {
            setState(() {});
          },
        ),
      );
}
