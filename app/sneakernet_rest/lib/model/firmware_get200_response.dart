//
// AUTO-GENERATED FILE, DO NOT MODIFY!
//
// @dart=2.18

// ignore_for_file: unused_element, unused_import
// ignore_for_file: always_put_required_named_parameters_first
// ignore_for_file: constant_identifier_names
// ignore_for_file: lines_longer_than_80_chars

part of openapi.api;

class FirmwareGet200Response {
  /// Returns a new [FirmwareGet200Response] instance.
  FirmwareGet200Response({
    this.version,
    this.hardware,
  });

  /// semantic firmware version
  ///
  /// Please note: This property should have been non-nullable! Since the specification file
  /// does not include a default value (using the "default:" property), however, the generated
  /// source code must fall back to having a nullable type.
  /// Consider adding a "default:" property in the specification file to hide this note.
  ///
  String? version;

  /// hardware type
  FirmwareGet200ResponseHardwareEnum? hardware;

  @override
  bool operator ==(Object other) => identical(this, other) || other is FirmwareGet200Response &&
    other.version == version &&
    other.hardware == hardware;

  @override
  int get hashCode =>
    // ignore: unnecessary_parenthesis
    (version == null ? 0 : version!.hashCode) +
    (hardware == null ? 0 : hardware!.hashCode);

  @override
  String toString() => 'FirmwareGet200Response[version=$version, hardware=$hardware]';

  Map<String, dynamic> toJson() {
    final json = <String, dynamic>{};
    if (this.version != null) {
      json[r'version'] = this.version;
    } else {
      json[r'version'] = null;
    }
    if (this.hardware != null) {
      json[r'hardware'] = this.hardware;
    } else {
      json[r'hardware'] = null;
    }
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
        version: mapValueOfType<String>(json, r'version'),
        hardware: FirmwareGet200ResponseHardwareEnum.fromJson(json[r'hardware']),
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
  };
}

/// hardware type
class FirmwareGet200ResponseHardwareEnum {
  /// Instantiate a new enum with the provided [value].
  const FirmwareGet200ResponseHardwareEnum._(this.value);

  /// The underlying value of this enum member.
  final String value;

  @override
  String toString() => value;

  String toJson() => value;

  static const eSP32 = FirmwareGet200ResponseHardwareEnum._(r'ESP32');

  /// List of all possible values in this [enum][FirmwareGet200ResponseHardwareEnum].
  static const values = <FirmwareGet200ResponseHardwareEnum>[
    eSP32,
  ];

  static FirmwareGet200ResponseHardwareEnum? fromJson(dynamic value) => FirmwareGet200ResponseHardwareEnumTypeTransformer().decode(value);

  static List<FirmwareGet200ResponseHardwareEnum> listFromJson(dynamic json, {bool growable = false,}) {
    final result = <FirmwareGet200ResponseHardwareEnum>[];
    if (json is List && json.isNotEmpty) {
      for (final row in json) {
        final value = FirmwareGet200ResponseHardwareEnum.fromJson(row);
        if (value != null) {
          result.add(value);
        }
      }
    }
    return result.toList(growable: growable);
  }
}

/// Transformation class that can [encode] an instance of [FirmwareGet200ResponseHardwareEnum] to String,
/// and [decode] dynamic data back to [FirmwareGet200ResponseHardwareEnum].
class FirmwareGet200ResponseHardwareEnumTypeTransformer {
  factory FirmwareGet200ResponseHardwareEnumTypeTransformer() => _instance ??= const FirmwareGet200ResponseHardwareEnumTypeTransformer._();

  const FirmwareGet200ResponseHardwareEnumTypeTransformer._();

  String encode(FirmwareGet200ResponseHardwareEnum data) => data.value;

  /// Decodes a [dynamic value][data] to a FirmwareGet200ResponseHardwareEnum.
  ///
  /// If [allowNull] is true and the [dynamic value][data] cannot be decoded successfully,
  /// then null is returned. However, if [allowNull] is false and the [dynamic value][data]
  /// cannot be decoded successfully, then an [UnimplementedError] is thrown.
  ///
  /// The [allowNull] is very handy when an API changes and a new enum value is added or removed,
  /// and users are still using an old app with the old code.
  FirmwareGet200ResponseHardwareEnum? decode(dynamic data, {bool allowNull = true}) {
    if (data != null) {
      switch (data) {
        case r'ESP32': return FirmwareGet200ResponseHardwareEnum.eSP32;
        default:
          if (!allowNull) {
            throw ArgumentError('Unknown enum value to decode: $data');
          }
      }
    }
    return null;
  }

  /// Singleton [FirmwareGet200ResponseHardwareEnumTypeTransformer] instance.
  static FirmwareGet200ResponseHardwareEnumTypeTransformer? _instance;
}


