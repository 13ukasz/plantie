idf_component_register(SRCS "web_server.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp-tls esp_http_server protocol_examples_common nvs_flash esp_netif esp_wifi esp_eth)
