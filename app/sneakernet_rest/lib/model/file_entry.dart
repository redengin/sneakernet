//
// AUTO-GENERATED FILE, DO NOT MODIFY!
//
// @dart=2.18

// ignore_for_file: unused_element, unused_import
// ignore_for_file: always_put_required_named_parameters_first
// ignore_for_file: constant_identifier_names
// ignore_for_file: lines_longer_than_80_chars

part of openapi.api;

class FileEntry {
  /// Returns a new [FileEntry] instance.
  FileEntry({
    required this.isFolder,
    required this.size,
    required this.timestamp,
    this.title,
    this.isLocked,
    this.hasIcon,
  });

  bool isFolder;

  /// size of the file in bytes
  int size;

  /// ISO 8601 Z
  String timestamp;

  ///
  /// Please note: This property should have been non-nullable! Since the specification file
  /// does not include a default value (using the "default:" property), however, the generated
  /// source code must fall back to having a nullable type.
  /// Consider adding a "default:" property in the specification file to hide this note.
  ///
  String? title;

  /// true if admin controls this folder
  ///
  /// Please note: This property should have been non-nullable! Since the specification file
  /// does not include a default value (using the "default:" property), however, the generated
  /// source code must fall back to having a nullable type.
  /// Consider adding a "default:" property in the specification file to hide this note.
  ///
  bool? isLocked;

  ///
  /// Please note: This property should have been non-nullable! Since the specification file
  /// does not include a default value (using the "default:" property), however, the generated
  /// source code must fall back to having a nullable type.
  /// Consider adding a "default:" property in the specification file to hide this note.
  ///
  bool? hasIcon;

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is FileEntry &&
          other.isFolder == isFolder &&
          other.size == size &&
          other.timestamp == timestamp &&
          other.title == title &&
          other.isLocked == isLocked &&
          other.hasIcon == hasIcon;

  @override
  int get hashCode =>
      // ignore: unnecessary_parenthesis
      (isFolder.hashCode) +
      (size.hashCode) +
      (timestamp.hashCode) +
      (title == null ? 0 : title!.hashCode) +
      (isLocked == null ? 0 : isLocked!.hashCode) +
      (hasIcon == null ? 0 : hasIcon!.hashCode);

  @override
  String toString() =>
      'FileEntry[isFolder=$isFolder, size=$size, timestamp=$timestamp, title=$title, isLocked=$isLocked, hasIcon=$hasIcon]';

  Map<String, dynamic> toJson() {
    final json = <String, dynamic>{};
    json[r'isFolder'] = this.isFolder;
    json[r'size'] = this.size;
    json[r'timestamp'] = this.timestamp;
    if (this.title != null) {
      json[r'title'] = this.title;
    } else {
      json[r'title'] = null;
    }
    if (this.isLocked != null) {
      json[r'isLocked'] = this.isLocked;
    } else {
      json[r'isLocked'] = null;
    }
    if (this.hasIcon != null) {
      json[r'hasIcon'] = this.hasIcon;
    } else {
      json[r'hasIcon'] = null;
    }
    return json;
  }

  /// Returns a new [FileEntry] instance and imports its values from
  /// [value] if it's a [Map], null otherwise.
  // ignore: prefer_constructors_over_static_methods
  static FileEntry? fromJson(dynamic value) {
    if (value is Map) {
      final json = value.cast<String, dynamic>();

      // Ensure that the map contains the required keys.
      // Note 1: the values aren't checked for validity beyond being non-null.
      // Note 2: this code is stripped in release mode!
      assert(() {
        requiredKeys.forEach((key) {
          assert(json.containsKey(key),
              'Required key "FileEntry[$key]" is missing from JSON.');
          assert(json[key] != null,
              'Required key "FileEntry[$key]" has a null value in JSON.');
        });
        return true;
      }());

      return FileEntry(
        isFolder: mapValueOfType<bool>(json, r'isFolder')!,
        size: mapValueOfType<int>(json, r'size')!,
        timestamp: mapValueOfType<String>(json, r'timestamp')!,
        title: mapValueOfType<String>(json, r'title'),
        isLocked: mapValueOfType<bool>(json, r'isLocked'),
        hasIcon: mapValueOfType<bool>(json, r'hasIcon'),
      );
    }
    return null;
  }

  static List<FileEntry> listFromJson(
    dynamic json, {
    bool growable = false,
  }) {
    final result = <FileEntry>[];
    if (json is List && json.isNotEmpty) {
      for (final row in json) {
        final value = FileEntry.fromJson(row);
        if (value != null) {
          result.add(value);
        }
      }
    }
    return result.toList(growable: growable);
  }

  static Map<String, FileEntry> mapFromJson(dynamic json) {
    final map = <String, FileEntry>{};
    if (json is Map && json.isNotEmpty) {
      json = json.cast<String, dynamic>(); // ignore: parameter_assignments
      for (final entry in json.entries) {
        final value = FileEntry.fromJson(entry.value);
        if (value != null) {
          map[entry.key] = value;
        }
      }
    }
    return map;
  }

  // maps a json object with a list of FileEntry-objects as value to a dart map
  static Map<String, List<FileEntry>> mapListFromJson(
    dynamic json, {
    bool growable = false,
  }) {
    final map = <String, List<FileEntry>>{};
    if (json is Map && json.isNotEmpty) {
      // ignore: parameter_assignments
      json = json.cast<String, dynamic>();
      for (final entry in json.entries) {
        map[entry.key] = FileEntry.listFromJson(
          entry.value,
          growable: growable,
        );
      }
    }
    return map;
  }

  /// The list of required keys that must be present in a JSON.
  static const requiredKeys = <String>{
    'isFolder',
    'size',
    'timestamp',
  };
}
