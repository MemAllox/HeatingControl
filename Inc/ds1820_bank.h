/*
 * ds1820_bank.h
 *
 *  Created on: 15.12.2017
 *      Author: Johannes
 */

#ifndef DS1820_BANK_H_
#define DS1820_BANK_H_

#include <stdlib.h>
#include <math.h>
#include "stm32f3xx_hal.h"
#include "tm_stm32_ds18b20.h"

/**
 * Will be called by ds1820_bank_update_temperature() (or ds1820_bank_update_temperature()) if no
 * sensor is available on a specific slot. Also, if a sensor is delivering its ROM properly but
 * is incapable of delivering a temperature, #DS1820_BANK_SENSOR_ERROR(i) will be called.
 * <b>Attention:</b> Don't forget to request a temperature conversion by ds1820_bank_start_conversion()
 * or ds1820_bank_start_conversions() or no valid temperature can be read. Also wait for the conversion
 * to be finished!
 */
#define DS1820_BANK_SENSOR_ERROR(i)

/**
 * The amount of times a slot asks its sensor for the temperature and fails before it requests the
 * ROM. Until then, the temperature will not be modified if there is a problem with the connection.
 * Set this to -1 if you want the module to ask for the sensors' ROM every time upon calling
 * ds1820_bank_update_temperature() or ds1820_bank_update_temperatures().
 */
#define DS1820_BANK_MAX_ERROR_COUNT		0

/** Make ds1820_bank_check_rom() accept an old valid ROM until an error occurs */
#define DS1820_BANK_KEEP_OLD_ROM		0
/** Make ds1820_bank_check_rom() request a new ROM even if there is already an old one registered */
#define DS1820_BANK_REQUEST_NEW_ROM		1

/**
 * The connectivity state of the sensor. Note that this only describes the state of the ROM,
 * not if the sensor is actually delivering valid temperature values.
 */
typedef enum {
	DS1820_STATE_UNKNOWN_ROM, DS1820_STATE_OK, DS1820_STATE_ABSENT
} DS1820_ROM_State;

/**
 * A struct to hold the data for each slot.
 */
typedef struct {
	TM_OneWire_t onewire;	// connection details + ROM number
	float temperature;		// read temperature
	DS1820_ROM_State rom_state;		// connectivity state (ROM number)
	uint8_t error_count;	// number of consecutive errors
} DS1820_Bank_Slot;

/**
 * The context to hold the data for a whole bank of sensors. Those sensors are are at the same
 * GPIO port (e.g. #GPIOA, #GPIOB, ..., #GPIOF) on \p n pins starting from Px0 up to Px(n-1).
 */
typedef struct {
	size_t n;
	DS1820_Bank_Slot* slots;
} DS1820_Bank_Context;


int ds1820_bank_init(DS1820_Bank_Context *ctx, size_t n, GPIO_TypeDef *GPIOx);
void ds1820_bank_deinit(DS1820_Bank_Context *ctx);
int ds1820_bank_check_rom(DS1820_Bank_Context *ctx, uint8_t i,
		uint8_t request_new_rom);
int ds1820_bank_start_conversion(DS1820_Bank_Context *ctx, uint8_t i);
int ds1820_bank_start_conversions(DS1820_Bank_Context *ctx);
int ds1820_bank_update_temperature(DS1820_Bank_Context *ctx, uint32_t i);
int ds1820_bank_update_temperatures(DS1820_Bank_Context *ctx);

#endif /* DS1820_BANK_H_ */
