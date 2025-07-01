# sneakernet_rest.api.DefaultApi

## Load the API package
```dart
import 'package:sneakernet_rest/api.dart';
```

All URIs are relative to *http://192.168.4.1/api*

Method | HTTP request | Description
------------- | ------------- | -------------
[**catalogContentsGet**](DefaultApi.md#catalogcontentsget) | **GET** /catalog_contents | 
[**catalogFolderDelete**](DefaultApi.md#catalogfolderdelete) | **DELETE** /catalog/{folder}/ | 
[**catalogFolderFileDelete**](DefaultApi.md#catalogfolderfiledelete) | **DELETE** /catalog/{folder}/{file} | 
[**catalogFolderFileGet**](DefaultApi.md#catalogfolderfileget) | **GET** /catalog/{folder}/{file} | 
[**catalogFolderFilePut**](DefaultApi.md#catalogfolderfileput) | **PUT** /catalog/{folder}/{file} | 
[**catalogFolderGet**](DefaultApi.md#catalogfolderget) | **GET** /catalog/{folder}/ | 
[**catalogFolderPost**](DefaultApi.md#catalogfolderpost) | **POST** /catalog/{folder}/ | 
[**catalogFolderPut**](DefaultApi.md#catalogfolderput) | **PUT** /catalog/{folder}/ | 
[**catalogIconFolderFilePut**](DefaultApi.md#catalogiconfolderfileput) | **PUT** /catalog_icon/{folder}/{file} | 
[**catalogTitleFolderFilePut**](DefaultApi.md#catalogtitlefolderfileput) | **PUT** /catalog_title/{folder}/{file} | 


# **catalogContentsGet**
> Map<String, CatalogEntry> catalogContentsGet()



used for synchronization - provides a current listing of all folders/files

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



remove subfolder

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
> catalogFolderFilePut(contentLength, folder, file, title)



add a file to the catalog

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final contentLength = 56; // int | 
final folder = folder_example; // String | 
final file = file_example; // String | 
final title = title_example; // String | 

try {
    api_instance.catalogFolderFilePut(contentLength, folder, file, title);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderFilePut: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **contentLength** | **int**|  | 
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
> CatalogEntry catalogFolderGet(folder)



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

[**CatalogEntry**](CatalogEntry.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFolderPost**
> catalogFolderPost(icon, contentLength, folder)



change the 'icon' for the folder

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final icon = true; // bool | 
final contentLength = 56; // int | 
final folder = folder_example; // String | an empty value will access the catalog root

try {
    api_instance.catalogFolderPost(icon, contentLength, folder);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderPost: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **icon** | **bool**|  | 
 **contentLength** | **int**|  | 
 **folder** | **String**| an empty value will access the catalog root | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: image/*
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFolderPut**
> catalogFolderPut(folder)



create subfolder

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

 - **Content-Type**: image/*
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogTitleFolderFilePut**
> catalogTitleFolderFilePut(folder, file, title)



set the title for the file

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 
final file = file_example; // String | 
final title = title_example; // String | 

try {
    api_instance.catalogTitleFolderFilePut(folder, file, title);
} catch (e) {
    print('Exception when calling DefaultApi->catalogTitleFolderFilePut: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **folder** | **String**|  | 
 **file** | **String**|  | 
 **title** | **String**|  | [optional] 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

