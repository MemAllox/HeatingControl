/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/07/hal-library-06-ds18b20-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   Library for interfacing DS18B20 temperature sensor from Dallas semiconductors.
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef TM_DS18B20_H
#define TM_DS18B20_H 100

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_DS12820
 * @brief    Library for interfacing DS18B20 temperature sensor from Dallas semiconductors - http://stm32f4-discovery.net/2015/07/hal-library-06-ds18b20-for-stm32fxxx/
 * @{
 *
 * With this you can read temperature, set and get temperature resolution from 9 to 12 bits and check if device is DS18B20.
 * 
 * Pin for STM32Fxxx is the same as set with @ref TM_ONEWIRE library.
 *
 * \par Changelog
 *
\verbatim
 Version 1.0
  - First release
\endverbatim
 *
 * \par Dependencies
 *
\verbatim
 - STM32Fxxx HAL
 - TM ONEWIRE
 - TM GPIO
 - defines.h
\endverbatim

<b>Minimal Example</b>:
char buf[6];
TM_OneWire_t ow;
float temp1 = 0;
TM_OneWire_Init(&ow, GPIOB, GPIO_PIN_4);
while (1) {
	TM_OneWire_First(&ow);
	TM_DS18S20_Start(&ow, ow.ROM_NO);
	while (!TM_DS18B20_AllDone(&ow))
		;
	if (TM_DS18S20_Read(&ow, ow.ROM_NO, &temp1) == TM_DS18B20_SUCCESS) {
		sprintf(buf, "%2.1f\n", temp1);
		HAL_USART_Transmit(&husart1, (uint8_t*) buf, 6, 1000);
	} else {
		//error management
	}

}
 */

#include <string.h>
#include "stm32f3xx_hal.h"
#include "tm_stm32_onewire.h"

/* OneWire version check */
#if TM_ONEWIRE_H < 100
#error "Please update TM ONEWIRE LIB, minimum required version is 2.0.0. Download available on stm32f4-discovery.net website"
#endif

/**
 * @defgroup TM_DS18B20_Macros
 * @brief    Library defines
 * @{
 */

/* Every onewire chip has different ROM code, but all the same chips has same family code */
/* in case of DS18B20 this is 0x28 and this is first byte of ROM address */
#define DS18S20_FAMILY_CODE				0x10
#define DS18B20_FAMILY_CODE				0x28
#define DS18B20_CMD_ALARMSEARCH			0xEC

/* DS18B20 read temperature command */
#define DS18B20_CMD_CONVERTTEMP			0x44 	/* Convert temperature */
#define DS18B20_DECIMAL_STEPS_12BIT		0.0625
#define DS18B20_DECIMAL_STEPS_11BIT		0.125
#define DS18B20_DECIMAL_STEPS_10BIT		0.25
#define DS18B20_DECIMAL_STEPS_9BIT		0.5

/* Bits locations for resolution */
#define DS18B20_RESOLUTION_R1			6
#define DS18B20_RESOLUTION_R0			5

/* CRC enabled (not implemented) */
//#ifdef DS18B20_USE_CRC
//#define DS18B20_DATA_LEN				9
//#else
//#define DS18B20_DATA_LEN				2
//#endif

#define TM_DS18B20_SUCCESS						1
#define TM_DS18B20_ERR_CRC_INVALID				2
#define TM_DS18B20_ERR_WRONG_DEVICE_FAMILY		4
#define TM_DS18B20_ERR_BUSY_CONVERTING			6
#define TM_DS18B20_ERR_NO_CONVERSION_YET		8


#define TM_DS18B20_DATA0_DEFAULT				0xAA
#define TM_DS18B20_DATA1_DEFAULT				0x00


/**
 * @}
 */

/**
 * @defgroup TM_DS18B20_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  DS18B20 Resolutions available (DS18B20 EXCLUSIVE!)
 */
typedef enum {
	TM_DS18B20_Resolution_9bits = 9,   /*!< DS18B20 9 bits resolution */
	TM_DS18B20_Resolution_10bits = 10, /*!< DS18B20 10 bits resolution */
	TM_DS18B20_Resolution_11bits = 11, /*!< DS18B20 11 bits resolution */
	TM_DS18B20_Resolution_12bits = 12  /*!< DS18B20 12 bits resolution */
} TM_DS18B20_Resolution_t;

/**
 * @}
 */

/**
 * @defgroup TM_DS18B20_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Starts temperature conversion for specific DS18S20 on specific onewire channel
 *         Does not check if there is device a device listening!
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12S80 device.
 *         Entire ROM address is 8-bytes long
 * @retval TM_DS18B20_SUCCESS if device is DS18S20 or TM_DS18B20_ERR_WRONG_DEVICE_FAMILY if not
 */
uint8_t TM_DS18S20_Start(TM_OneWire_t* OneWireStruct, uint8_t* ROM);

/**
 * @brief  Starts temperature conversion for specific DS18B20 on specific onewire channel
 *         Does not check if there is device a device listening!
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @retval TM_DS18B20_SUCCESS if device is DS18B20 or TM_DS18B20_ERR_WRONG_DEVICE_FAMILY if not
 */
uint8_t TM_DS18B20_Start(TM_OneWire_t* OneWireStruct, uint8_t* ROM);

/**
 * @brief  Starts temperature conversion for all DS18B20 devices on specific onewire channel
 * @note   This mode will skip ROM addressing
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @retval None
 */
void TM_DS18B20_StartAll(TM_OneWire_t* OneWireStruct);


/**
 * @brief  Checks if the DS18S20 is still busy with converting the temperature
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @retval Temperature status:
 *            - 1: Device is busy
 *            - 0: Device is not busy
 */
uint8_t TM_DS18S20_IsBusy(TM_OneWire_t* OneWire);

/**
 * @brief  Checks if the DS18B20 is still busy with converting the temperature
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @retval Temperature status:
 *            - 1: Device is busy
 *            - 0: Device is not busy
 */
uint8_t TM_DS18B20_IsBusy(TM_OneWire_t* OneWire);

/**
 * @brief  Reads temperature from DS18S20
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12S80 device.
 *         Entire ROM address is 8-bytes long
 * @param  *destination: Pointer to float variable to store temperature
 * @retval Temperature status:
 *            - TM_DS18B20_ERR_WRONG_DEVICE_FAMILY: Device is not DS18S20
 *            - TM_DS18B20_ERR_BUSY_CONVERTING: conversion is not done yet
 *            - TM_DS18B20_ERR_CRC_INVALID: CRC failed
 *			  - TM_DS18B20_ERR_NO_CONVERSION_YET: there was no conversion requested yet, -> TM_DS18S20_Start
 *            - TM_DS18B20_SUCCESS: Temperature is read OK
 */
uint8_t TM_DS18S20_Read(TM_OneWire_t* OneWireStruct, uint8_t* ROM, float* destination);

/**
 * @brief  Reads temperature from DS18B20
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @param  *destination: Pointer to float variable to store temperature
 * @retval Temperature status:
 *            - TM_DS18B20_ERR_WRONG_DEVICE_FAMILY: Device is not DS18S20
 *            - TM_DS18B20_ERR_BUSY_CONVERTING: conversion is not done yet
 *            - TM_DS18B20_ERR_CRC_INVALID: CRC failed
 *			  - TM_DS18B20_ERR_NO_CONVERSION_YET: there was no conversion requested yet, -> TM_DS18S20_Start
 *            - TM_DS18B20_SUCCESS: Temperature is read OK
 */
uint8_t TM_DS18B20_Read(TM_OneWire_t* OneWireStruct, uint8_t* ROM, float* destination);

/**
 * @brief  Gets resolution for temperature conversion from DS18B20 device (DS18B20 EXCLUSIVE!)
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @retval Resolution:
 *            - TM_DS18B20_ERR_WRONG_DEVICE_FAMILY: Device is not DS18B20
 *            - 9 - 12: Resolution of DS18B20
 */
uint8_t TM_DS18B20_GetResolution(TM_OneWire_t* OneWireStruct, uint8_t* ROM);

/**
 * @brief  Sets resolution for specific DS18B20 device (DS18B20 EXCLUSIVE!)
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @param  resolution: Resolution for DS18B20 device. This parameter can be a value of @ref TM_DS18B20_Resolution_t enumeration.
 * @retval Success status:
 *            - TM_DS18B20_ERR_WRONG_DEVICE_FAMILY: Device is not DS18B20
 *            - > 0: Resolution set OK
 */
uint8_t TM_DS18B20_SetResolution(TM_OneWire_t* OneWireStruct, uint8_t* ROM, TM_DS18B20_Resolution_t resolution);

/**
 * @brief  Checks if device with specific ROM number is DS18S20
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12S80 device.
 *         Entire ROM address is 8-bytes long
 * @retval Device status
 *            - 0: Device is not DS18S20
 *            - 1: Device is DS18S20
 */
uint8_t TM_DS18S20_Is(uint8_t* ROM);

/**
 * @brief  Checks if device with specific ROM number is DS18B20
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @retval Device status
 *            - 0: Device is not DS18B20
 *            - 1: Device is DS18B20
 */
uint8_t TM_DS18B20_Is(uint8_t* ROM);

/**
 * @brief  Sets high alarm temperature to specific DS18B20 sensor
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @param  temp: integer value for temperature between -55 to 125 degrees
 * @retval Success status:
 *            - TM_DS18B20_ERR_WRONG_DEVICE_FAMILY: Device is not DS18B20
 *            - TM_DS18B20_SUCCESS: High alarm set OK
 */
uint8_t TM_DS18B20_SetAlarmHighTemperature(TM_OneWire_t* OneWireStruct, uint8_t* ROM, int8_t temp);

/**
 * @brief  Sets low alarm temperature to specific DS18B20 sensor
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @param  temp: integer value for temperature between -55 to 125 degrees
 * @retval Success status:
 *            - TM_DS18B20_ERR_WRONG_DEVICE_FAMILY: Device is not DS18B20
 *            - TM_DS18B20_SUCCESS: Low alarm set OK
 */
uint8_t TM_DS18B20_SetAlarmLowTemperature(TM_OneWire_t* OneWireStruct, uint8_t* ROM, int8_t temp);

/**
 * @brief  Disables alarm temperature for specific DS18B20 sensor
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @retval Success status:
 *            - TM_DS18B20_ERR_WRONG_DEVICE_FAMILY: Device is not DS18B20
 *            - TM_DS18B20_SUCCESS: Alarm disabled OK
 */
uint8_t TM_DS18B20_DisableAlarmTemperature(TM_OneWire_t* OneWireStruct, uint8_t* ROM);

/**
 * @brief  Searches for devices with alarm flag set
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @retval Alarm search status
 *            - 0: No device found with alarm flag set
 *            - > 0: Device is found with alarm flag
 * @note   To get all devices on one onewire channel with alarm flag set, you can do this:
\verbatim
while (TM_DS18B20_AlarmSearch(&OneWireStruct)) {
	//Read device ID here 
	//Print to user device by device
}
\endverbatim 
 * @retval 1 if any device has flag, otherwise 0
 */
uint8_t TM_DS18B20_AlarmSearch(TM_OneWire_t* OneWireStruct);

/**
 * @brief  Checks if all DS18B20 sensors are done with temperature conversion
 * @param  *OneWireStruct: Pointer to @ref TM_OneWire_t working structure (OneWire channel)
 * @retval Conversion status
 *            - 0: Not all devices are done
 *            - 1: All devices are done with conversion
 */
uint8_t TM_DS18B20_AllDone(TM_OneWire_t* OneWireStruct);

/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */
 
#endif

