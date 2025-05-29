#include "WebServer.hpp"

#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL

#include <string>

extern "C" esp_err_t redirect(httpd_req_t *req, httpd_err_code_t err);

extern "C" esp_err_t CAPPORT(httpd_req_t *);
extern "C" esp_err_t CONNECT_INSTRUCTIONS(httpd_req_t *);

extern "C" esp_err_t WEBAPP(httpd_req_t *);
extern "C" esp_err_t STYLES_CSS(httpd_req_t *);
extern "C" esp_err_t MAIN_JS(httpd_req_t *);
extern "C" esp_err_t POLYFILLS_JS(httpd_req_t *);



WebServer::WebServer(const size_t max_sockets)
{
  // tune down logging chatter
  esp_log_level_set("esp-tls-mbedtls", ESP_LOG_NONE);   // hides TLS errors
  esp_log_level_set("esp_https_server", ESP_LOG_NONE);  // hides session failed errors
  esp_log_level_set("httpd", ESP_LOG_NONE);             // hides session failed errors
  esp_log_level_set("httpd_txrx", ESP_LOG_NONE);        // hides 404 warnings
  esp_log_level_set("httpd_parse", ESP_LOG_NONE);       // hides parsing messages

  // per httpd_main.c, http uses internal sockets
  constexpr size_t HTTP_INTERNAL_SOCKET_COUNT = 3;
  size_t max_http_sockets = max_sockets - HTTP_INTERNAL_SOCKET_COUNT;

  // create HTTPS server
  httpd_ssl_config_t httpsConfig = HTTPD_SSL_CONFIG_DEFAULT();
  // purge oldest connection
  httpsConfig.httpd.lru_purge_enable = true;
  // only support one admin at at time
  httpsConfig.httpd.max_open_sockets = 1;
  // allow wildcard uris
  httpsConfig.httpd.uri_match_fn = httpd_uri_match_wildcard;
  // provide handlers
  httpsConfig.httpd.max_uri_handlers = MAX_HTTPS_URI_HANDLERS;
  // provide the private key
  extern const unsigned char prvtkey_pem_start[] asm("_binary_sneakernet_https_priv_pem_start");
  extern const unsigned char prvtkey_pem_end[] asm("_binary_sneakernet_https_priv_pem_end");
  httpsConfig.prvtkey_pem = prvtkey_pem_start;
  httpsConfig.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;
  // provide the public key
  extern const unsigned char servercert_start[] asm("_binary_sneakernet_https_pub_pem_start");
  extern const unsigned char servercert_end[] asm("_binary_sneakernet_https_pub_pem_end");
  httpsConfig.servercert = servercert_start;
  httpsConfig.servercert_len = servercert_end - servercert_start;
  // start the https server
  ESP_ERROR_CHECK(httpd_ssl_start(&httpsHandle, &httpsConfig));

  // redirect non-sneakernet https traffic to web-app
  httpd_register_err_handler(httpsHandle, HTTPD_404_NOT_FOUND, redirect);

  // create HTTP server
  httpd_config_t httpConfig = HTTPD_DEFAULT_CONFIG();
  // purge oldest connection
  httpConfig.lru_purge_enable = true;
  // provide handlers
  httpConfig.max_uri_handlers = MAX_HTTP_URI_HANDLERS;
  // allow wildcard uris
  httpConfig.uri_match_fn = httpd_uri_match_wildcard;
  // maximize the availability to users
  httpConfig.max_open_sockets =
      max_http_sockets - httpsConfig.httpd.max_open_sockets;
  // increase stack size
  httpConfig.stack_size = 10 * 1024;
  // start the http server
  ESP_ERROR_CHECK(httpd_start(&httpHandle, &httpConfig));

  // capport support (https://datatracker.ietf.org/doc/html/rfc8908)
  {
    const httpd_uri_t handler = {
        .uri = CAPPORT_URI,
        .method = HTTP_GET,
        .handler = CAPPORT,
        .user_ctx = nullptr,
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpsHandle, &handler));
  }
  // Android captive portal support
  {
    httpd_uri_t handler = {
        .uri = "",
        .method = HTTP_GET,
        .handler = CONNECT_INSTRUCTIONS, // provide connecting instructions
        .user_ctx = nullptr,
    };
    handler.uri = "/gen_204";
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpHandle, &handler));
    handler.uri = "/generate_204";
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpHandle, &handler));
  }
  // iOS captive portal support
  {
    const httpd_uri_t handler = {
        .uri = "/hotspot-detect.html",
        .method = HTTP_GET,
        .handler = WEBAPP,  // iOS is able to use the web-app directly
        .user_ctx = nullptr,
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpHandle, &handler));
  }

  // register the web-app
  registerUriHandler(httpd_uri_t{
      .uri = "/",
      .method = HTTP_GET,
      .handler = WEBAPP,
      .user_ctx = nullptr,
  });
  registerUriHandler(httpd_uri_t{
      .uri = "/styles.css",
      .method = HTTP_GET,
      .handler = STYLES_CSS,
      .user_ctx = nullptr,
  });
  registerUriHandler(httpd_uri_t{
      .uri = "/main.js",
      .method = HTTP_GET,
      .handler = MAIN_JS,
      .user_ctx = nullptr,
  });
  registerUriHandler(httpd_uri_t{
      .uri = "/polyfills.js",
      .method = HTTP_GET,
      .handler = POLYFILLS_JS,
      .user_ctx = nullptr,
  });
}

