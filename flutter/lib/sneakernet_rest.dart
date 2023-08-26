import 'package:openapi_generator_annotations/openapi_generator_annotations.dart';

// to generate use:
//    dart run build_runner clean
//    dart run build_runner build --delete-conflicting-outputs
@Openapi(
    additionalProperties: AdditionalProperties(pubName: 'sneakernet_rest'),
    alwaysRun: true,
    inputSpecFile: '../openapi/sneakernet.yaml',
    generatorName: Generator.dart,
    outputDirectory: 'lib/rest')
class SneakernetRest extends OpenapiGeneratorConfig {}