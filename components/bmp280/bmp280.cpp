#include "bmp280.h"
#include <esp_log.h>

BMP280::BMP280() {
}

void BMP280::initialize() {
    ESP_LOGI("BMP280", "initialize...");

    readDeviceID();
    resetSensor();
    waitForOKStatus();
    readCalibrationData();
    configureSensor();
    controlSensor();

    ESP_LOGI("BMP280", "...initialized");
}

int8_t BMP280::read1Byte(uint8_t reg) {
	uint8_t buffer[1];
	I2Cdev::readByte(BMP280_I2C_ADDRESS_0, reg, buffer);
	return buffer[0];
}

int16_t BMP280::read2Bytes(uint8_t reg) {
	uint8_t buffer[2];
	I2Cdev::readByte(BMP280_I2C_ADDRESS_0, reg, buffer);
	return buffer[1] << 8 | buffer[0];
}

void BMP280::resetSensor() {
    I2Cdev::writeByte(BMP280_I2C_ADDRESS_0, BMP280_REG_RESET, BMP280_RESET_VALUE);
}
void BMP280::waitForOKStatus() {
    uint8_t status;
    while (1) {
        status = read1Byte(BMP280_REG_STATUS);
        if (status & 1)
            break;
    }
}
void BMP280::configureSensor() {
    uint8_t _config = (config.standby << 5) | (config.filter << 2);
    I2Cdev::writeByte(BMP280_I2C_ADDRESS_0, BMP280_REG_CONFIG, _config);
}

void BMP280::controlSensor() {
    uint8_t ctrl = (config.oversampling_temperature << 5) | (config.oversampling_pressure << 2) | (config.mode);
    I2Cdev::writeByte(BMP280_I2C_ADDRESS_0, BMP280_REG_CTRL, ctrl);
}

void BMP280::readCalibrationData() {
    calibration_data.dig_T1 = (uint16_t)read2Bytes(0x88);
    calibration_data.dig_T2 = read2Bytes(0x8a);
    calibration_data.dig_T3 = read2Bytes(0x8c);
    calibration_data.dig_P1 = (uint16_t)read2Bytes(0x8e);
    calibration_data.dig_P2 = read2Bytes(0x90);
    calibration_data.dig_P3 = read2Bytes(0x92);
    calibration_data.dig_P4 = read2Bytes(0x94);
    calibration_data.dig_P5 = read2Bytes(0x96);
    calibration_data.dig_P6 = read2Bytes(0x98);
    calibration_data.dig_P7 = read2Bytes(0x9a);
    calibration_data.dig_P8 = read2Bytes(0x9c);
    calibration_data.dig_P9 = read2Bytes(0x9e);
    
    /*
    calibration_data.dig_H1 = (uint8_t)read1Byte(0xa1);
    calibration_data.dig_H2 = read2Bytes(0xe1);
    calibration_data.dig_H3 = (uint8_t)read1Byte(0xe3);

    uint16_t h4, h5;
    h4 = (uint16_t)read2Bytes(0xe4);
    calibration_data.dig_H4 = (h4 & 0x00ff) << 4 | (h4 & 0x0f00) >> 8;
    h5 = (uint16_t)read2Bytes(0xe5);
    calibration_data.dig_H5 = h5 >> 4;
    */
}

void BMP280::readDeviceID() {
	uint8_t buffer[1];
	I2Cdev::readByte(BMP280_I2C_ADDRESS_0, BMP280_REG_ID, buffer);
	id = buffer[0];
}

BMP280SensorData BMP280::getSensorData() {
    uint8_t buffer[6];
    I2Cdev::readBytes(BMP280_I2C_ADDRESS_0, BMP280_REG_PRESS_MSB, 8, buffer, I2Cdev::readTimeout);
    int32_t _pressure = buffer[0] << 12 | buffer[1] << 4 | buffer[2] >> 4;
    int32_t _temp = buffer[3] << 12 | buffer[4] << 4 | buffer[5] >> 4;
    // int32_t _humidity = buffer[6] << 8 | buffer[7];

    int32_t fine_temp;
    _temp = compensateTemperature(_temp, &fine_temp);
    _pressure = compensatePressure(_pressure, fine_temp);
    // _humidity = compensateHumidity(_humidity, fine_temp);

    float temp = (float)_temp / 100;
    float pressure = (float)_pressure / 256;
    // float humidity = (float)_humidity / 1024;

    return {temp, pressure};
}


/**
 * Compensation algorithm is taken from BMP280 datasheet.
 *
 * Return value is in degrees Celsius.
 */
int32_t BMP280::compensateTemperature(int32_t adc_temp, int32_t *fine_temp) {
    int32_t var1, var2;

    var1 = ((((adc_temp >> 3) - ((int32_t)calibration_data.dig_T1 << 1))) * (int32_t)calibration_data.dig_T2) >> 11;
    var2 = (((((adc_temp >> 4) - (int32_t)calibration_data.dig_T1) * ((adc_temp >> 4) - (int32_t)calibration_data.dig_T1)) >> 12) * (int32_t)calibration_data.dig_T3) >> 14;

    *fine_temp = var1 + var2;
    return (*fine_temp * 5 + 128) >> 8;
}

/**
 * Compensation algorithm is taken from BMP280 datasheet.
 *
 * Return value is in Pa, 24 integer bits and 8 fractional bits.
 */
uint32_t BMP280::compensatePressure(int32_t adc_press, int32_t fine_temp) {
    int64_t var1, var2, p;

    var1 = (int64_t)fine_temp - 128000;
    var2 = var1 * var1 * (int64_t)calibration_data.dig_P6;
    var2 = var2 + ((var1 * (int64_t)calibration_data.dig_P5) << 17);
    var2 = var2 + (((int64_t)calibration_data.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)calibration_data.dig_P3) >> 8) + ((var1 * (int64_t)calibration_data.dig_P2) << 12);
    var1 = (((int64_t)1 << 47) + var1) * ((int64_t)calibration_data.dig_P1) >> 33;

    if (var1 == 0)
    {
        return 0;  // avoid exception caused by division by zero
    }

    p = 1048576 - adc_press;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = ((int64_t)calibration_data.dig_P9 * (p >> 13) * (p >> 13)) >> 25;
    var2 = ((int64_t)calibration_data.dig_P8 * p) >> 19;

    p = ((p + var1 + var2) >> 8) + ((int64_t)calibration_data.dig_P7 << 4);
    return p;
}

/**
 * Compensation algorithm is taken from BME280 datasheet.
 *
 * Return value is in Pa, 24 integer bits and 8 fractional bits.
 */
/*
uint32_t BMP280::compensateHumidity(int32_t adc_hum, int32_t fine_temp) {
    int32_t v_x1_u32r;

    v_x1_u32r = fine_temp - (int32_t)76800;
    v_x1_u32r = ((((adc_hum << 14) - ((int32_t)calibration_data.dig_H4 << 20) - ((int32_t)calibration_data.dig_H5 * v_x1_u32r)) + (int32_t)16384) >> 15)
            * (((((((v_x1_u32r * (int32_t)calibration_data.dig_H6) >> 10) * (((v_x1_u32r * (int32_t)calibration_data.dig_H3) >> 11) + (int32_t)32768)) >> 10)
                    + (int32_t)2097152) * (int32_t)calibration_data.dig_H2 + 8192) >> 14);
    v_x1_u32r = v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * (int32_t)calibration_data.dig_H1) >> 4);
    v_x1_u32r = v_x1_u32r < 0 ? 0 : v_x1_u32r;
    v_x1_u32r = v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r;
    return v_x1_u32r >> 12;
}
*/