void WebServer::registerUriHandler(const httpd_uri_t &handler)
{
  ESP_ERROR_CHECK(httpd_register_uri_handler(httpHandle, &handler));
  ESP_ERROR_CHECK(httpd_register_uri_handler(httpsHandle, &handler));
}

/// provides captive portal redirect to webapp
esp_err_t redirect(httpd_req_t *request, httpd_err_code_t err)
{
  ESP_LOGD(WebServer::TAG, "Serving 303 redirect for request[%s]", request->uri);
  httpd_resp_set_status(request, "303 See Other");
  httpd_resp_set_hdr(request, "Location", "http://192.168.4.1/");
  // iOS requires content in the response to detect a captive portal
  return httpd_resp_send(request, "Redirect to the captive portal",
                  HTTPD_RESP_USE_STRLEN);
}

/// @brief send capport json
esp_err_t CAPPORT(httpd_req_t *request)
{
  ESP_LOGD(WebServer::TAG, "got a capport query");
  constexpr char capport_json[] = R"END(
{
  "captive": false,
  "venue-info-url": "http://192.168.4.1/",
}
)END";
  auto response = request;
  httpd_resp_set_type(response, "application/captive+json");
  return httpd_resp_send(request, capport_json, HTTPD_RESP_USE_STRLEN);
}

extern "C" const char captiveportalHtml_start[] asm("_binary_captiveportal_html_start");
extern "C" const char captiveportalHtml_end[] asm("_binary_captiveportal_html_end");
esp_err_t CONNECT_INSTRUCTIONS(httpd_req_t *request)
{
  auto response = request;
  // limit caching to 15 minutes (15 * 60) = 900
  httpd_resp_set_hdr(response, "Cache-Control", "max-age=900");
  // send the data
  httpd_resp_set_type(response, "text/html");
  const size_t sz = captiveportalHtml_end - captiveportalHtml_start;
  return httpd_resp_send(response, captiveportalHtml_start, sz);
}

extern "C" const char webappHtml_start[] asm("_binary_index_html_start");
extern "C" const char webappHtml_end[] asm("_binary_index_html_end");
esp_err_t WEBAPP(httpd_req_t *request)
{
  auto response = request;
  // limit caching to 15 minutes (15 * 60) = 900
  httpd_resp_set_hdr(response, "Cache-Control", "max-age=900");
  // send the data
  httpd_resp_set_type(response, "text/html");
  const size_t sz = webappHtml_end - webappHtml_start;
  return httpd_resp_send(response, webappHtml_start, sz);
}

extern "C" const char stylesCss_start[] asm("_binary_styles_css_start");
extern "C" const char stylesCss_end[] asm("_binary_styles_css_end");
esp_err_t STYLES_CSS(httpd_req_t *request)
{
  auto response = request;
  // limit caching to 15 minutes (15 * 60) = 900
  httpd_resp_set_hdr(response, "Cache-Control", "max-age=900");
  // send the data
  httpd_resp_set_type(response, "text/css");
  const size_t sz = stylesCss_end - stylesCss_start;
  return httpd_resp_send(response, stylesCss_start, sz);
}

extern "C" const char mainJs_start[] asm("_binary_main_js_start");
extern "C" const char mainJs_end[] asm("_binary_main_js_end");
esp_err_t MAIN_JS(httpd_req_t *request)
{
  auto response = request;
  // limit caching to 15 minutes (15 * 60) = 900
  httpd_resp_set_hdr(response, "Cache-Control", "max-age=900");
  // send the data
  httpd_resp_set_type(response, "text/javascript");
  const size_t sz = mainJs_end - mainJs_start;
  return httpd_resp_send(response, mainJs_start, sz);
}

extern "C" const char polyfillsJs_start[] asm("_binary_polyfills_js_start");
extern "C" const char polyfillsJs_end[] asm("_binary_polyfills_js_end");
esp_err_t POLYFILLS_JS(httpd_req_t *request)
{
  auto response = request;
  // limit caching to 15 minutes (15 * 60) = 900
  httpd_resp_set_hdr(response, "Cache-Control", "max-age=900");
  // send the data
  httpd_resp_set_type(response, "text/javascript");
  const size_t sz = polyfillsJs_end - polyfillsJs_start;
  return httpd_resp_send(response, polyfillsJs_start, sz);
}
