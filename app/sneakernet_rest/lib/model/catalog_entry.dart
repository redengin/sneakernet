//
// AUTO-GENERATED FILE, DO NOT MODIFY!
//
// @dart=2.18

// ignore_for_file: unused_element, unused_import
// ignore_for_file: always_put_required_named_parameters_first
// ignore_for_file: constant_identifier_names
// ignore_for_file: lines_longer_than_80_chars

part of openapi.api;

class CatalogEntry {
  /// Returns a new [CatalogEntry] instance.
  CatalogEntry({
    required this.isFolder,
    required this.hasIcon,
    required this.size,
    required this.timestamp,
    this.title,
  });

  bool isFolder;

  bool hasIcon;

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

  @override
  bool operator ==(Object other) => identical(this, other) || other is CatalogEntry &&
    other.isFolder == isFolder &&
    other.hasIcon == hasIcon &&
    other.size == size &&
    other.timestamp == timestamp &&
    other.title == title;

  @override
  int get hashCode =>
    // ignore: unnecessary_parenthesis
    (isFolder.hashCode) +
    (hasIcon.hashCode) +
    (size.hashCode) +
    (timestamp.hashCode) +
    (title == null ? 0 : title!.hashCode);

  @override
  String toString() => 'CatalogEntry[isFolder=$isFolder, hasIcon=$hasIcon, size=$size, timestamp=$timestamp, title=$title]';

  Map<String, dynamic> toJson() {
    final json = <String, dynamic>{};
      json[r'isFolder'] = this.isFolder;
      json[r'hasIcon'] = this.hasIcon;
      json[r'size'] = this.size;
      json[r'timestamp'] = this.timestamp;
    if (this.title != null) {
      json[r'title'] = this.title;
    } else {
      json[r'title'] = null;
    }
    return json;
  }

  /// Returns a new [CatalogEntry] instance and imports its values from
  /// [value] if it's a [Map], null otherwise.
  // ignore: prefer_constructors_over_static_methods
  static CatalogEntry? fromJson(dynamic value) {
    if (value is Map) {
      final json = value.cast<String, dynamic>();

      // Ensure that the map contains the required keys.
      // Note 1: the values aren't checked for validity beyond being non-null.
      // Note 2: this code is stripped in release mode!
      assert(() {
        requiredKeys.forEach((key) {
          assert(json.containsKey(key), 'Required key "CatalogEntry[$key]" is missing from JSON.');
          assert(json[key] != null, 'Required key "CatalogEntry[$key]" has a null value in JSON.');
        });
        return true;
      }());

      return CatalogEntry(
        isFolder: mapValueOfType<bool>(json, r'isFolder')!,
        hasIcon: mapValueOfType<bool>(json, r'hasIcon')!,
        size: mapValueOfType<int>(json, r'size')!,
        timestamp: mapValueOfType<String>(json, r'timestamp')!,
        title: mapValueOfType<String>(json, r'title'),
      );
    }
    return null;
  }

  static List<CatalogEntry> listFromJson(dynamic json, {bool growable = false,}) {
    final result = <CatalogEntry>[];
    if (json is List && json.isNotEmpty) {
      for (final row in json) {
        final value = CatalogEntry.fromJson(row);
        if (value != null) {
          result.add(value);
        }
      }
    }
    return result.toList(growable: growable);
  }

  static Map<String, CatalogEntry> mapFromJson(dynamic json) {
    final map = <String, CatalogEntry>{};
    if (json is Map && json.isNotEmpty) {
      json = json.cast<String, dynamic>(); // ignore: parameter_assignments
      for (final entry in json.entries) {
        final value = CatalogEntry.fromJson(entry.value);
        if (value != null) {
          map[entry.key] = value;
        }
      }
    }
    return map;
  }

  // maps a json object with a list of CatalogEntry-objects as value to a dart map
  static Map<String, List<CatalogEntry>> mapListFromJson(dynamic json, {bool growable = false,}) {
    final map = <String, List<CatalogEntry>>{};
    if (json is Map && json.isNotEmpty) {
      // ignore: parameter_assignments
      json = json.cast<String, dynamic>();
      for (final entry in json.entries) {
        map[entry.key] = CatalogEntry.listFromJson(entry.value, growable: growable,);
      }
    }
    return map;
  }

  /// The list of required keys that must be present in a JSON.
  static const requiredKeys = <String>{
    'isFolder',
    'hasIcon',
    'size',
    'timestamp',
  };
}

