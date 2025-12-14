idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES max6675 esp_driver_gpio
)
