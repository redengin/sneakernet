import 'package:flutter/material.dart';
import 'package:flutter_file_dialog/flutter_file_dialog.dart';
import 'package:open_filex/open_filex.dart';
import 'dart:io';

import '../library.dart';
import '../widgets/sneakernet_drawer.dart';
import '../widgets/file_tile.dart';

class LibraryPage extends StatefulWidget {
  static const String routeName = '/library';

  const LibraryPage({super.key});

  @override
  createState() => _LibraryPageState();
}

class _LibraryPageState extends State<LibraryPage> {
  var files;

  @override
  Widget build(BuildContext context) {
    // get the latest set of files
    files = library.files();
    // most recent first
    files.sort(
        (a, b) => a.lastAccessedSync().isAfter(b.lastAccessedSync()) ? 1 : -1);

    return Scaffold(
      appBar: AppBar(
        title: const Text('SneakerNet Library'),
      ),
      drawer: SneakerNetDrawer(),
      body: (files.isEmpty)
          ? Center(
              child: Text('no files',
                  style: Theme.of(context).textTheme.headlineLarge))
          : ListView.builder(
              itemExtent: 120,
              itemCount: files.length,
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
    File file = files[index];
    return GestureDetector(
        onTap: () => {OpenFilex.open(file.path)},
        child: Card(
            child: Stack(
          children: [
            FileTile(file),
            Align(
              alignment: Alignment.bottomRight,
              child: IconButton(
                  icon: const Icon(Icons.delete),
                  onPressed: () => setState(() {
                        library.remove(file);
                      })),
            )
          ],
        )));
  }
}
