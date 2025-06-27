# openapi.api.DefaultApi

## Load the API package
```dart
import 'package:openapi/api.dart';
```

All URIs are relative to *http://192.168.4.1/api*

Method | HTTP request | Description
------------- | ------------- | -------------
[**catalogPathDelete**](DefaultApi.md#catalogpathdelete) | **DELETE** /catalog/{path}/ | 
[**catalogPathFileDelete**](DefaultApi.md#catalogpathfiledelete) | **DELETE** /catalog/{path}/{file} | 
[**catalogPathFileGet**](DefaultApi.md#catalogpathfileget) | **GET** /catalog/{path}/{file} | 
[**catalogPathFilePut**](DefaultApi.md#catalogpathfileput) | **PUT** /catalog/{path}/{file} | 
[**catalogPathFileiconGet**](DefaultApi.md#catalogpathfileiconget) | **GET** /catalog/{path}/{file}?icon | 
[**catalogPathFileiconPut**](DefaultApi.md#catalogpathfileiconput) | **PUT** /catalog/{path}/{file}?icon | 
[**catalogPathFiletitletitlePut**](DefaultApi.md#catalogpathfiletitletitleput) | **PUT** /catalog/{path}/{file}?title={title} | 
[**catalogPathGet**](DefaultApi.md#catalogpathget) | **GET** /catalog/{path}/ | 
[**catalogPathPut**](DefaultApi.md#catalogpathput) | **PUT** /catalog/{path}/ | 


# **catalogPathDelete**
> catalogPathDelete(path)



remove empty subfolder

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final path = path_example; // String | 

try {
    api_instance.catalogPathDelete(path);
} catch (e) {
    print('Exception when calling DefaultApi->catalogPathDelete: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **path** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogPathFileDelete**
> catalogPathFileDelete(path, file, contentLength)



remove a file

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final path = path_example; // String | 
final file = file_example; // String | 
final contentLength = 56; // int | 

try {
    api_instance.catalogPathFileDelete(path, file, contentLength);
} catch (e) {
    print('Exception when calling DefaultApi->catalogPathFileDelete: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **path** | **String**|  | 
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

# **catalogPathFileGet**
> catalogPathFileGet(path, file, contentLength)



download file

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final path = path_example; // String | 
final file = file_example; // String | 
final contentLength = 56; // int | 

try {
    api_instance.catalogPathFileGet(path, file, contentLength);
} catch (e) {
    print('Exception when calling DefaultApi->catalogPathFileGet: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **path** | **String**|  | 
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

# **catalogPathFilePut**
> catalogPathFilePut(contentLength, xTimestamp, path, file)



add a file

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final contentLength = 56; // int | 
final xTimestamp = xTimestamp_example; // String | 
final path = path_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogPathFilePut(contentLength, xTimestamp, path, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogPathFilePut: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **contentLength** | **int**|  | 
 **xTimestamp** | **String**|  | 
 **path** | **String**|  | 
 **file** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogPathFileiconGet**
> catalogPathFileiconGet(path, file)



download icon

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final path = path_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogPathFileiconGet(path, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogPathFileiconGet: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **path** | **String**|  | 
 **file** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/octet-stream

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogPathFileiconPut**
> catalogPathFileiconPut(path, file)



upload a new icon

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final path = path_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogPathFileiconPut(path, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogPathFileiconPut: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **path** | **String**|  | 
 **file** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/octet-stream
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogPathFiletitletitlePut**
> catalogPathFiletitletitlePut(title, path, file)



set the title for the file

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final title = title_example; // String | 
final path = path_example; // String | 
final file = file_example; // String | 

try {
    api_instance.catalogPathFiletitletitlePut(title, path, file);
} catch (e) {
    print('Exception when calling DefaultApi->catalogPathFiletitletitlePut: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **title** | **String**|  | 
 **path** | **String**|  | 
 **file** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogPathGet**
> Folder catalogPathGet(path)



get folder content

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final path = path_example; // String | 

try {
    final result = api_instance.catalogPathGet(path);
    print(result);
} catch (e) {
    print('Exception when calling DefaultApi->catalogPathGet: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **path** | **String**|  | 

### Return type

[**Folder**](Folder.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogPathPut**
> catalogPathPut(path)



create subfolder

### Example
```dart
import 'package:openapi/api.dart';

final api_instance = DefaultApi();
final path = path_example; // String | 

try {
    api_instance.catalogPathPut(path);
} catch (e) {
    print('Exception when calling DefaultApi->catalogPathPut: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **path** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

