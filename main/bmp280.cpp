#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <string.h>

#include <bmp280.h>

BMP280 bmp;

void bmp280(void *pvParameters) {
    bmp.initialize();
    while(1) {
        vTaskDelay(1);
    }
}
