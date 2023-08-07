import 'package:flutter/material.dart';
import 'dart:io';
import 'package:path/path.dart' as p;
import 'package:ebook/ebook.dart';

class FileTile extends StatelessWidget {
  final File file;

  late final Widget _preview;
  late final String _title;

  FileTile(this.file, {super.key}) {
    Widget preview = const Icon(Icons.note);
    var title = p.basename(file.path);

    switch (p.extension(file.path)) {
      case '.epub':
        final ebook = EBook(file);
        final info = ebook.getInfo();
        if (info != null) {
          if (info.coverImageRefs.isNotEmpty) {
            final data =
                ebook.readContainerFile(info.coverImageRefs.first.href);
            if (data != null) {
              preview = Image.memory(data);
            }
            if (info.titles.isNotEmpty) {
              title = info.titles[0];
            }
          }
        }
    }
    _preview = preview;
    _title = title;
  }

  @override
  Widget build(BuildContext context) => ListTile(
        leading: _preview,
        title: Text(_title),
        // subtitle: Text(_subtitle),
      );
}

// ListTile FileTile(File file) {
//   final extension = p.extension(file.path);
//
//   var _preview = Icon(Icons.note);
//   var _title = p.basename(file.path);
//
// }
