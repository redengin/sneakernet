import 'dart:io';
import 'dart:typed_data';
import 'package:flutter/cupertino.dart';
import 'package:path/path.dart' as p;

class Library with ChangeNotifier {
  final Directory libraryDir;
  late final Directory catalogDir;
  late final StringListFile unwantedFiles;
  late final StringListFile flaggedFiles;

  Library(this.libraryDir) {
    catalogDir = Directory(p.join(libraryDir.path, 'catalog'));
    catalogDir.createSync(recursive: true);
    unwantedFiles =
        StringListFile(File(p.join(libraryDir.path, 'unwanted.lst')));
    flaggedFiles = StringListFile(File(p.join(libraryDir.path, 'flagged.lst')));
  }

  final inworkSuffix = ".INWORK";

  List<File> files() {
    var files = catalogDir.listSync();
    // remove non-files from list
    files.removeWhere((item) => item is! File);
    files.removeWhere((item) => item.path.endsWith(inworkSuffix));
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

  Future<bool> add(String filename, Uint8List data, DateTime timestamp) async {
    // create an inwork file
    final inWorkFile = File(p.join(catalogDir.path, filename + inworkSuffix));
    try {
      inWorkFile.writeAsBytesSync(data);
    } catch (e) {
      inWorkFile.delete();
      return false;
    }
    // rename it to the actual file name
    final file = await inWorkFile.rename(p.join(catalogDir.path, filename));
    await file.setLastModified(timestamp);
    return true;
  }

  void remove(File file) {
    unwantedFiles.add(p.basename(file.path));
    file.deleteSync();
  }

  bool isWanted(String filename, DateTime timestamp) {
    final flaggedFilenames = flaggedFiles.list();
    if (flaggedFilenames.contains(filename)) {
      return false;
    }
    final unwantedFilenames = unwantedFiles.list();
    if (unwantedFilenames.contains(filename)) {
      return false;
    }
    final localFiles = files();
    try {
      final localFile =
          localFiles.firstWhere((e) => p.basename(e.path) == filename);
      return (localFile.lastModifiedSync().isBefore(timestamp)) ? true : false;
    } on StateError catch (_) {
      // no local file match
      return true;
    }
  }

  void flag(File file) {
    flaggedFiles.add(p.basename(file.path));
    file.deleteSync();
  }

  bool isFlagged(String filename) {
    final flaggedFilenames = flaggedFiles.list();
    return flaggedFilenames.contains(filename);
  }
}

class StringListFile {
  File file;

  StringListFile(this.file);

  List<String> list() {
    try {
      return file.readAsLinesSync();
    } catch (e) {
      return [];
    }
  }

  add(String entry) {
    var items = list();
    if (items.contains(entry)) return;
    // append the item to the file
    file.writeAsString('$entry\n', mode: FileMode.writeOnlyAppend);
  }

  remove(List<String> entries) {
    var items = list();
    // filter current items
    items.removeWhere((entry) => entries.contains(entry));
    _write(items);
  }

  // replace entire list
  void _write(List<String> entries) {
    var ofile = file.openSync(mode: FileMode.writeOnly);
    for (String entry in entries) {
      ofile.writeString('$entry\n');
    }
    ofile.closeSync();
  }
}
