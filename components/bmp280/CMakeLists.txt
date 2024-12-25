idf_component_register(
    SRCS bmp280.cpp
    INCLUDE_DIRS .
    REQUIRES driver I2Cdev log esp_idf_lib_helpers
)
