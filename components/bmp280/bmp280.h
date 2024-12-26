
#ifndef __BMP280_H__
#define __BMP280_H__

#include "I2Cdev.h"

#define BMP280_I2C_ADDRESS_0  0x76 //!< I2C address when SDO pin is low
#define BMP280_I2C_ADDRESS_1  0x77 //!< I2C address when SDO pin is high

#define BMP280_CHIP_ID  0x58 //!< BMP280 has chip-id 0x58
#define BME280_CHIP_ID  0x60 //!< BME280 has chip-id 0x60

#define BMP280_REG_ID          0xD0

class BMP280 {
    public:
        BMP280();
        void initialize();
};

#endif  // __BMP280_H__