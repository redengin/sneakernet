// Openapi Generator last run: : 2024-02-23T23:16:11.456568
import 'package:openapi_generator_annotations/openapi_generator_annotations.dart';

@Openapi(
    additionalProperties: AdditionalProperties(pubName: 'sneakernet_rest'),
    alwaysRun: true,
    inputSpecFile: '../openapi/sneakernet.yaml',
    generatorName: Generator.dart,
    outputDirectory: 'lib/rest')
class SneakernetRest extends OpenapiGeneratorConfig {}