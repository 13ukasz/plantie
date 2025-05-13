idf_component_register(SRCS "zigbee_relay.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash
                             esp-zigbee-lib)
