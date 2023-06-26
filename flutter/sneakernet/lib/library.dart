import 'dart:io';
import 'package:path_provider/path_provider.dart';
import 'package:path/path.dart' as Path;

class Library {
  // import an epub book from downloads
  static import(downloadsPath) async {
    File from = File(downloadsPath);

    // TODO validate ebook (need a better ebook package)

    // copy the ebook into the app storage
    await File(downloadsPath)
        .copy(Path.join(_getStoragePath(), Path.basename(downloadsPath)));
  }

  // turn each stored ebook file into an EbookRef
  static List catalog() {
    return [];
    // return _getStoragePath()
    //     .list()
    //     .map((_) => EpubReader.readBook(File(_).readAsBytes()));
  }

  static _getStoragePath() {
    return Directory(
            Path.join(getApplicationSupportDirectory().toString(), 'library'))
        .create();
  }
}
