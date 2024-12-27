
#ifndef __BMP280_H__
#define __BMP280_H__

#include "I2Cdev.h"

#define BMP280_I2C_ADDRESS_0    0x76 //!< I2C address when SDO pin is low
#define BMP280_I2C_ADDRESS_1    0x77 //!< I2C address when SDO pin is high

#define BMP280_CHIP_ID          0x58 //!< BMP280 has chip-id 0x58
#define BME280_CHIP_ID          0x60 //!< BME280 has chip-id 0x60

#define BMP280_REG_ID           0xD0

#define BMP280_REG_PRESS_MSB    0xF7

#define BMP280_REG_RESET        0xE0
#define BMP280_RESET_VALUE      0xB6
#define BMP280_REG_STATUS       0xF3 /* bits: 3 measuring; 0 im_update */

#define BMP280_REG_CONFIG       0xF5 /* bits: 7-5 t_sb; 4-2 filter; 0 spi3w_en */
#define BMP280_REG_CTRL         0xF4 /* bits: 7-5 osrs_t; 4-2 osrs_p; 1-0 mode */

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
    // float humidity;
};


/**
 * Mode of BMP280 module operation.
 */
enum BMP280Mode {
    BMP280_MODE_SLEEP = 0,  //!< Sleep mode
    BMP280_MODE_FORCED = 1, //!< Measurement is initiated by user
    BMP280_MODE_NORMAL = 3  //!< Continues measurement
};

enum BMP280Filter {
    BMP280_FILTER_OFF = 0,
    BMP280_FILTER_2 = 1,
    BMP280_FILTER_4 = 2,
    BMP280_FILTER_8 = 3,
    BMP280_FILTER_16 = 4
};

/**
 * Pressure oversampling settings
 */
enum BMP280Oversampling {
    BMP280_SKIPPED = 0,          //!< no measurement
    BMP280_ULTRA_LOW_POWER = 1,  //!< oversampling x1
    BMP280_LOW_POWER = 2,        //!< oversampling x2
    BMP280_STANDARD = 3,         //!< oversampling x4
    BMP280_HIGH_RES = 4,         //!< oversampling x8
    BMP280_ULTRA_HIGH_RES = 5    //!< oversampling x16
};

/**
 * Stand by time between measurements in normal mode
 */
enum BMP280StandbyTime {
    BMP280_STANDBY_05 = 0,      //!< stand by time 0.5ms
    BMP280_STANDBY_62 = 1,      //!< stand by time 62.5ms
    BMP280_STANDBY_125 = 2,     //!< stand by time 125ms
    BMP280_STANDBY_250 = 3,     //!< stand by time 250ms
    BMP280_STANDBY_500 = 4,     //!< stand by time 500ms
    BMP280_STANDBY_1000 = 5,    //!< stand by time 1s
    BMP280_STANDBY_2000 = 6,    //!< stand by time 2s BMP280, 10ms BME280
    BMP280_STANDBY_4000 = 7,    //!< stand by time 4s BMP280, 20ms BME280
};

struct BMP280Config {
    BMP280Mode mode = BMP280_MODE_NORMAL;
    BMP280Filter filter = BMP280_FILTER_OFF;
    BMP280Oversampling oversampling_pressure = BMP280_STANDARD;
    BMP280Oversampling oversampling_temperature = BMP280_STANDARD;
    BMP280Oversampling oversampling_humidity = BMP280_STANDARD;
    BMP280StandbyTime standby = BMP280_STANDBY_250;
};

class BMP280 {
    public:
        uint8_t id;
        BMP280Config config;
        CalibrationData calibration_data;

        BMP280();
        void initialize();
        void readCalibrationData();
        void resetSensor();
        void waitForOKStatus();
        void configureSensor();
        void controlSensor();

        int32_t compensateTemperature(int32_t adc_temp, int32_t *fine_temp);
        uint32_t compensatePressure(int32_t adc_press, int32_t fine_temp);
        // uint32_t compensateHumidity(int32_t adc_hum, int32_t fine_temp);
        BMP280SensorData getSensorData();

        int8_t read1Byte(uint8_t reg);
        int16_t read2Bytes(uint8_t reg);

    private:
        void readDeviceID();
};

#endif  // __BMP280_H__