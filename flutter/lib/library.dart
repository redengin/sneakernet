import 'dart:io';
import 'dart:typed_data';
import 'package:path/path.dart' as p;

// initialized by main
late final Library library;

class Library {
  final Directory libraryDir;
  late final StringListFile unwantedFiles;
  late final StringListFile flaggedFiles;
  late final Directory catalogDir;
  Library(this.libraryDir) {
    catalogDir = Directory(p.join(libraryDir.path, 'catalog'));
    catalogDir.create(recursive: true);
    unwantedFiles =
        StringListFile(File(p.join(libraryDir.path, 'unwanted.lst')));
    flaggedFiles = StringListFile(File(p.join(libraryDir.path, 'flagged.lst')));
  }

  List<File> files() {
    var files = catalogDir.listSync();
    // remove non-files
    files.removeWhere((item) => item is! File);
    // remove any in-work files
    files.removeWhere((item) => item.path.endsWith(inworkSuffix));
    return files.map((e) => e as File).toList(growable: false);
  }

  // import a file
  Future<bool> import(Future<String?> from) async {
    final String? fromPath = await from;
    if (fromPath == null) return false;

    // copy the ebook into the app storage
    var file = await File(fromPath)
        .copy(p.join(catalogDir.path, p.basename(fromPath)));

    return file.exists();
  }

  static const inworkSuffix = ".inwork";
  bool add(String filename, Uint8List bytes) {
    // create an inwork file
    final inWorkFile = File(p.join(catalogDir.path, filename + inworkSuffix));
    try {
      inWorkFile.writeAsBytesSync(bytes);
    } catch (e) {
      inWorkFile.delete();
      return false;
    }
    // rename inwork file to actual name
    inWorkFile.rename(p.join(catalogDir.path, filename));
    return true;
  }

  void remove(File file) {
    unwantedFiles.add(p.basename(file.path));
    file.deleteSync();
  }

  void flag(File file) {
    flaggedFiles.add(p.basename(file.path));
    file.deleteSync();
  }
}

class StringListFile {
  File file;

  StringListFile(this.file);

  List<String> load() {
    try {
      return file.readAsLinesSync();
    }
    catch(e) {
      return [];
    }
  }

  void _append(String filename) =>
      file.writeAsString('$filename\n', mode: FileMode.writeOnlyAppend);

  Future<void> _write(List<String> filenames) async {
    var ofile = await file.open(mode: FileMode.writeOnly);
    for (String filename in filenames) {
      ofile.writeString('$filename\n');
    }
    ofile.closeSync();
  }

  add(String filename) {
    var items = load();
    if (items.contains(filename)) return;
    // add the item to the file
    _append(filename);
  }

  removeAll(List<String> removals) {
    var items = load();
    // filter current items
    items.removeWhere((filename) => removals.contains(filename));
    _write(items);
  }
}
