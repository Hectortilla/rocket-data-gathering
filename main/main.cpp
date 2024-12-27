#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/message_buffer.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "Blink.c"

#include "parameter.h"

extern "C" {
	void app_main(void);
}

void mpu6050(void *pvParameters);
void blink_task(void *pvParameters);
void bmp280(void *pvParameters);

#ifdef __cplusplus
extern "C" {
#endif
void start_i2c(void);
#ifdef __cplusplus
}
#endif

void start_i2c(void) {
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = (gpio_num_t)CONFIG_GPIO_SDA;
	conf.scl_io_num = (gpio_num_t)CONFIG_GPIO_SCL;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 400000;
	conf.clk_flags = 0;
	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
}

void app_main(void)
{
	// Initialize i2c
	start_i2c();

	// Start imu task
	// xTaskCreate(&mpu6050, "IMU", 1024*8, NULL, 5, NULL);

	// Start blink task
	// xTaskCreate(&blink_task, "Blink", 1024*8, NULL, 5, NULL);

	// Start bmp280 task
    // xTaskCreatePinnedToCore(bmp280, "bmp280", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
	xTaskCreate(&bmp280, "Blink", 1024*8, NULL, 5, NULL);

	vTaskDelay(100);
}
