# sneakernet_rest.api.DefaultApi

## Load the API package
```dart
import 'package:sneakernet_rest/api.dart';
```

All URIs are relative to *http://192.168.4.1/api*

Method | HTTP request | Description
------------- | ------------- | -------------
[**catalogContentsGet**](DefaultApi.md#catalogcontentsget) | **GET** /catalog.contents | provides a current listing of all folders/files
[**catalogFolderDelete**](DefaultApi.md#catalogfolderdelete) | **DELETE** /catalog/{folder}/ | remove folder
[**catalogFolderFileDelete**](DefaultApi.md#catalogfolderfiledelete) | **DELETE** /catalog/{folder}/{file} | remove the file
[**catalogFolderFileGet**](DefaultApi.md#catalogfolderfileget) | **GET** /catalog/{folder}/{file} | download the file
[**catalogFolderFilePut**](DefaultApi.md#catalogfolderfileput) | **PUT** /catalog/{folder}/{file} | add a file to the catalog
[**catalogFolderGet**](DefaultApi.md#catalogfolderget) | **GET** /catalog/{folder}/ | list folder content (subfolders and files)
[**catalogFolderPut**](DefaultApi.md#catalogfolderput) | **PUT** /catalog/{folder}/ | create folder
[**catalogIconFolderDelete**](DefaultApi.md#catalogiconfolderdelete) | **DELETE** /catalog.icon/{folder}/ | remove the icon for the folder
[**catalogIconFolderFileDelete**](DefaultApi.md#catalogiconfolderfiledelete) | **DELETE** /catalog.icon/{folder}/{file} | remove the icon for the file
[**catalogIconFolderFileGet**](DefaultApi.md#catalogiconfolderfileget) | **GET** /catalog.icon/{folder}/{file} | get the icon for the file
[**catalogIconFolderFilePut**](DefaultApi.md#catalogiconfolderfileput) | **PUT** /catalog.icon/{folder}/{file} | set the icon for the file
[**catalogIconFolderGet**](DefaultApi.md#catalogiconfolderget) | **GET** /catalog.icon/{folder}/ | get the icon for the folder
[**catalogIconFolderPut**](DefaultApi.md#catalogiconfolderput) | **PUT** /catalog.icon/{folder}/ | set the icon for the folder
[**catalogTitleFolderFileDelete**](DefaultApi.md#catalogtitlefolderfiledelete) | **DELETE** /catalog.title/{folder}/{file} | remove the title for the file
[**catalogTitleFolderFilePut**](DefaultApi.md#catalogtitlefolderfileput) | **PUT** /catalog.title/{folder}/{file} | set the title for the file
[**firmwareGet**](DefaultApi.md#firmwareget) | **GET** /firmware | firmware info
[**firmwarePut**](DefaultApi.md#firmwareput) | **PUT** /firmware | update firmware


# **catalogContentsGet**
> Map<String, CatalogEntry> catalogContentsGet()

provides a current listing of all folders/files

used for synchronization

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();

try {
    final result = api_instance.catalogContentsGet();
    print(result);
} catch (e) {
    print('Exception when calling DefaultApi->catalogContentsGet: $e\n');
}
```

### Parameters
This endpoint does not need any parameter.

### Return type

[**Map<String, CatalogEntry>**](CatalogEntry.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFolderDelete**
> catalogFolderDelete(folder)

remove folder

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | an empty value will access the catalog root

try {
    api_instance.catalogFolderDelete(folder);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderDelete: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **folder** | **String**| an empty value will access the catalog root | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFolderFileDelete**
> catalogFolderFileDelete(folder, file)

remove the file

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogFolderFileDelete(folder, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderFileDelete: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **folder** | **String**|  | 
 **file** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFolderFileGet**
> catalogFolderFileGet(folder, file)

download the file

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogFolderFileGet(folder, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderFileGet: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **folder** | **String**|  | 
 **file** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/octet-stream

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFolderFilePut**
> catalogFolderFilePut(contentLength, xTimestamp, folder, file, title)

add a file to the catalog

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final contentLength = 56; // int | 
final xTimestamp = xTimestamp_example; // String | 
final folder = folder_example; // String | 
final file = file_example; // String | 
final title = title_example; // String | 

try {
    api_instance.catalogFolderFilePut(contentLength, xTimestamp, folder, file, title);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderFilePut: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **contentLength** | **int**|  | 
 **xTimestamp** | **String**|  | 
 **folder** | **String**|  | 
 **file** | **String**|  | 
 **title** | **String**|  | [optional] 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/octet-stream
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFolderGet**
> Map<String, CatalogEntry> catalogFolderGet(folder)

list folder content (subfolders and files)

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | an empty value will access the catalog root

try {
    final result = api_instance.catalogFolderGet(folder);
    print(result);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderGet: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **folder** | **String**| an empty value will access the catalog root | 

### Return type

[**Map<String, CatalogEntry>**](CatalogEntry.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFolderPut**
> catalogFolderPut(folder)

create folder

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | an empty value will access the catalog root

try {
    api_instance.catalogFolderPut(folder);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderPut: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **folder** | **String**| an empty value will access the catalog root | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogIconFolderDelete**
> catalogIconFolderDelete(folder)

remove the icon for the folder

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 

try {
    api_instance.catalogIconFolderDelete(folder);
} catch (e) {
    print('Exception when calling DefaultApi->catalogIconFolderDelete: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **folder** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogIconFolderFileDelete**
> catalogIconFolderFileDelete(folder, file)

remove the icon for the file

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogIconFolderFileDelete(folder, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogIconFolderFileDelete: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **folder** | **String**|  | 
 **file** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogIconFolderFileGet**
> catalogIconFolderFileGet(folder, file)

get the icon for the file

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogIconFolderFileGet(folder, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogIconFolderFileGet: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **folder** | **String**|  | 
 **file** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/octet-stream

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogIconFolderFilePut**
> catalogIconFolderFilePut(contentLength, folder, file)

set the icon for the file

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final contentLength = 56; // int | 
final folder = folder_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogIconFolderFilePut(contentLength, folder, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogIconFolderFilePut: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **contentLength** | **int**|  | 
 **folder** | **String**|  | 
 **file** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/octet-stream
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogIconFolderGet**
> catalogIconFolderGet(folder)

get the icon for the folder

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 

try {
    api_instance.catalogIconFolderGet(folder);
} catch (e) {
    print('Exception when calling DefaultApi->catalogIconFolderGet: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **folder** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/octet-stream

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogIconFolderPut**
> catalogIconFolderPut(contentLength, folder)

set the icon for the folder

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final contentLength = 56; // int | 
final folder = folder_example; // String | 

try {
    api_instance.catalogIconFolderPut(contentLength, folder);
} catch (e) {
    print('Exception when calling DefaultApi->catalogIconFolderPut: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **contentLength** | **int**|  | 
 **folder** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: image/*
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogTitleFolderFileDelete**
> catalogTitleFolderFileDelete(folder, file)

remove the title for the file

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogTitleFolderFileDelete(folder, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogTitleFolderFileDelete: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **folder** | **String**|  | 
 **file** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogTitleFolderFilePut**
> catalogTitleFolderFilePut(title, folder, file)

set the title for the file

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final title = title_example; // String | 
final folder = folder_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogTitleFolderFilePut(title, folder, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogTitleFolderFilePut: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **title** | **String**|  | 
 **folder** | **String**|  | 
 **file** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **firmwareGet**
> FirmwareGet200Response firmwareGet()

firmware info

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();

try {
    final result = api_instance.firmwareGet();
    print(result);
} catch (e) {
    print('Exception when calling DefaultApi->firmwareGet: $e\n');
}
```

### Parameters
This endpoint does not need any parameter.

### Return type

[**FirmwareGet200Response**](FirmwareGet200Response.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **firmwarePut**
> firmwarePut()

update firmware

new firmware must be new revision than current

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();

try {
    api_instance.firmwarePut();
} catch (e) {
    print('Exception when calling DefaultApi->firmwarePut: $e\n');
}
```

### Parameters
This endpoint does not need any parameter.

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/octet-stream
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

