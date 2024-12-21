idf_component_register(
    SRCS bmp280.c
    INCLUDE_DIRS .
    REQUIRES I2Cdev log esp_idf_lib_helpers
)
