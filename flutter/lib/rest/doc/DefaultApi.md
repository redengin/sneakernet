# sneakernet_rest.api.DefaultApi

## Load the API package
```dart
import 'package:sneakernet_rest/api.dart';
```

All URIs are relative to *http://sneakernet.monster*

Method | HTTP request | Description
------------- | ------------- | -------------
[**catalogFilenameDelete**](DefaultApi.md#catalogfilenamedelete) | **DELETE** /catalog/{filename} | 
[**catalogFilenameGet**](DefaultApi.md#catalogfilenameget) | **GET** /catalog/{filename} | 
[**catalogFilenamePut**](DefaultApi.md#catalogfilenameput) | **PUT** /catalog/{filename} | 
[**catalogGet**](DefaultApi.md#catalogget) | **GET** /catalog | 


# **catalogFilenameDelete**
> catalogFilenameDelete(filename)



remove file from catalog

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final filename = filename_example; // String | 

try {
    api_instance.catalogFilenameDelete(filename);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFilenameDelete: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **filename** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: Not defined

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFilenameGet**
> catalogFilenameGet(filename)



receive the file

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final filename = filename_example; // String | 

try {
    api_instance.catalogFilenameGet(filename);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFilenameGet: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **filename** | **String**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/octet-stream

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogFilenamePut**
> catalogFilenamePut(filename, body)



add file to catalog

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();
final filename = filename_example; // String | 
final body = MultipartFile(); // MultipartFile | 

try {
    api_instance.catalogFilenamePut(filename, body);
} catch (e) {
    print('Exception when calling DefaultApi->catalogFilenamePut: $e\n');
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **filename** | **String**|  | 
 **body** | **MultipartFile**|  | [optional] 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/octet-stream
 - **Accept**: text/plain

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **catalogGet**
> List<CatalogGet200ResponseInner> catalogGet()



Returns a list of content objects

### Example
```dart
import 'package:sneakernet_rest/api.dart';

final api_instance = DefaultApi();

try {
    final result = api_instance.catalogGet();
    print(result);
} catch (e) {
    print('Exception when calling DefaultApi->catalogGet: $e\n');
}
```

### Parameters
This endpoint does not need any parameter.

### Return type

[**List<CatalogGet200ResponseInner>**](CatalogGet200ResponseInner.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

