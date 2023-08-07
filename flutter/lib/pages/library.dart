import 'package:flutter/material.dart';
import 'package:flutter_file_dialog/flutter_file_dialog.dart';
import 'package:path/path.dart' as path;
import '../library.dart';
import 'dart:io';

import '../widgets/file_tile.dart';
import '../widgets/sneakernet_drawer.dart';

class LibraryPage extends StatefulWidget {
  static const String routeName = '/library';

  const LibraryPage({super.key});

  @override
  createState() => _LibraryPageState();
}

class _LibraryPageState extends State<LibraryPage> {
  var catalog;

  @override
  Widget build(BuildContext context) {
    // get the latest set of files
    catalog = library.catalog();
    // most recent first
    catalog.sort(
        (a, b) => a.lastAccessedSync().isAfter(b.lastAccessedSync()) ? 1 : -1);

    return Scaffold(
      appBar: AppBar(
        title: const Text('SneakerNet Library'),
      ),
      drawer: SneakerNetDrawer(),
      body: (catalog.isEmpty)
          ? Center(
              child: Text('no files',
                  style: Theme.of(context).textTheme.headlineLarge))
          : ListView.builder(
              itemExtent: 120,
              itemCount: catalog.length,
              itemBuilder: _itemBuilder,
            ),
      floatingActionButton: FloatingActionButton(
        child: const Icon(Icons.add),
        onPressed: () async {
          if (await library.import(FlutterFileDialog.pickFile())) {
            setState(() {});
          }
        },
      ),
    );
  }

  Widget? _itemBuilder(BuildContext context, int index) {
    File file = catalog[index];
    return Card(
        child: Column(children: [
      FileTile(file),
      Row(mainAxisAlignment: MainAxisAlignment.end, children: [
        IconButton(
          icon: const Icon(Icons.delete),
          onPressed: () => setState(() {
            file.delete();
            catalog.removeAt(index);
          }),
        )
      ])
    ]));
  }
}
