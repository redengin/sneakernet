# openapi.api.DefaultApi

## Load the API package
```dart
import 'package:openapi/api.dart';
```

All URIs are relative to *http://192.168.4.1/api*

Method | HTTP request | Description
------------- | ------------- | -------------
[**catalogFolderDelete**](DefaultApi.md#catalogfolderdelete) | **DELETE** /catalog/{folder}/ | 
[**catalogFolderFileDelete**](DefaultApi.md#catalogfolderfiledelete) | **DELETE** /catalog/{folder}/{file} | 
[**catalogFolderFileGet**](DefaultApi.md#catalogfolderfileget) | **GET** /catalog/{folder}/{file} | 
[**catalogFolderFilePut**](DefaultApi.md#catalogfolderfileput) | **PUT** /catalog/{folder}/{file} | 
[**catalogFolderFileiconGet**](DefaultApi.md#catalogfolderfileiconget) | **GET** /catalog/{folder}/{file}?icon | 
[**catalogFolderFileiconPut**](DefaultApi.md#catalogfolderfileiconput) | **PUT** /catalog/{folder}/{file}?icon | 
[**catalogFolderFiletitletitlePut**](DefaultApi.md#catalogfolderfiletitletitleput) | **PUT** /catalog/{folder}/{file}?title={title} | 
[**catalogFolderGet**](DefaultApi.md#catalogfolderget) | **GET** /catalog/{folder}/ | 
[**catalogFolderPut**](DefaultApi.md#catalogfolderput) | **PUT** /catalog/{folder}/ | 


# **catalogFolderDelete**
> catalogFolderDelete(folder)



remove empty subfolder

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 

try {
    api_instance.catalogFolderDelete(folder);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderDelete: $e\n');
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

# **catalogFolderFileDelete**
> catalogFolderFileDelete(folder, file, contentLength)



remove a file

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 
final file = file_example; // String | 
final contentLength = 56; // int | 

try {
    api_instance.catalogFolderFileDelete(folder, file, contentLength);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderFileDelete: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **folder** | **String**|  | 
 **file** | **String**|  | 
 **contentLength** | **int**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFolderFileGet**
> catalogFolderFileGet(folder, file, contentLength)



download file

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 
final file = file_example; // String | 
final contentLength = 56; // int | 

try {
    api_instance.catalogFolderFileGet(folder, file, contentLength);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderFileGet: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **folder** | **String**|  | 
 **file** | **String**|  | 
 **contentLength** | **int**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/octet-stream

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFolderFilePut**
> catalogFolderFilePut(contentLength, xTimestamp, folder, file)



add a file

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final contentLength = 56; // int | 
final xTimestamp = xTimestamp_example; // String | 
final folder = folder_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogFolderFilePut(contentLength, xTimestamp, folder, file);
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

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFolderFileiconGet**
> catalogFolderFileiconGet(folder, file)



download icon

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogFolderFileiconGet(folder, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderFileiconGet: $e\n');
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

# **catalogFolderFileiconPut**
> catalogFolderFileiconPut(folder, file)



upload a new icon

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogFolderFileiconPut(folder, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderFileiconPut: $e\n');
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

 - **Content-Type**: application/octet-stream
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFolderFiletitletitlePut**
> catalogFolderFiletitletitlePut(title, folder, file)



set the title for the file

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final title = title_example; // String | 
final folder = folder_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogFolderFiletitletitlePut(title, folder, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderFiletitletitlePut: $e\n');
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

# **catalogFolderGet**
> Folder catalogFolderGet(folder)



get folder content

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 

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
 **folder** | **String**|  | 

### Return type

[**Folder**](Folder.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFolderPut**
> catalogFolderPut(folder)



create subfolder

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final folder = folder_example; // String | 

try {
    api_instance.catalogFolderPut(folder);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFolderPut: $e\n');
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

