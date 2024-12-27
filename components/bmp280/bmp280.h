
#ifndef __BMP280_H__
#define __BMP280_H__

#include "I2Cdev.h"

#define BMP280_I2C_ADDRESS_0  0x76 //!< I2C address when SDO pin is low
#define BMP280_I2C_ADDRESS_1  0x77 //!< I2C address when SDO pin is high

#define BMP280_CHIP_ID  0x58 //!< BMP280 has chip-id 0x58
#define BME280_CHIP_ID  0x60 //!< BME280 has chip-id 0x60

#define BMP280_REG_ID          0xD0

#define BMP280_REG_PRESS_MSB   0xF7

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    // Pressure compensation for BME280
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

    // Humidity compensation for BME280
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} CalibrationData;


struct BMP280SensorData {
    float temp;
    float pressure;
    float humidity;
};

class BMP280 {
    public:
        uint8_t id;
        CalibrationData calibration_data;

        BMP280();
        void initialize();
        void readCalibrationData();
        int32_t compensateTemperature(int32_t adc_temp, int32_t *fine_temp);
        uint32_t compensatePressure(int32_t adc_press, int32_t fine_temp);
        uint32_t compensateHumidity(int32_t adc_hum, int32_t fine_temp);
        BMP280SensorData getSensorData();

        int8_t read1Byte(uint8_t reg);
        int16_t read2Bytes(uint8_t reg);

    private:
        void readDeviceID();
};

#endif  // __BMP280_H__