import 'package:flutter/material.dart';
import '../library.dart';

class BookCard extends StatelessWidget {
  // final EpubBook ebook;

  // const BookCard(this.ebook, {super.key});

  @override
  Widget build(BuildContext context) {
    return Card(
        child: Column(
      children: [
        ListTile(
          leading:
              SizedBox(height: 100.0, width: 100.0, child: Icon(Icons.note)),
          // ebook.CoverImage.isDefinedAndNotNull ?
          // Image.memory(ebook.CoverImage.getBytes())
          //     : Icon(Icons.note)),
          // title: Text(ebook.Title ?? ""),
        ),
        Row(
          mainAxisAlignment: MainAxisAlignment.end,
          children: [],
        )
      ],
    ));
  }
}
