//
// AUTO-GENERATED FILE, DO NOT MODIFY!
//
// @dart=2.12

// ignore_for_file: unused_element, unused_import
// ignore_for_file: always_put_required_named_parameters_first
// ignore_for_file: constant_identifier_names
// ignore_for_file: lines_longer_than_80_chars

part of openapi.api;

class FirmwareGet200Response {
  /// Returns a new [FirmwareGet200Response] instance.
  FirmwareGet200Response({
    required this.filename,
    required this.version,
  });

  String filename;

  String version;

  @override
  bool operator ==(Object other) => identical(this, other) || other is FirmwareGet200Response &&
     other.filename == filename &&
     other.version == version;

  @override
  int get hashCode =>
    // ignore: unnecessary_parenthesis
    (filename.hashCode) +
    (version.hashCode);

  @override
  String toString() => 'FirmwareGet200Response[filename=$filename, version=$version]';

  Map<String, dynamic> toJson() {
    final json = <String, dynamic>{};
      json[r'filename'] = this.filename;
      json[r'version'] = this.version;
    return json;
  }

  /// Returns a new [FirmwareGet200Response] instance and imports its values from
  /// [value] if it's a [Map], null otherwise.
  // ignore: prefer_constructors_over_static_methods
  static FirmwareGet200Response? fromJson(dynamic value) {
    if (value is Map) {
      final json = value.cast<String, dynamic>();

      // Ensure that the map contains the required keys.
      // Note 1: the values aren't checked for validity beyond being non-null.
      // Note 2: this code is stripped in release mode!
      assert(() {
        requiredKeys.forEach((key) {
          assert(json.containsKey(key), 'Required key "FirmwareGet200Response[$key]" is missing from JSON.');
          assert(json[key] != null, 'Required key "FirmwareGet200Response[$key]" has a null value in JSON.');
        });
        return true;
      }());

      return FirmwareGet200Response(
        filename: mapValueOfType<String>(json, r'filename')!,
        version: mapValueOfType<String>(json, r'version')!,
      );
    }
    return null;
  }

  static List<FirmwareGet200Response> listFromJson(dynamic json, {bool growable = false,}) {
    final result = <FirmwareGet200Response>[];
    if (json is List && json.isNotEmpty) {
      for (final row in json) {
        final value = FirmwareGet200Response.fromJson(row);
        if (value != null) {
          result.add(value);
        }
      }
    }
    return result.toList(growable: growable);
  }

  static Map<String, FirmwareGet200Response> mapFromJson(dynamic json) {
    final map = <String, FirmwareGet200Response>{};
    if (json is Map && json.isNotEmpty) {
      json = json.cast<String, dynamic>(); // ignore: parameter_assignments
      for (final entry in json.entries) {
        final value = FirmwareGet200Response.fromJson(entry.value);
        if (value != null) {
          map[entry.key] = value;
        }
      }
    }
    return map;
  }

  // maps a json object with a list of FirmwareGet200Response-objects as value to a dart map
  static Map<String, List<FirmwareGet200Response>> mapListFromJson(dynamic json, {bool growable = false,}) {
    final map = <String, List<FirmwareGet200Response>>{};
    if (json is Map && json.isNotEmpty) {
      // ignore: parameter_assignments
      json = json.cast<String, dynamic>();
      for (final entry in json.entries) {
        map[entry.key] = FirmwareGet200Response.listFromJson(entry.value, growable: growable,);
      }
    }
    return map;
  }

  /// The list of required keys that must be present in a JSON.
  static const requiredKeys = <String>{
    'filename',
    'version',
  };
}

