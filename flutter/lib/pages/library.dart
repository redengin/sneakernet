import 'dart:io';
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
  final Future<List<FileSystemEntity>> files = Library.files();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          title: Text('SneakerNet Library'),
        ),
        body: FutureBuilder<List<FileSystemEntity>>(
          future: files,
          builder: _bodyBuilder,
        )
      // body: catalog.isNotEmpty
      //     ? ListView.builder(
      //         itemCount: catalog.length,
      //         itemBuilder: (context, index) {
      //           return BookCard(catalog.elementAt(index));
      //         })
      //     : Center(child: Text('SneakerNet library is empty')),
    );
  }

  Widget _bodyBuilder(BuildContext context,
      AsyncSnapshot<List<FileSystemEntity>> snapshot) {
    if (snapshot.hasData && snapshot.data!.isNotEmpty) {
      final files = snapshot.data!;
      return ListView.builder(
          itemCount: files.length,
          itemBuilder: (context, index) {
            return BookCard(files.elementAt(index));
          });
    }

    return const Center(child: Text('SneakerNet library is empty'));
  }
}
