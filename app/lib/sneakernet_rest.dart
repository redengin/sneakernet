// Openapi Generator last run: : 2025-06-27T15:52:31.264988
import 'package:openapi_generator_annotations/openapi_generator_annotations.dart';

@Openapi(
    // additionalProperties: AdditionalProperties(pubName: 'sneakernet_rest'),
    inputSpec: InputSpec(path: '../openapi/catalog.yml'),
    skipSpecValidation: true,   // FIXME
    generatorName: Generator.dart,
    outputDirectory: 'lib/rest',
    skipIfSpecIsUnchanged: false,
)
class SneakernetRest {}

/// to generate
///```sh
/// dart run build_runner build --delete-conflicting-outputs
///```