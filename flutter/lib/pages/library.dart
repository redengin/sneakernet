import 'package:flutter/material.dart';
import 'package:sneakernet/library.dart';
import 'package:sneakernet/widgets/book_card.dart';

class LibraryPage extends StatefulWidget {
  static const String routeName = '/library';

  const LibraryPage({super.key});

  @override
  createState() => _LibraryPageState();
}

class _LibraryPageState extends State<LibraryPage> {
  // List<EpubBook> catalog = Library.catalog();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('SneakerNet Library'),
      ),
      // body: catalog.isNotEmpty
      //     ? ListView.builder(
      //         itemCount: catalog.length,
      //         itemBuilder: (context, index) {
      //           return BookCard(catalog.elementAt(index));
      //         })
      //     : Center(child: Text('SneakerNet library is empty')),
    );
  }
}
