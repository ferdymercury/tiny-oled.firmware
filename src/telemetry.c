/****************************************************************************
    tiny-oled.firmware - A project to push the limits of my abilities and
    understanding of embedded firmware development.
    Copyright (C) 2020 Stephen Murphy - github.com/stephendpmurphy

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
****************************************************************************/

#include <stdio.h>
#include <util/delay.h>
#include "telemetry.h"
#include "icm20948_api.h"
#include "spi.h"
#include "pins.h"

/*! @brief ICM20948 captured gyro data */
icm20948_gyro_t gyro_data;
/*! @brief ICM20948 captured accel data */
icm20948_accel_t accel_data;

/*!
 * @brief User provided API for writing data via SPI
 *
 * @param[in] addr: The register address which you would like to write to
 * @param[in] *buf : Pointer to a buffer of data to be written
 * @param[in] len : Length of data to be written.
 *
 * @return Returns the state of the SPI write
 */
static int8_t usr_write(const uint8_t addr, const uint8_t *data, const uint32_t len) {

    // Check the input parameters
    if( data == NULL )
    {
        return ICM20948_RET_NULL_PTR;
    }

    // Assert CS
    spi_assertCS(&SPI_ICM20948_CS_PORT, SPI_ICM20948_CS_PIN, 0);

    // Transmit the address
    spi_write(&addr, 0x01);
    // Transmit the data buffer
    spi_write((uint8_t *)data, len);

    // De-assert CS
    spi_assertCS(&SPI_ICM20948_CS_PORT, SPI_ICM20948_CS_PIN, 1);

    return ICM20948_RET_OK;
}

/*!
 * @brief User provided API for reading data via SPI
 *
 * @param[in] addr: The register address which you would like to read from
 * @param[in] *buf : Pointer to a buffer where read data should be placed
 * @param[in] len : Length of data to be read
 *
 * @return Returns the state of the SPI read
 */
static int8_t usr_read(const uint8_t addr, uint8_t *data, const uint32_t len) {
    // Check the input parameters
    if( data == NULL )
    {
        return ICM20948_RET_NULL_PTR;
    }

    // Assert CS
    spi_assertCS(&SPI_ICM20948_CS_PORT, SPI_ICM20948_CS_PIN, 0);

    // Transmit the address
    spi_write(&addr, 0x01);
    // Read the data back
    spi_read(data, len);

    // De-assert CS
    spi_assertCS(&SPI_ICM20948_CS_PORT, SPI_ICM20948_CS_PIN, 1);

    return ICM20948_RET_OK;
}

/*!
 * @brief User provided API for delaying in microseconds
 *
 * @param[in] period: The duration in which we should delay in microseconds
 *
 * @return Returns the state of the SPI write
 */
static void usr_delay_us(uint32_t period) {
    while( period > 0) {
        _delay_us(1);
        period--;
    }
}

/*!
 * @brief This API initializes the telemetry module
 */
int8_t telemetry_init(void) {
    icm20948_return_code_t ret = ICM20948_RET_OK;
    icm20948_settings_t settings;

    ret = icm20948_init(usr_read, usr_write, usr_delay_us);

    if( ret == ICM20948_RET_OK ) {
        settings.gyro_en = ICM20948_GYRO_ENABLE;
        settings.accel_en = ICM20948_ACCEL_ENABLE;
        ret = icm20948_applySettings(&settings);
    }

    return ret;
}

/*!
 * @brief This API retrieves a sample of data from the telemetry module
 */
int8_t telemetry_getData(void) {
    icm20948_return_code_t ret = ICM20948_RET_OK;

    ret |= icm20948_getGyroData(&gyro_data);
    ret |= icm20948_getAccelData(&accel_data);

    return ret;
}