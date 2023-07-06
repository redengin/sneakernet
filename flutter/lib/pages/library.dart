import 'package:flutter/material.dart';
import 'package:ebook/ebook.dart';

import '../library.dart';
import '../widgets/book_card.dart';

class LibraryPage extends StatefulWidget {
  static const String routeName = '/library';

  const LibraryPage({super.key});

  @override
  createState() => _LibraryPageState();
}

class _LibraryPageState extends State<LibraryPage> {
  final Future<List<EBook>> catalog = Library.catalog();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          title: Text('SneakerNet Library'),
        ),
        body: FutureBuilder<List<EBook>>(
          future: catalog,
          builder: _bodyBuilder,
        )
    );
  }

  Widget _bodyBuilder(BuildContext context,
      AsyncSnapshot<List<EBook>> snapshot) {
    if (snapshot.hasData && snapshot.data!.isNotEmpty) {
      final ebookRefs = snapshot.data!;
      return ListView.builder(
          itemCount: ebookRefs.length,
          itemBuilder: (context, index) {
            return BookCard(ebookRefs.elementAt(index));
          });
    }
    return const Center(child: Text('SneakerNet library is empty'));
  }
}
