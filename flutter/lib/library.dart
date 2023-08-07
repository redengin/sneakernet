import 'dart:io';
import 'package:path/path.dart' as p;

// initialized by main
late final Library library;

class Library {
  late final Directory libraryDir;
  Library(Directory dir) {
    libraryDir = Directory(p.join(dir.path, 'library'));
    libraryDir.create(recursive: true);
  }
  static const inworkSuffix = ".inwork";

  // import a file
  Future<bool> import(Future<String?> from) async {
    final String? fromPath = await from;
    if (fromPath == null) return false;

    // copy the ebook into the app storage
    var file = await File(fromPath)
        .copy(p.join(libraryDir.path, p.basename(fromPath)));

    return file.exists();
  }

  List<File> catalog() {
    var files = libraryDir.listSync();
    // remove non-files
    files.removeWhere((item) => item is! File);
    // remove any in-work files
    files.removeWhere((item) => item.path.endsWith(inworkSuffix));
    return files
      .map((e) => e as File)
      .toList();
  }
}
