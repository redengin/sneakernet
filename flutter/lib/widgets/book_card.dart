import 'package:flutter/material.dart';
import 'package:epubx/epubx.dart' as Epub;

class BookCard extends StatefulWidget {
  final Future<Epub.EpubBookRef> book;

  const BookCard(this.book, {super.key});

  @override
  createState() => _BookCardState();
}

class _BookCardState extends State<BookCard> {
  @override
  Widget build(BuildContext context) {
    return Card(
      child: Column(
        children: [
          ListTile(
            leading: FutureBuilder<Epub.EpubBookRef>(
              future: widget.book,
              builder: (context, snapshot) =>
              snapshot.hasData
                  ? _Cover(snapshot.data!.readCover())
                  : const Icon(Icons.note),
            ),
            title: FutureBuilder<Epub.EpubBookRef>(
                future: widget.book,
                builder: (context, snapshot) =>
                (snapshot.hasData && (snapshot.data!.Title != null))
                    ? Text(snapshot.data!.Title!)
                    : const Text("")),
            subtitle: FutureBuilder<Epub.EpubBookRef>(
                future: widget.book,
                builder: (context, snapshot) =>
                (snapshot.hasData && (snapshot.data!.Author != null))
                    ? Text(snapshot.data!.Author!)
                    : const Text("")),
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

class _Cover extends StatefulWidget {
  final Future<Epub.Image?> cover;

  _Cover(this.cover);

  @override
  createState() => _CoverState();
}

class _CoverState extends State<_Cover> {
  @override
  Widget build(BuildContext context) {
    return FutureBuilder(
      future: widget.cover,
      builder: (context, snapshot) =>
      snapshot.hasData
          ? Image.memory(snapshot.data!.getBytes())
          : const Icon(Icons.downloading),
    );
  }
}
