#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include "esp_log.h"
#include <string.h>

#include <bmp280.h>

BMP280 bmp;

static const char *TAG = "BARO";

void bmp280(void *pvParameters) {
    bmp.initialize();

	// Get Device ID
	uint8_t buffer[1];
	I2Cdev::readByte(BMP280_I2C_ADDRESS_0, BMP280_REG_ID, buffer);
	ESP_LOGI(TAG, "getDeviceID=0x%x", buffer[0]);

    while(1) {
        vTaskDelay(1);
    }
}
