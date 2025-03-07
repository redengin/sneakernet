#include "WebServer.hpp"

#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL

extern "C" esp_err_t http_redirect(httpd_req_t* req, httpd_err_code_t err);

extern "C" esp_err_t GENERATE_204(httpd_req_t*);
extern "C" esp_err_t PORTAL(httpd_req_t*);
extern "C" esp_err_t STYLES_CSS(httpd_req_t*);
extern "C" esp_err_t MAIN_JS(httpd_req_t*);
extern "C" esp_err_t POLYFILLS_JS(httpd_req_t*);

WebServer::WebServer(const size_t max_sockets) {
  // tune down logging chatter
  // esp_log_level_set("httpd_uri", ESP_LOG_ERROR);
  // esp_log_level_set("httpd_txrx", ESP_LOG_WARN);
  // esp_log_level_set("httpd_parse", ESP_LOG_ERROR);

  // per httpd_main.c, http uses internal sockets
  constexpr size_t HTTP_INTERNAL_SOCKET_COUNT = 3;
  size_t max_http_sockets = max_sockets - HTTP_INTERNAL_SOCKET_COUNT;

  // create HTTPS server
  httpd_ssl_config_t httpsConfig = HTTPD_SSL_CONFIG_DEFAULT();
  {
    // only support one admin at at time
    httpsConfig.httpd.max_open_sockets = 1;
    // allow wildcard uris
    httpsConfig.httpd.uri_match_fn = httpd_uri_match_wildcard;
    // provide the private key
    extern const unsigned char prvtkey_pem_start[] asm("_binary_sneakernet_https_priv_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_sneakernet_https_priv_pem_end");
    httpsConfig.prvtkey_pem = prvtkey_pem_start;
    httpsConfig.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;
    // provide the public key
    extern const unsigned char servercert_start[] asm("_binary_sneakernet_https_pub_pem_start");
    extern const unsigned char servercert_end[]   asm("_binary_sneakernet_https_pub_pem_end");
    httpsConfig.servercert = servercert_start;
    httpsConfig.servercert_len = servercert_end - servercert_start;
    ESP_ERROR_CHECK(httpd_ssl_start(&httpsHandle, &httpsConfig));
    // upon 404, redirect to index
    ESP_ERROR_CHECK(httpd_register_err_handler(httpsHandle, HTTPD_404_NOT_FOUND, http_redirect));
  }

  // create HTTP server
  httpd_config_t httpConfig = HTTPD_DEFAULT_CONFIG();
  {
    // allow wildcard uris
    httpConfig.uri_match_fn = httpd_uri_match_wildcard;
    // maximize the availability to users
    httpConfig.max_open_sockets =
        max_http_sockets - httpsConfig.httpd.max_open_sockets;
    // increase stack size
    httpConfig.stack_size = 10 * 1024;
    ESP_ERROR_CHECK(httpd_start(&httpHandle, &httpConfig));
  }

  // provide captive portal
  {
    // lie that this connection has internet connectivity
    constexpr httpd_uri_t _204 = {
        .uri = "/generate_204",
        .method = HTTP_GET,
        .handler = GENERATE_204,
        .user_ctx = nullptr,
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpHandle, &_204));

    /// provide 404 redirect to support captive portal
    ESP_ERROR_CHECK(httpd_register_err_handler(httpHandle, HTTPD_404_NOT_FOUND,
                                               http_redirect));
  }

  registerUriHandler(httpd_uri_t{
      .uri = "/app/",
      .method = HTTP_GET,
      .handler = PORTAL,
      .user_ctx = nullptr,
  });
  registerUriHandler(httpd_uri_t{
      .uri = "/app/styles.css",
      .method = HTTP_GET,
      .handler = STYLES_CSS,
      .user_ctx = nullptr,
  });
  registerUriHandler(httpd_uri_t{
      .uri = "/app/main.js",
      .method = HTTP_GET,
      .handler = MAIN_JS,
      .user_ctx = nullptr,
  });
  registerUriHandler(httpd_uri_t{
      .uri = "/app/polyfills.js",
      .method = HTTP_GET,
      .handler = POLYFILLS_JS,
      .user_ctx = nullptr,
  });
}

void WebServer::registerUriHandler(const httpd_uri_t& handler) {
  ESP_ERROR_CHECK(httpd_register_uri_handler(httpHandle, &handler));
  ESP_ERROR_CHECK(httpd_register_uri_handler(httpsHandle, &handler));
}

/// signal that this has internet connectivity
esp_err_t GENERATE_204(httpd_req_t* request) {
  return httpd_resp_send_custom_err(request, "204 No Content", "");
}

/// provides captive portal redirect
esp_err_t http_redirect(httpd_req_t* request, httpd_err_code_t err) {
  ESP_LOGI(WebServer::TAG, "Serving 303 redirect for request[%s]", request->uri);
  // Set status
  httpd_resp_set_status(request, "303 See Other");
  // Redirect to the "/" root directory
  httpd_resp_set_hdr(request, "Location", "http://192.168.4.1/app/");
  // iOS requires content in the response to detect a captive portal, simply
  // redirecting is not sufficient.
  httpd_resp_send(request, "Redirect to the captive portal",
                  HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

extern "C" const char portalHtml_start[] asm("_binary_index_html_start");
extern "C" const char portalHtml_end[] asm("_binary_index_html_end");
esp_err_t PORTAL(httpd_req_t* request) {
  auto response = request;
  ESP_ERROR_CHECK(httpd_resp_set_type(response, "text/html"));
  const size_t sz = portalHtml_end - portalHtml_start;
  return httpd_resp_send(response, portalHtml_start, sz);
}

extern "C" const char stylesCss_start[] asm("_binary_styles_css_start");
extern "C" const char stylesCss_end[] asm("_binary_styles_css_end");
esp_err_t STYLES_CSS(httpd_req_t* request) {
  auto response = request;
  ESP_ERROR_CHECK(httpd_resp_set_type(response, "text/css"));
  const size_t sz = stylesCss_end - stylesCss_start;
  return httpd_resp_send(response, stylesCss_start, sz);
}

extern "C" const char mainJs_start[] asm("_binary_main_js_start");
extern "C" const char mainJs_end[] asm("_binary_main_js_end");
esp_err_t MAIN_JS(httpd_req_t* request) {
  auto response = request;
  ESP_ERROR_CHECK(httpd_resp_set_type(response, "text/javascript"));
  const size_t sz = mainJs_end - mainJs_start;
  return httpd_resp_send(response, mainJs_start, sz);
}

extern "C" const char polyfillsJs_start[] asm("_binary_polyfills_js_start");
extern "C" const char polyfillsJs_end[] asm("_binary_polyfills_js_end");
esp_err_t POLYFILLS_JS(httpd_req_t* request) {
  auto response = request;
  ESP_ERROR_CHECK(httpd_resp_set_type(response, "text/javascript"));
  const size_t sz = polyfillsJs_end - polyfillsJs_start;
  return httpd_resp_send(response, polyfillsJs_start, sz);
}
