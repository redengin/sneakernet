#include "WebServer.hpp"

#include <esp_log.h>

extern "C" esp_err_t redirect(httpd_req_t *req, httpd_err_code_t err);

extern "C" esp_err_t PORTAL(httpd_req_t*);
extern "C" esp_err_t STYLES_CSS(httpd_req_t*);
extern "C" esp_err_t MAIN_JS(httpd_req_t*);
extern "C" esp_err_t POLYFILLS_JS(httpd_req_t*);

WebServer::WebServer(const size_t max_sockets)
{
    // tune down logging chatter
    // TODO
    // esp_log_level_set("httpd_uri", ESP_LOG_ERROR);
    // esp_log_level_set("httpd_txrx", ESP_LOG_WARN);
    // esp_log_level_set("httpd_parse", ESP_LOG_ERROR);

    // create HTTPS server
    httpd_ssl_config_t httpsConfig = HTTPD_SSL_CONFIG_DEFAULT();
    {
        // allow wildcard uris
        httpsConfig.httpd.uri_match_fn = httpd_uri_match_wildcard;
        // provide the public key
        extern const unsigned char servercert_start[] asm("_binary_sneakernet_pem_start");
        extern const unsigned char servercert_end[]   asm("_binary_sneakernet_pem_end");
        httpsConfig.servercert = servercert_start;
        httpsConfig.servercert_len = servercert_end - servercert_start;
        // provide the private key
        extern const unsigned char prvtkey_pem_start[] asm("_binary_sneakernet_start");
        extern const unsigned char prvtkey_pem_end[]   asm("_binary_sneakernet_end");
        httpsConfig.prvtkey_pem = prvtkey_pem_start;
        httpsConfig.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;
        ESP_ERROR_CHECK(httpd_ssl_start(&httpsHandle, &httpsConfig));
        // upon 404, redirect to index
        ESP_ERROR_CHECK(httpd_register_err_handler(httpsHandle, HTTPD_404_NOT_FOUND, redirect));
    }

    // create HTTP server
    httpd_config_t httpConfig = HTTPD_DEFAULT_CONFIG();
    {
        // allow wildcard uris
        httpConfig.uri_match_fn = httpd_uri_match_wildcard;
        // maximize the availability to users
        httpConfig.max_open_sockets = max_sockets - httpsConfig.httpd.max_open_sockets;
        // increase stack size
        httpConfig.stack_size = 10 * 1024;
        ESP_ERROR_CHECK(httpd_start(&httpHandle, &httpConfig));
        // upon 404, redirect to index
        ESP_ERROR_CHECK(httpd_register_err_handler(httpHandle, HTTPD_404_NOT_FOUND, redirect));
    }

    // provide captive portal
    registerUriHandler(
        httpd_uri_t{
            .uri = "/",
            .method = HTTP_GET,
            .handler = PORTAL,
            .user_ctx = nullptr,
        }
    );
    registerUriHandler(
        httpd_uri_t{
            .uri = "/styles.css",
            .method = HTTP_GET,
            .handler = STYLES_CSS,
            .user_ctx = nullptr,
        }
    );
    registerUriHandler(
        httpd_uri_t{
            .uri = "/main.js",
            .method = HTTP_GET,
            .handler = MAIN_JS,
            .user_ctx = nullptr,
        }
    );
    registerUriHandler(
        httpd_uri_t{
            .uri = "/polyfills.js",
            .method = HTTP_GET,
            .handler = POLYFILLS_JS,
            .user_ctx = nullptr,
        }
    );
}

void WebServer::registerUriHandler(const httpd_uri_t& handler)
{
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpHandle, &handler));
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpsHandle, &handler));
}

/// provides captive portal redirect
esp_err_t redirect(httpd_req_t *request, httpd_err_code_t err)
{
    ESP_LOGV(WebServer::TAG, "Serving 302 redirect");
    // Set status
    httpd_resp_set_status(request, "302 Temporary Redirect");
    httpd_resp_set_hdr(request, "Location", "/");
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(request, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

extern "C" const char portalHtml_start[] asm("_binary_index_html_start");
extern "C" const char portalHtml_end[] asm("_binary_index_html_end");
esp_err_t PORTAL(httpd_req_t* request)
{
    auto response = request;
    ESP_ERROR_CHECK(httpd_resp_set_type(response, "text/html"));
    const size_t sz = portalHtml_end - portalHtml_start;
    return httpd_resp_send(response, portalHtml_start, sz);
}

extern "C" const char stylesCss_start[] asm("_binary_styles_css_start");
extern "C" const char stylesCss_end[] asm("_binary_styles_css_end");
esp_err_t STYLES_CSS(httpd_req_t* request)
{
    auto response = request;
    ESP_ERROR_CHECK(httpd_resp_set_type(response, "text/css"));
    const size_t sz = stylesCss_end - stylesCss_start;
    return httpd_resp_send(response, stylesCss_start, sz);
}

extern "C" const char mainJs_start[] asm("_binary_main_js_start");
extern "C" const char mainJs_end[] asm("_binary_main_js_end");
esp_err_t MAIN_JS(httpd_req_t* request)
{
    auto response = request;
    ESP_ERROR_CHECK(httpd_resp_set_type(response, "text/javascript"));
    const size_t sz = mainJs_end - mainJs_start;
    return httpd_resp_send(response, mainJs_start, sz);
}

extern "C" const char polyfillsJs_start[] asm("_binary_polyfills_js_start");
extern "C" const char polyfillsJs_end[] asm("_binary_polyfills_js_end");
esp_err_t POLYFILLS_JS(httpd_req_t* request)
{
    auto response = request;
    ESP_ERROR_CHECK(httpd_resp_set_type(response, "text/javascript"));
    const size_t sz = polyfillsJs_end - polyfillsJs_start;
    return httpd_resp_send(response, polyfillsJs_start, sz);
}

