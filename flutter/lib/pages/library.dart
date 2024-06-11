import 'package:flutter/material.dart';
import 'package:flutter_file_dialog/flutter_file_dialog.dart';
import 'package:share_plus/share_plus.dart';
import 'package:open_filex/open_filex.dart';
import 'package:path/path.dart' as p;
import 'dart:io';

import '../main.dart';
import '../widgets/sneakernet_drawer.dart';
import '../widgets/file_tile.dart';

class LibraryPage extends StatefulWidget {
  static const String routeName = '/';

  const LibraryPage({super.key});

  @override
  createState() => _LibraryPageState();
}

class _LibraryPageState extends State<LibraryPage> {
  // FIXME rebuild upon change of library.files()
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
        body: RefreshIndicator(
            child: (files.isEmpty)
                ? ListView(children: [
                    Center(
                        child: Text('no files yet...',
                            style: Theme.of(context).textTheme.headlineLarge))
                  ])
                : ListView.builder(
                    itemExtent: 120,
                    itemCount: files.length,
                    itemBuilder: _itemBuilder,
                  ),
            onRefresh: () async {
              setState(() {});
            }),
        floatingActionButton: FloatingActionButton(
          child: const Icon(Icons.attach_file),
          onPressed: () async {
            if (await library.import(FlutterFileDialog.pickFile())) {
              setState(() {});
            }
          },
        ));
  }

  Widget? _itemBuilder(BuildContext context, int index) {
    File file = files[index];
    return GestureDetector(
        onTap: () => {
              // FIXME OpenFilex doesn't understand epub
              if (p.extension(file.path) == 'epub')
                OpenFilex.open(file.path, type: 'application/epub+zip')
              else
                OpenFilex.open(file.path)
            },
        child: Card(
            child: Stack(
          children: [
            FileTile(file),
            Align(
              alignment: Alignment.bottomRight,
              child: Row(
                mainAxisAlignment: MainAxisAlignment.end,
                children: [
                  IconButton(
                      icon: const Icon(Icons.flag),
                      tooltip: 'flag offensive content',
                      onPressed: () => setState(() {
                            library.flag(file);
                          })),
                  IconButton(
                      icon: const Icon(Icons.delete),
                      tooltip: 'unwanted content',
                      onPressed: () => setState(() {
                            library.remove(file);
                          })),
                  // IconButton(
                  //     icon: const Icon(Icons.share),
                  //     tooltip: 'share content',
                  //     onPressed: () => setState(() {
                  //           Share.shareXFiles([file.path]);
                  //         })),
                ],
              ),
            ),
          ],
        )));
  }
}
