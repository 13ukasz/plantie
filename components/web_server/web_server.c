#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "protocol_examples_utils.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_check.h"
#include "driver/gpio.h"

#include "temperature_sensor.h" 
#include "moisture_sensor.h"

#if !CONFIG_IDF_TARGET_LINUX
#include <esp_wifi.h>
#include <esp_system.h>
#include "nvs_flash.h"
#include "esp_eth.h"
#endif  // !CONFIG_IDF_TARGET_LINUX

#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN  (64)
#define RELAY_GPIO 4

static const char *TAG = "HTMLServer";

const char* index_html =
    "<!DOCTYPE html><html><head><meta charset='utf-8'><title>ESP32 Sensor</title></head><body>"
    "<h1>ESP32 Sensor</h1>"
    "<p>Temperature: <span id='temp'>--</span> °C</p>"
    "<p>Moisture: <span id='moist'>--</span> %</p>"
    "<p>Water pump status: <span id='pump'>OFF</span></p>"   // <-- nowe pole statusu
    "<script>"
    "setInterval(()=>{"
    "fetch('/api/data').then(r=>r.json()).then(d=>{"
    "document.getElementById('temp').textContent=d.temperature;"
    "document.getElementById('moist').textContent=d.moisture;"
    "document.getElementById('pump').textContent=d.pump;"
    "});"
    "},1000);"
    "</script></body></html>";

esp_err_t index_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_uri_t index_page = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_get_handler,
    .user_ctx  = NULL
};

esp_err_t data_get_handler(httpd_req_t *req)
{
    char json_response[256];
    snprintf(json_response, sizeof(json_response),
             "{\"temperature\": %.2f, \"moisture\": %.2f, \"pump\": \"%s\"}",
             temperature, moisture,
             water_pump_status ? "ON" : "OFF");

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

httpd_uri_t data_page = {
    .uri       = "/api/data",
    .method    = HTTP_GET,
    .handler   = data_get_handler,
    .user_ctx  = NULL
};

esp_err_t trigger_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Button pressed! Executing action...");

    gpio_set_level(RELAY_GPIO, !gpio_get_level(RELAY_GPIO));

    const char* resp = "OK";
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

httpd_uri_t trigger_uri = {
    .uri       = "/api/trigger",
    .method    = HTTP_POST,
    .handler   = trigger_post_handler,
    .user_ctx  = NULL
};

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
#if CONFIG_IDF_TARGET_LINUX
    // Setting port as 8001 when building for Linux. Port 80 can be used only by a privileged user in linux.
    // So when a unprivileged user tries to run the application, it throws bind error and the server is not started.
    // Port 8001 can be used by an unprivileged user as well. So the application will not throw bind error and the
    // server will be started.
    config.server_port = 8001;
#endif // !CONFIG_IDF_TARGET_LINUX
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &index_page);
        httpd_register_uri_handler(server, &data_page); 
        httpd_register_uri_handler(server, &trigger_uri);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

#if !CONFIG_IDF_TARGET_LINUX
static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}
#endif // !CONFIG_IDF_TARGET_LINUX

void web_server_task(void *pvParameters)
{
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /* Register event handlers to stop the server when Wi-Fi or Ethernet is disconnected,   
     * and re-start it upon connection.
     */
#if !CONFIG_IDF_TARGET_LINUX
#ifdef CONFIG_EXAMPLE_CONNECT_WIFI
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_WIFI
#ifdef CONFIG_EXAMPLE_CONNECT_ETHERNET
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_ETHERNET
#endif // !CONFIG_IDF_TARGET_LINUX

    /* Start the server for the first time */
    server = start_webserver();

    while (server) {
        sleep(5);
    }
}
