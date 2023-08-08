//
// AUTO-GENERATED FILE, DO NOT MODIFY!
//
// @dart=2.12

// ignore_for_file: unused_element, unused_import
// ignore_for_file: always_put_required_named_parameters_first
// ignore_for_file: constant_identifier_names
// ignore_for_file: lines_longer_than_80_chars

part of openapi.api;

class CatalogGet200ResponseInner {
  /// Returns a new [CatalogGet200ResponseInner] instance.
  CatalogGet200ResponseInner({
    required this.filename,
    this.size,
    required this.sha256,
    this.identifiers = const [],
    this.sneakernetSigned,
  });

  String filename;

  ///
  /// Please note: This property should have been non-nullable! Since the specification file
  /// does not include a default value (using the "default:" property), however, the generated
  /// source code must fall back to having a nullable type.
  /// Consider adding a "default:" property in the specification file to hide this note.
  ///
  int? size;

  String sha256;

  List<String> identifiers;

  ///
  /// Please note: This property should have been non-nullable! Since the specification file
  /// does not include a default value (using the "default:" property), however, the generated
  /// source code must fall back to having a nullable type.
  /// Consider adding a "default:" property in the specification file to hide this note.
  ///
  bool? sneakernetSigned;

  @override
  bool operator ==(Object other) => identical(this, other) || other is CatalogGet200ResponseInner &&
     other.filename == filename &&
     other.size == size &&
     other.sha256 == sha256 &&
     other.identifiers == identifiers &&
     other.sneakernetSigned == sneakernetSigned;

  @override
  int get hashCode =>
    // ignore: unnecessary_parenthesis
    (filename.hashCode) +
    (size == null ? 0 : size!.hashCode) +
    (sha256.hashCode) +
    (identifiers.hashCode) +
    (sneakernetSigned == null ? 0 : sneakernetSigned!.hashCode);

  @override
  String toString() => 'CatalogGet200ResponseInner[filename=$filename, size=$size, sha256=$sha256, identifiers=$identifiers, sneakernetSigned=$sneakernetSigned]';

  Map<String, dynamic> toJson() {
    final json = <String, dynamic>{};
      json[r'filename'] = this.filename;
    if (this.size != null) {
      json[r'size'] = this.size;
    } else {
      json[r'size'] = null;
    }
      json[r'sha256'] = this.sha256;
      json[r'identifiers'] = this.identifiers;
    if (this.sneakernetSigned != null) {
      json[r'sneakernetSigned'] = this.sneakernetSigned;
    } else {
      json[r'sneakernetSigned'] = null;
    }
    return json;
  }

  /// Returns a new [CatalogGet200ResponseInner] instance and imports its values from
  /// [value] if it's a [Map], null otherwise.
  // ignore: prefer_constructors_over_static_methods
  static CatalogGet200ResponseInner? fromJson(dynamic value) {
    if (value is Map) {
      final json = value.cast<String, dynamic>();

      // Ensure that the map contains the required keys.
      // Note 1: the values aren't checked for validity beyond being non-null.
      // Note 2: this code is stripped in release mode!
      assert(() {
        requiredKeys.forEach((key) {
          assert(json.containsKey(key), 'Required key "CatalogGet200ResponseInner[$key]" is missing from JSON.');
          assert(json[key] != null, 'Required key "CatalogGet200ResponseInner[$key]" has a null value in JSON.');
        });
        return true;
      }());

      return CatalogGet200ResponseInner(
        filename: mapValueOfType<String>(json, r'filename')!,
        size: mapValueOfType<int>(json, r'size'),
        sha256: mapValueOfType<String>(json, r'sha256')!,
        identifiers: json[r'identifiers'] is List
            ? (json[r'identifiers'] as List).cast<String>()
            : const [],
        sneakernetSigned: mapValueOfType<bool>(json, r'sneakernetSigned'),
      );
    }
    return null;
  }

  static List<CatalogGet200ResponseInner> listFromJson(dynamic json, {bool growable = false,}) {
    final result = <CatalogGet200ResponseInner>[];
    if (json is List && json.isNotEmpty) {
      for (final row in json) {
        final value = CatalogGet200ResponseInner.fromJson(row);
        if (value != null) {
          result.add(value);
        }
      }
    }
    return result.toList(growable: growable);
  }

  static Map<String, CatalogGet200ResponseInner> mapFromJson(dynamic json) {
    final map = <String, CatalogGet200ResponseInner>{};
    if (json is Map && json.isNotEmpty) {
      json = json.cast<String, dynamic>(); // ignore: parameter_assignments
      for (final entry in json.entries) {
        final value = CatalogGet200ResponseInner.fromJson(entry.value);
        if (value != null) {
          map[entry.key] = value;
        }
      }
    }
    return map;
  }

  // maps a json object with a list of CatalogGet200ResponseInner-objects as value to a dart map
  static Map<String, List<CatalogGet200ResponseInner>> mapListFromJson(dynamic json, {bool growable = false,}) {
    final map = <String, List<CatalogGet200ResponseInner>>{};
    if (json is Map && json.isNotEmpty) {
      // ignore: parameter_assignments
      json = json.cast<String, dynamic>();
      for (final entry in json.entries) {
        map[entry.key] = CatalogGet200ResponseInner.listFromJson(entry.value, growable: growable,);
      }
    }
    return map;
  }

  /// The list of required keys that must be present in a JSON.
  static const requiredKeys = <String>{
    'filename',
    'sha256',
  };
}

