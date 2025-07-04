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
  DefaultApi([ApiClient? apiClient]) : apiClient = apiClient ?? defaultApiClient;

  final ApiClient apiClient;

  /// provides a current listing of all folders/files
  ///
  /// used for synchronization
  ///
  /// Note: This method returns the HTTP [Response].
  Future<Response> catalogContentsGetWithHttpInfo() async {
    // ignore: prefer_const_declarations
    final path = r'/catalog.contents';

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

  /// provides a current listing of all folders/files
  ///
  /// used for synchronization
  Future<Map<String, CatalogEntry>?> catalogContentsGet() async {
    final response = await catalogContentsGetWithHttpInfo();
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
    // When a remote server returns no body with a status of 204, we shall not decode it.
    // At the time of writing this, `dart:convert` will throw an "Unexpected end of input"
    // FormatException when trying to decode an empty string.
    if (response.body.isNotEmpty && response.statusCode != HttpStatus.noContent) {
      return Map<String, CatalogEntry>.from(await apiClient.deserializeAsync(await _decodeBodyBytes(response), 'Map<String, CatalogEntry>'),);

    }
    return null;
  }

  /// remove folder
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///   an empty value will access the catalog root
  Future<Response> catalogFolderDeleteWithHttpInfo(String folder,) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/'
      .replaceAll('{folder}', folder);

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

  /// remove folder
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///   an empty value will access the catalog root
  Future<void> catalogFolderDelete(String folder,) async {
    final response = await catalogFolderDeleteWithHttpInfo(folder,);
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// remove the file
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogFolderFileDeleteWithHttpInfo(String folder, String file,) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/{file}'
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
      'DELETE',
      queryParams,
      postBody,
      headerParams,
      formParams,
      contentTypes.isEmpty ? null : contentTypes.first,
    );
  }

  /// remove the file
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<void> catalogFolderFileDelete(String folder, String file,) async {
    final response = await catalogFolderFileDeleteWithHttpInfo(folder, file,);
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// download the file
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogFolderFileGetWithHttpInfo(String folder, String file,) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/{file}'
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

  /// download the file
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<void> catalogFolderFileGet(String folder, String file,) async {
    final response = await catalogFolderFileGetWithHttpInfo(folder, file,);
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// add a file to the catalog
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
  ///
  /// * [String] title:
  Future<Response> catalogFolderFilePutWithHttpInfo(int contentLength, String xTimestamp, String folder, String file, { String? title, }) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/{file}'
      .replaceAll('{folder}', folder)
      .replaceAll('{file}', file);

    // ignore: prefer_final_locals
    Object? postBody;

    final queryParams = <QueryParam>[];
    final headerParams = <String, String>{};
    final formParams = <String, String>{};

    if (title != null) {
      queryParams.addAll(_queryParams('', 'title', title));
    }

    headerParams[r'Content-Length'] = parameterToString(contentLength);
    headerParams[r'X-Timestamp'] = parameterToString(xTimestamp);

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

  /// add a file to the catalog
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
  ///
  /// * [String] title:
  Future<void> catalogFolderFilePut(int contentLength, String xTimestamp, String folder, String file, { String? title, }) async {
    final response = await catalogFolderFilePutWithHttpInfo(contentLength, xTimestamp, folder, file,  title: title, );
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// list folder content (subfolders and files)
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///   an empty value will access the catalog root
  Future<Response> catalogFolderGetWithHttpInfo(String folder,) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/'
      .replaceAll('{folder}', folder);

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

  /// list folder content (subfolders and files)
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///   an empty value will access the catalog root
  Future<Map<String, CatalogEntry>?> catalogFolderGet(String folder,) async {
    final response = await catalogFolderGetWithHttpInfo(folder,);
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
    // When a remote server returns no body with a status of 204, we shall not decode it.
    // At the time of writing this, `dart:convert` will throw an "Unexpected end of input"
    // FormatException when trying to decode an empty string.
    if (response.body.isNotEmpty && response.statusCode != HttpStatus.noContent) {
      return Map<String, CatalogEntry>.from(await apiClient.deserializeAsync(await _decodeBodyBytes(response), 'Map<String, CatalogEntry>'),);

    }
    return null;
  }

  /// create folder
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///   an empty value will access the catalog root
  Future<Response> catalogFolderPutWithHttpInfo(String folder,) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog/{folder}/'
      .replaceAll('{folder}', folder);

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

  /// create folder
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///   an empty value will access the catalog root
  Future<void> catalogFolderPut(String folder,) async {
    final response = await catalogFolderPutWithHttpInfo(folder,);
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// remove the icon for the folder
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  Future<Response> catalogIconFolderDeleteWithHttpInfo(String folder,) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog.icon/{folder}/'
      .replaceAll('{folder}', folder);

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

  /// remove the icon for the folder
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  Future<void> catalogIconFolderDelete(String folder,) async {
    final response = await catalogIconFolderDeleteWithHttpInfo(folder,);
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// remove the icon for the file
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogIconFolderFileDeleteWithHttpInfo(String folder, String file,) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog.icon/{folder}/{file}'
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
      'DELETE',
      queryParams,
      postBody,
      headerParams,
      formParams,
      contentTypes.isEmpty ? null : contentTypes.first,
    );
  }

  /// remove the icon for the file
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<void> catalogIconFolderFileDelete(String folder, String file,) async {
    final response = await catalogIconFolderFileDeleteWithHttpInfo(folder, file,);
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// get the icon for the file
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogIconFolderFileGetWithHttpInfo(String folder, String file,) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog.icon/{folder}/{file}'
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

  /// get the icon for the file
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<void> catalogIconFolderFileGet(String folder, String file,) async {
    final response = await catalogIconFolderFileGetWithHttpInfo(folder, file,);
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// set the icon for the file
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [int] contentLength (required):
  ///
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogIconFolderFilePutWithHttpInfo(int contentLength, String folder, String file,) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog.icon/{folder}/{file}'
      .replaceAll('{folder}', folder)
      .replaceAll('{file}', file);

    // ignore: prefer_final_locals
    Object? postBody;

    final queryParams = <QueryParam>[];
    final headerParams = <String, String>{};
    final formParams = <String, String>{};

    headerParams[r'Content-Length'] = parameterToString(contentLength);

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

  /// set the icon for the file
  ///
  /// Parameters:
  ///
  /// * [int] contentLength (required):
  ///
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<void> catalogIconFolderFilePut(int contentLength, String folder, String file,) async {
    final response = await catalogIconFolderFilePutWithHttpInfo(contentLength, folder, file,);
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// get the icon for the folder
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  Future<Response> catalogIconFolderGetWithHttpInfo(String folder,) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog.icon/{folder}/'
      .replaceAll('{folder}', folder);

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

  /// get the icon for the folder
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  Future<void> catalogIconFolderGet(String folder,) async {
    final response = await catalogIconFolderGetWithHttpInfo(folder,);
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// set the icon for the folder
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [int] contentLength (required):
  ///
  /// * [String] folder (required):
  Future<Response> catalogIconFolderPutWithHttpInfo(int contentLength, String folder,) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog.icon/{folder}/'
      .replaceAll('{folder}', folder);

    // ignore: prefer_final_locals
    Object? postBody;

    final queryParams = <QueryParam>[];
    final headerParams = <String, String>{};
    final formParams = <String, String>{};

    headerParams[r'Content-Length'] = parameterToString(contentLength);

    const contentTypes = <String>['image/*'];


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

  /// set the icon for the folder
  ///
  /// Parameters:
  ///
  /// * [int] contentLength (required):
  ///
  /// * [String] folder (required):
  Future<void> catalogIconFolderPut(int contentLength, String folder,) async {
    final response = await catalogIconFolderPutWithHttpInfo(contentLength, folder,);
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// remove the title for the file
  ///
  /// Note: This method returns the HTTP [Response].
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<Response> catalogTitleFolderFileDeleteWithHttpInfo(String folder, String file,) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog.title/{folder}/{file}'
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
      'DELETE',
      queryParams,
      postBody,
      headerParams,
      formParams,
      contentTypes.isEmpty ? null : contentTypes.first,
    );
  }

  /// remove the title for the file
  ///
  /// Parameters:
  ///
  /// * [String] folder (required):
  ///
  /// * [String] file (required):
  Future<void> catalogTitleFolderFileDelete(String folder, String file,) async {
    final response = await catalogTitleFolderFileDeleteWithHttpInfo(folder, file,);
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
  Future<Response> catalogTitleFolderFilePutWithHttpInfo(String title, String folder, String file,) async {
    // ignore: prefer_const_declarations
    final path = r'/catalog.title/{folder}/{file}'
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
  Future<void> catalogTitleFolderFilePut(String title, String folder, String file,) async {
    final response = await catalogTitleFolderFilePutWithHttpInfo(title, folder, file,);
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }

  /// firmware info
  ///
  /// Note: This method returns the HTTP [Response].
  Future<Response> firmwareGetWithHttpInfo() async {
    // ignore: prefer_const_declarations
    final path = r'/firmware';

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

  /// firmware info
  Future<FirmwareGet200Response?> firmwareGet() async {
    final response = await firmwareGetWithHttpInfo();
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
    // When a remote server returns no body with a status of 204, we shall not decode it.
    // At the time of writing this, `dart:convert` will throw an "Unexpected end of input"
    // FormatException when trying to decode an empty string.
    if (response.body.isNotEmpty && response.statusCode != HttpStatus.noContent) {
      return await apiClient.deserializeAsync(await _decodeBodyBytes(response), 'FirmwareGet200Response',) as FirmwareGet200Response;
    
    }
    return null;
  }

  /// update firmware
  ///
  /// new firmware must be new revision than current
  ///
  /// Note: This method returns the HTTP [Response].
  Future<Response> firmwarePutWithHttpInfo() async {
    // ignore: prefer_const_declarations
    final path = r'/firmware';

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

  /// update firmware
  ///
  /// new firmware must be new revision than current
  Future<void> firmwarePut() async {
    final response = await firmwarePutWithHttpInfo();
    if (response.statusCode >= HttpStatus.badRequest) {
      throw ApiException(response.statusCode, await _decodeBodyBytes(response));
    }
  }
}
