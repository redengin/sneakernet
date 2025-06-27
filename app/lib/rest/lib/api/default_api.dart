//
// AUTO-GENERATED FILE, DO NOT MODIFY!
//
// @dart=2.18

// ignore_for_file: unused_element, unused_import
// ignore_for_file: always_put_required_named_parameters_first
// ignore_for_file: constant_identifier_names
// ignore_for_file: lines_longer_than_80_chars

part of openapi.api;

class DefaultApi {
  DefaultApi([ApiClient? apiClient])
      : apiClient = apiClient ?? defaultApiClient;

  final ApiClient apiClient;

  /// remove empty subfolder
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  Future<Response> catalogPathDeleteWithHttpInfo(
    String path,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{path}/'.replaceAll('{path}', path);

    // ignore: prefer_final_locals
    Object? postBody;

    final queryParams = <QueryParam>[];
    final headerParams = <String, String>{};
    final formParams = <String, String>{};

    const contentTypes = <String>[];

    return apiClient.invokeAPI(
      path,
      'DELETE',
      queryParams,
      postBody,
      headerParams,
      formParams,
      contentTypes.isEmpty ? null : contentTypes.first,
    );
  }

  /// remove empty subfolder
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  Future<void> catalogPathDelete(
    String path,
  ) async {
    final response = await catalogPathDeleteWithHttpInfo(
      path,
    );
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// remove a file
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  ///
  /// * [String] file (required):
  ///
  /// * [int] contentLength (required):
  Future<Response> catalogPathFileDeleteWithHttpInfo(
    String path,
    String file,
    int contentLength,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{path}/{file}'
        .replaceAll('{path}', path)
        .replaceAll('{file}', file);

    // ignore: prefer_final_locals
    Object? postBody;

    final queryParams = <QueryParam>[];
    final headerParams = <String, String>{};
    final formParams = <String, String>{};

    headerParams[r'Content-Length'] = parameterToString(contentLength);

    const contentTypes = <String>[];

    return apiClient.invokeAPI(
      path,
      'DELETE',
      queryParams,
      postBody,
      headerParams,
      formParams,
      contentTypes.isEmpty ? null : contentTypes.first,
    );
  }

  /// remove a file
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  ///
  /// * [String] file (required):
  ///
  /// * [int] contentLength (required):
  Future<void> catalogPathFileDelete(
    String path,
    String file,
    int contentLength,
  ) async {
    final response = await catalogPathFileDeleteWithHttpInfo(
      path,
      file,
      contentLength,
    );
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// download file
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  ///
  /// * [String] file (required):
  ///
  /// * [int] contentLength (required):
  Future<Response> catalogPathFileGetWithHttpInfo(
    String path,
    String file,
    int contentLength,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{path}/{file}'
        .replaceAll('{path}', path)
        .replaceAll('{file}', file);

    // ignore: prefer_final_locals
    Object? postBody;

    final queryParams = <QueryParam>[];
    final headerParams = <String, String>{};
    final formParams = <String, String>{};

    headerParams[r'Content-Length'] = parameterToString(contentLength);

    const contentTypes = <String>[];

    return apiClient.invokeAPI(
      path,
      'GET',
      queryParams,
      postBody,
      headerParams,
      formParams,
      contentTypes.isEmpty ? null : contentTypes.first,
    );
  }

  /// download file
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  ///
  /// * [String] file (required):
  ///
  /// * [int] contentLength (required):
  Future<void> catalogPathFileGet(
    String path,
    String file,
    int contentLength,
  ) async {
    final response = await catalogPathFileGetWithHttpInfo(
      path,
      file,
      contentLength,
    );
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// add a file
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [int] contentLength (required):
  ///
  /// * [String] xTimestamp (required):
  ///
  /// * [String] path (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogPathFilePutWithHttpInfo(
    int contentLength,
    String xTimestamp,
    String path,
    String file,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{path}/{file}'
        .replaceAll('{path}', path)
        .replaceAll('{file}', file);

    // ignore: prefer_final_locals
    Object? postBody;

    final queryParams = <QueryParam>[];
    final headerParams = <String, String>{};
    final formParams = <String, String>{};

    headerParams[r'Content-Length'] = parameterToString(contentLength);
    headerParams[r'X-timestamp'] = parameterToString(xTimestamp);

    const contentTypes = <String>[];

    return apiClient.invokeAPI(
      path,
      'PUT',
      queryParams,
      postBody,
      headerParams,
      formParams,
      contentTypes.isEmpty ? null : contentTypes.first,
    );
  }

  /// add a file
  ///
  /// Parameters:
  ///
  /// * [int] contentLength (required):
  ///
  /// * [String] xTimestamp (required):
  ///
  /// * [String] path (required):
  ///
  /// * [String] file (required):
  Future<void> catalogPathFilePut(
    int contentLength,
    String xTimestamp,
    String path,
    String file,
  ) async {
    final response = await catalogPathFilePutWithHttpInfo(
      contentLength,
      xTimestamp,
      path,
      file,
    );
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// download icon
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogPathFileiconGetWithHttpInfo(
    String path,
    String file,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{path}/{file}?icon'
        .replaceAll('{path}', path)
        .replaceAll('{file}', file);

    // ignore: prefer_final_locals
    Object? postBody;

    final queryParams = <QueryParam>[];
    final headerParams = <String, String>{};
    final formParams = <String, String>{};

    const contentTypes = <String>[];

    return apiClient.invokeAPI(
      path,
      'GET',
      queryParams,
      postBody,
      headerParams,
      formParams,
      contentTypes.isEmpty ? null : contentTypes.first,
    );
  }

  /// download icon
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  ///
  /// * [String] file (required):
  Future<void> catalogPathFileiconGet(
    String path,
    String file,
  ) async {
    final response = await catalogPathFileiconGetWithHttpInfo(
      path,
      file,
    );
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// upload a new icon
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogPathFileiconPutWithHttpInfo(
    String path,
    String file,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{path}/{file}?icon'
        .replaceAll('{path}', path)
        .replaceAll('{file}', file);

    // ignore: prefer_final_locals
    Object? postBody;

    final queryParams = <QueryParam>[];
    final headerParams = <String, String>{};
    final formParams = <String, String>{};

    const contentTypes = <String>['application/octet-stream'];

    return apiClient.invokeAPI(
      path,
      'PUT',
      queryParams,
      postBody,
      headerParams,
      formParams,
      contentTypes.isEmpty ? null : contentTypes.first,
    );
  }

  /// upload a new icon
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  ///
  /// * [String] file (required):
  Future<void> catalogPathFileiconPut(
    String path,
    String file,
  ) async {
    final response = await catalogPathFileiconPutWithHttpInfo(
      path,
      file,
    );
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// set the title for the file
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] title (required):
  ///
  /// * [String] path (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogPathFiletitletitlePutWithHttpInfo(
    String title,
    String path,
    String file,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{path}/{file}?title={title}'
        .replaceAll('{path}', path)
        .replaceAll('{file}', file);

    // ignore: prefer_final_locals
    Object? postBody;

    final queryParams = <QueryParam>[];
    final headerParams = <String, String>{};
    final formParams = <String, String>{};

    queryParams.addAll(_queryParams('', 'title', title));

    const contentTypes = <String>[];

    return apiClient.invokeAPI(
      path,
      'PUT',
      queryParams,
      postBody,
      headerParams,
      formParams,
      contentTypes.isEmpty ? null : contentTypes.first,
    );
  }

  /// set the title for the file
  ///
  /// Parameters:
  ///
  /// * [String] title (required):
  ///
  /// * [String] path (required):
  ///
  /// * [String] file (required):
  Future<void> catalogPathFiletitletitlePut(
    String title,
    String path,
    String file,
  ) async {
    final response = await catalogPathFiletitletitlePutWithHttpInfo(
      title,
      path,
      file,
    );
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// get folder content
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  Future<Response> catalogPathGetWithHttpInfo(
    String path,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{path}/'.replaceAll('{path}', path);

    // ignore: prefer_final_locals
    Object? postBody;

    final queryParams = <QueryParam>[];
    final headerParams = <String, String>{};
    final formParams = <String, String>{};

    const contentTypes = <String>[];

    return apiClient.invokeAPI(
      path,
      'GET',
      queryParams,
      postBody,
      headerParams,
      formParams,
      contentTypes.isEmpty ? null : contentTypes.first,
    );
  }

  /// get folder content
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  Future<Folder?> catalogPathGet(
    String path,
  ) async {
    final response = await catalogPathGetWithHttpInfo(
      path,
    );
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
    // When a remote server returns no body with a status of 204, we shall not decode it.
    // At the time of writing this, `dart:convert` will throw an "Unexpected end of input"
    // FormatException when trying to decode an empty string.
    if (response.body.isNotEmpty &&
        response.statusCode != HttpStatus.noContent) {
      return await apiClient.deserializeAsync(
        await _decodeBodyBytes(response),
        'Folder',
      ) as Folder;
    }
    return null;
  }

  /// create subfolder
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  Future<Response> catalogPathPutWithHttpInfo(
    String path,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{path}/'.replaceAll('{path}', path);

    // ignore: prefer_final_locals
    Object? postBody;

    final queryParams = <QueryParam>[];
    final headerParams = <String, String>{};
    final formParams = <String, String>{};

    const contentTypes = <String>[];

    return apiClient.invokeAPI(
      path,
      'PUT',
      queryParams,
      postBody,
      headerParams,
      formParams,
      contentTypes.isEmpty ? null : contentTypes.first,
    );
  }

  /// create subfolder
  ///
  /// Parameters:
  ///
  /// * [String] path (required):
  Future<void> catalogPathPut(
    String path,
  ) async {
    final response = await catalogPathPutWithHttpInfo(
      path,
    );
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }
}
