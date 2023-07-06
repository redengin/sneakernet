import 'dart:typed_data';

import 'package:image/image.dart' as DartImage;
import 'package:flutter/material.dart';
import 'package:ebook/ebook.dart';

class BookCard extends StatefulWidget {
  final EBook ebook;

  const BookCard(this.ebook, {super.key});

  @override
  createState() => _BookCardState();
}

class _BookCardState extends State<BookCard> {
  String _title = "";
  Uint8List? _coverImage;

  @override
  void initState() {
    var info = widget.ebook.getInfo();
    if (info != null) {
      if (info!.titles.isNotEmpty) _title = info.titles.first;
      if (info!.coverImageRefs.isNotEmpty) {
        _coverImage = widget.ebook.readContainerFile(info.coverImageRefs.first.href);
      }
    }

    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Column(
        children: [
          ListTile(
            leading: (_coverImage != null)
                ? Image.memory(_coverImage!)
                : const Icon(Icons.note),
            title: Text(_title),
            // subtitle: _getSubtitle(),
          ),
          // actions
          const Row(
            mainAxisAlignment: MainAxisAlignment.end,
            children: [],
          )
        ],
      ),
    );
  }
}

// class _Cover extends StatefulWidget {
//   final Future<Epub.Image?> cover;
//
//   _Cover(this.cover);
//
//   @override
//   createState() => _CoverState();
// }
//
// class _CoverState extends State<_Cover> {
//   @override
//   Widget build(BuildContext context) {
//     return FutureBuilder(
//       future: widget.cover,
//       builder: (context, snapshot) =>
//       snapshot.hasData
//           ? Image.memory(snapshot.data!.getBytes())
//           : const Icon(Icons.downloading),
//     );
//   }
// }
