import 'dart:io';
import 'dart:typed_data';
import 'package:path/path.dart' as p;

// initialized by main
late final Library library;

class Library {
  late final Directory libraryDir;
  Library(this.libraryDir) {
    libraryDir.create(recursive: true);
  }

  List<File> files() {
    var files = libraryDir.listSync();
    // remove non-files
    files.removeWhere((item) => item is! File);
    // remove any in-work files
    files.removeWhere((item) => item.path.endsWith(inworkSuffix));
    return files
        .map((e) => e as File)
        .toList(growable: false);
  }

  // import a file
  Future<bool> import(Future<String?> from) async {
    final String? fromPath = await from;
    if (fromPath == null) return false;

    // copy the ebook into the app storage
    var file = await File(fromPath)
        .copy(p.join(libraryDir.path, p.basename(fromPath)));

    return file.exists();
  }

  static const inworkSuffix = ".inwork";
  bool add(String filename, Uint8List bytes) {
    // create an inwork file
    final inWorkFile = File(p.join(libraryDir.path, filename+inworkSuffix));
    try {
      inWorkFile.writeAsBytesSync(bytes, flush: true);
    } catch(e) {
      inWorkFile.delete();
      return false;
    }
    // rename inwork file to actual name
    inWorkFile.rename(p.join(libraryDir.path, filename));
    return true;
  }

  void remove(File file) {
      file.deleteSync();
  }
}
