import 'dart:io';
import 'package:epubx/epubx.dart';
import 'package:path_provider/path_provider.dart';
import 'package:path/path.dart' as Path;

class Library {
  // import an epub book from downloads
  static import(Future<String?> from) async {
    final String? fromPath = await from;
    if (fromPath == null) return;

    // TODO validate ebook (need a better ebook package)

    // copy the ebook into the app storage
    final storageDir = await _getStorageDirectory();
    await File(fromPath)
        .copy(Path.join(storageDir.path, Path.basename(fromPath)));
  }

  static Future<List<FileSystemEntity>> files() async {
    final storageDir = await _getStorageDirectory();
    return storageDir.listSync(followLinks: false);
  }

  // turn each stored file into an EbookRef
  static Future<List<Future<EpubBookRef>>> catalog() async {
    final libraryFiles = await files();
    return libraryFiles
        .map((file) => EpubReader.openBook(File(file.path).readAsBytesSync()))
        .toList();
  }

  static Future<Directory> _getStorageDirectory() async {
    // temporary directory : not backed up
    Directory dir = await getTemporaryDirectory();
    final path = Path.join(dir.path, 'library');
    return Directory(path).create(recursive: true);
  }
}
