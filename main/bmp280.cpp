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
	ESP_LOGI(TAG, "getDeviceID=0x%x", bmp.id);

    BMP280SensorData sensorData;
	while(1) {
		sensorData = bmp.getSensorData();
        printf("Pressure: %.2f Pa, Temperature: %.2f C\n", sensorData.pressure, sensorData.temp);

		vTaskDelay(100/portTICK_PERIOD_MS);
	}
}
