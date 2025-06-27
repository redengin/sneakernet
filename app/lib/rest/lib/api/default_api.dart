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
  /// * [String] folder (required):
  Future<Response> catalogFolderDeleteWithHttpInfo(
    String folder,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/'.replaceAll('{folder}', folder);

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
  /// * [String] folder (required):
  Future<void> catalogFolderDelete(
    String folder,
  ) async {
    final response = await catalogFolderDeleteWithHttpInfo(
      folder,
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
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  ///
  /// * [int] contentLength (required):
  Future<Response> catalogFolderFileDeleteWithHttpInfo(
    String folder,
    String file,
    int contentLength,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/{file}'
        .replaceAll('{folder}', folder)
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
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  ///
  /// * [int] contentLength (required):
  Future<void> catalogFolderFileDelete(
    String folder,
    String file,
    int contentLength,
  ) async {
    final response = await catalogFolderFileDeleteWithHttpInfo(
      folder,
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
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  ///
  /// * [int] contentLength (required):
  Future<Response> catalogFolderFileGetWithHttpInfo(
    String folder,
    String file,
    int contentLength,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/{file}'
        .replaceAll('{folder}', folder)
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
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  ///
  /// * [int] contentLength (required):
  Future<void> catalogFolderFileGet(
    String folder,
    String file,
    int contentLength,
  ) async {
    final response = await catalogFolderFileGetWithHttpInfo(
      folder,
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
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogFolderFilePutWithHttpInfo(
    int contentLength,
    String xTimestamp,
    String folder,
    String file,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/{file}'
        .replaceAll('{folder}', folder)
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
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<void> catalogFolderFilePut(
    int contentLength,
    String xTimestamp,
    String folder,
    String file,
  ) async {
    final response = await catalogFolderFilePutWithHttpInfo(
      contentLength,
      xTimestamp,
      folder,
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
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogFolderFileiconGetWithHttpInfo(
    String folder,
    String file,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/{file}?icon'
        .replaceAll('{folder}', folder)
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
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<void> catalogFolderFileiconGet(
    String folder,
    String file,
  ) async {
    final response = await catalogFolderFileiconGetWithHttpInfo(
      folder,
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
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogFolderFileiconPutWithHttpInfo(
    String folder,
    String file,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/{file}?icon'
        .replaceAll('{folder}', folder)
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
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<void> catalogFolderFileiconPut(
    String folder,
    String file,
  ) async {
    final response = await catalogFolderFileiconPutWithHttpInfo(
      folder,
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
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogFolderFiletitletitlePutWithHttpInfo(
    String title,
    String folder,
    String file,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/{file}?title={title}'
        .replaceAll('{folder}', folder)
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
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<void> catalogFolderFiletitletitlePut(
    String title,
    String folder,
    String file,
  ) async {
    final response = await catalogFolderFiletitletitlePutWithHttpInfo(
      title,
      folder,
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
  /// * [String] folder (required):
  Future<Response> catalogFolderGetWithHttpInfo(
    String folder,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/'.replaceAll('{folder}', folder);

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
  /// * [String] folder (required):
  Future<Folder?> catalogFolderGet(
    String folder,
  ) async {
    final response = await catalogFolderGetWithHttpInfo(
      folder,
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
  /// * [String] folder (required):
  Future<Response> catalogFolderPutWithHttpInfo(
    String folder,
  ) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/'.replaceAll('{folder}', folder);

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
  /// * [String] folder (required):
  Future<void> catalogFolderPut(
    String folder,
  ) async {
    final response = await catalogFolderPutWithHttpInfo(
      folder,
    );
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }
}
