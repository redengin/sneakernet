import 'package:epubx/src/ref_entities/epub_book_ref.dart';
import 'package:flutter/material.dart';

import '../library.dart';
import '../widgets/book_card.dart';

class LibraryPage extends StatefulWidget {
  static const String routeName = '/library';

  const LibraryPage({super.key});

  @override
  createState() => _LibraryPageState();
}

class _LibraryPageState extends State<LibraryPage> {
  final Future<List<Future<EpubBookRef>>> catalog = Library.catalog();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          title: Text('SneakerNet Library'),
        ),
        body: FutureBuilder<List<Future<EpubBookRef>>>(
          future: catalog,
          builder: _bodyBuilder,
        )
    );
  }

  Widget _bodyBuilder(BuildContext context,
      AsyncSnapshot<List<Future<EpubBookRef>>> snapshot) {
    if (snapshot.hasData && snapshot.data!.isNotEmpty) {
      final ebookRefs = snapshot.data!;
      return ListView.builder(
          itemExtent: 120,
          itemCount: ebookRefs.length,
          itemBuilder: (context, index) {
            return BookCard(ebookRefs.elementAt(index));
          });
    }

    return const Center(child: Text('SneakerNet library is empty'));
  }
}
