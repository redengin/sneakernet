import 'package:flutter/material.dart';
import 'package:epubx/epubx.dart';

class BookCard extends StatelessWidget {
  late final EpubBookRef? ebookRef;
  BookCard(file, {super.key}) {
    ebookRef = null;
  }

  @override
  Widget build(BuildContext context) {

    return Card(
        child: Column(
      children: [
        ListTile(
          leading:
              const SizedBox(height: 100.0, width: 100.0,
              child: Icon(Icons.note)),
          // ebook.CoverImage.isDefinedAndNotNull ?
          // Image.memory(ebook.CoverImage.getBytes())
          //     : Icon(Icons.note)),
          // title: Text(ebookRef.Title ?? ""),
        ),
        // actions
        const Row(
          mainAxisAlignment: MainAxisAlignment.end,
          children: [],
        )
      ],
    ));
  }
}
