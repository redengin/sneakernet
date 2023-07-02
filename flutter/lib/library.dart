import 'dart:io';
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

  // turn each stored ebook file into an EbookRef
  static Future<List<FileSystemEntity>> files() async {
    final storageDir = await _getStorageDirectory();
    return storageDir.listSync(followLinks: false);
  }

  static Future<Directory> _getStorageDirectory() async {
    // temporary directory : not backed up
    Directory dir = await getTemporaryDirectory();
    final path = Path.join(dir.path, 'library');
    return Directory(path).create(recursive: true);
  }
}
