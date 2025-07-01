// Openapi Generator last run: : 2025-07-01T12:49:12.135200
import 'package:openapi_generator_annotations/openapi_generator_annotations.dart';
import 'package:sneakernet_rest/api.dart';

/// generate rest API
///```sh
/// dart run build_runner build --delete-conflicting-outputs
///```
@Openapi(
    additionalProperties: AdditionalProperties(pubName: 'sneakernet_rest'),
    inputSpec: InputSpec(path: '../openapi/sneakernet.yml'),
    // skipSpecValidation: true,
    generatorName: Generator.dart,
    outputDirectory: 'sneakernet_rest',
    skipIfSpecIsUnchanged: false,
)
class SneakernetRest{}

class SneakerNet {
    static synchronize()
    {
        final client = DefaultApi();
    }
}