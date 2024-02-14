import 'dart:io';
import 'dart:typed_data';
import 'package:path/path.dart' as p;

// initialized by main
late final Library library;

class Library {
  final Directory libraryDir;
  late final Directory catalogDir;
  late final StringListFile unwantedFiles;
  late final StringListFile flaggedFiles;
  Library(this.libraryDir) {
    catalogDir = Directory(p.join(libraryDir.path, 'catalog'));
    catalogDir.create(recursive: true);
    unwantedFiles =
        StringListFile(File(p.join(libraryDir.path, 'unwanted.lst')));
    flaggedFiles = StringListFile(File(p.join(libraryDir.path, 'flagged.lst')));
  }

  List<File> files() {
    var files = catalogDir.listSync();
    // remove non-files from list
    files.removeWhere((item) => item is! File);
    return files.map((e) => e as File).toList(growable: false);
  }

  // import a file
  Future<bool> import(Future<String?> from) async {
    final String? fromPath = await from;
    if (fromPath == null) return false;

    // copy the data into our library (app storage)
    var file = await File(fromPath)
        .copy(p.join(catalogDir.path, p.basename(fromPath)));

    return file.exists();
  }

  bool add(String filename, Uint8List data, DateTime timestamp) {
    // create an inwork file
    final inWorkFile = File(p.join(catalogDir.path, filename));
    try {
      inWorkFile.writeAsBytesSync(data);
      inWorkFile.setLastModified(timestamp);
    } catch (e) {
      inWorkFile.delete();
      return false;
    }

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

  List<String> list() {
    try {
      return file.readAsLinesSync();
    }
    catch(e) {
      return [];
    }
  }

  add(String entry) {
    var items = list();
    if (items.contains(entry)) return;
    // add the item to the file
    _append(entry);
  }

  removeEach(List<String> entries) {
    var items = list();
    // filter current items
    items.removeWhere((entry) => entries.contains(entry));
    _write(items);
  }

  void _append(String entry) =>
      file.writeAsString('$entry\n', mode: FileMode.writeOnlyAppend);

  // replace entire list
  Future<void> _write(List<String> entries) async {
    var ofile = await file.open(mode: FileMode.writeOnly);
    for (String entry in entries) {
      ofile.writeString('$entry\n');
    }
    ofile.closeSync();
  }

}
