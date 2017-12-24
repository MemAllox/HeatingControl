/*
 * ds1820_bank.c
 *
 *  Created on: 15.12.2017
 *      Author: Johannes
 */

/**
 ******************************************************************************
 * @file    ds1820_bank.c
 * @brief  Support for a parallel DS1820 bank (multiple DS1820 temperature sensors on one pin each)
 * @author  MemAllox
 ******************************************************************************
 *
 * This module supports connecting one <b>DS1820</b> (or DS18S20) temperature sensor on one pin each.
 * One whole GPIO port (like #GPIOA, #GPIOB, ..., #GPIOF) is dedicated to reading the sensors.
 * Each pin is a <b>slot</b> for exactly one sensor. Do not connect multiple sensors to one pin!
 * Also, don't forget the pull-up resistor (4.7k to Vcc).
 *
 * Error management is strict in this library. That means, if a sensor does not respond in the
 * expected way, the temperature will be set to NaN. The address of each sensor is called
 * <b>ROM number</b>. In case of connectivity problems, the module will automatically ask for an
 * (eventually new) ROM number. If there is no sensor responding properly, the #DS1820_ROM_State
 * will be set accordingly.
 *
 ******************************************************************************
 */

#include "ds1820_bank.h"

/**
 * Initializes the DS1820_Bank_Context and configures the GPIO registers.
 * The first \p pins of \p GPIOx will be slots to connect a DS1820 to.
 * Internally calls ds1820_bank_check_rom() to obtain all sensors' ROM numbers.
 * Call ds1820_bank_deinit() to free the allocated memory.
 * @param ctx Context for the DS1820 sensor slots
 * @param n Number of slots on port \p GPIOx beginning with Px0 up to Px(n-1)
 * @param GPIOx Port that holds the pins (like #GPIOA, #GPIOB, ..., #GPIOF)
 * @return
 * - 0 if no memory could be allocated or if there is no sensor available at at least one slot
 * - 1 if every slot yielded a proper ROM number
 */
int ds1820_bank_init(DS1820_Bank_Context *ctx, size_t n, GPIO_TypeDef *GPIOx) {
	if (n > 16) {
		n = 16;
	}

	ctx->slots = malloc(sizeof(DS1820_Bank_Slot) * n);

	if (!(ctx->slots)) {
		return 0;
	}

	ctx->n = n;

	for (int i = 0; i < n; i++) {
		// initialize the GPIOs and the onewire structs
		TM_OneWire_Init(&(ctx->slots[i].onewire), GPIOx, 1 << i);
		ctx->slots[i].rom_state = DS1820_STATE_UNKNOWN_ROM;
		ctx->slots[i].error_count = 0;
		ctx->slots[i].temperature = NAN;
		ds1820_bank_check_rom(ctx, i, DS1820_BANK_REQUEST_NEW_ROM);
	}

	return 1;
}

/**
 * Frees the memory allocated in ds1820_bank_init()
 * @param ctx
 */
void ds1820_bank_deinit(DS1820_Bank_Context *ctx) {
	free(ctx->slots);
}

/**
 * Checks if a ROM number is registered for a specific slot. If not, the slot will be asked for
 * a present sensor (or to be more precise its ROM number). The \p ctx->slots[i].rom_state will
 * be set to either #DS1820_STATE_OK if a sensor answere with its ROM number or #DS1820_STATE_ABSENT
 * if there is no sensor. If one sets \p request_new_rom to #DS1820_BANK_REQUEST_NEW_ROM, the slot
 * will be asked for its ROM code even if a valid ROM code is registered. (Valid in this context means,
 * that the device is of the DS1820 family)
 * @param ctx Context for the DS1820 sensor slots
 * @param i Number of the slot (Pxi of GPIOx)
 * @param request_new_rom #DS1820_BANK_REQUEST_NEW_ROM if function should ignore a valid ROM,
 * #DS1820_BANK_KEEP_OLD_ROM otherwise
 * @return
 * - 0 if no valid ROM code could be obtained
 * - 1 if a valid ROM code is registered or could be obtained
 */
int ds1820_bank_check_rom(DS1820_Bank_Context *ctx, uint8_t i,
		uint8_t request_new_rom) {
	// if there is a problem with the rom
	if (request_new_rom == DS1820_BANK_REQUEST_NEW_ROM
			|| ctx->slots[i].rom_state
					== DS1820_STATE_UNKNOWN_ROM|| ctx->slots[i].rom_state == DS1820_STATE_ABSENT
					|| ctx->slots[i].error_count >= DS1820_BANK_MAX_ERROR_COUNT) {
		// ask for rom again (eventually sets the rom no)
		int success = TM_OneWire_First(&(ctx->slots[i].onewire));

		if (success) {
			ctx->slots[i].rom_state = DS1820_STATE_OK;
			return 1; // got a rom
		}

		// no answer
		ctx->slots[i].rom_state = DS1820_STATE_ABSENT;
		return 0;
	}

	// everything was fine from the start
	return 1;
}

/**
 * Request a slot for a <b>temperature conversion</b>. That means (if the sensor is functioning)
 * that the sensor will start measuring the temperature. About 700ms later, the temperature can
 * be read from the sensor using ds1820_bank_update_temperature(). This function calls
 * ds1820_bank_check_rom() internally, so if there is no valid ROM, the function tries to obtain one.
 * However, if there is a valid ROM, this function does not get feedback from the sensor.
 * There might as well be no sensor at all. If that's the case, ds1820_bank_update_temperature()
 * will then yield an error.
 * @param ctx Context for the DS1820 sensor slots
 * @param i Number of the slot (Pxi of GPIOx)
 * @return
 * - 0 if there was no valid ROM and there is no sensor available
 * - 1 if there is a valid ROM
 */
int ds1820_bank_start_conversion(DS1820_Bank_Context *ctx, uint8_t i) {
	if (!ds1820_bank_check_rom(ctx, i, DS1820_BANK_KEEP_OLD_ROM)) {
		return 0; // critical!
	}

	return TM_DS18S20_Start(&(ctx->slots[i].onewire),
			ctx->slots[i].onewire.ROM_NO);
}

/**
 * Calls ds1820_bank_start_conversion() for every slot.
 * @param ctx Context for the DS1820 sensor slots
 * @return
 * - 0 if there was no valid ROM and no sensor available for at least one slot
 * - 1 if every slot has a valid ROM
 */
int ds1820_bank_start_conversions(DS1820_Bank_Context *ctx) {
	int succcess = 1;

	for (int i = 0; i < ctx->n; i++) {
		succcess &= ds1820_bank_start_conversion(ctx, i);
	}

	return succcess;
}

/**
 * Requests the temperature from a slot. Requires a temperature conversion request beforehand
 * by calling ds1820_bank_start_conversion() or ds1820_bank_start_conversions(). <b>If one does not
 * await the finish of the conversion (after ca. 700ms), an error will occur!</b> This function
 * calls ds1820_bank_check_rom() internally so if there is no valid ROM, it will try to receive one.
 * If no ROM can be aquired, 0 will be returned, \p ctx->slots[i].temperature will be set to #NAN and
 * #DS1820_BANK_SENSOR_ERROR(i) will be called.
 * If there is a ROM, the temperature will be read. An error upon reading the temperature will result
 * in a return value of 0, but \p ctx->slots[i].temperature will <b>not</b> be modified until the
 * temperature reading failed #DS1820_BANK_MAX_ERROR_COUNT times. Only then a new ROM will be
 * requested. Upon a failure, the temperature will be set to #NAN and #DS1820_BANK_SENSOR_ERROR(i)
 * will be called.
 * @param ctx Context for the DS1820 sensor slots
 * @param i Number of the slot (Pxi of GPIOx)
 * @return
 * - 0 if no temperature could be aquired
 * - 1 if the temperature got updated by a newer value
 */
int ds1820_bank_update_temperature(DS1820_Bank_Context *ctx, uint32_t i) {
	// if error_count exceeded
	if (ctx->slots[i].error_count >= DS1820_BANK_MAX_ERROR_COUNT) {
		// ask for (eventually new) rom
		if (!ds1820_bank_check_rom(ctx, i, DS1820_BANK_REQUEST_NEW_ROM)) {
			// error reading rom
			ctx->slots[i].temperature = NAN;
			DS1820_BANK_SENSOR_ERROR(i);
			return 0;
		}
	} else { // error_count not exceeded -> request rom only when existing rom is invalid
		if (!ds1820_bank_check_rom(ctx, i, DS1820_BANK_KEEP_OLD_ROM)) {
			// error reading rom
			ctx->slots[i].temperature = NAN;
			DS1820_BANK_SENSOR_ERROR(i);
			return 0;
		}
	}

	// rom seems to be (still) valid - read temperature
	int success = TM_DS18S20_Read(&(ctx->slots[i].onewire),
			ctx->slots[i].onewire.ROM_NO,
			&(ctx->slots[i].temperature)) == TM_DS18B20_SUCCESS;

	if (!success) {
		// error reading temperature
		if (ctx->slots[i].error_count < INT8_MAX) {
			ctx->slots[i].error_count++;
		}

		if (ctx->slots[i].error_count > DS1820_BANK_MAX_ERROR_COUNT) {
			// rom is available, but temperature keeps being invalid
			ctx->slots[i].temperature = NAN;
			DS1820_BANK_SENSOR_ERROR(i);
		} else {
			// error, but keep temperature until sensor is unreachable or error_count reaches maximum
		}

		return 0;
	}

	// no error
	ctx->slots[i].error_count = 0;
	return success;
}

/**
 * Calls ds1820_bank_update_temperature() for every slot.
 * @param ctx Context for the DS1820 sensor slots
 * @return
 * - 0 if at least one slot could not aquire a temperature
 * - 1 if every single slot could update its temperature
 */
int ds1820_bank_update_temperatures(DS1820_Bank_Context *ctx) {
	int succcess = 1;

	for (int i = 0; i < ctx->n; i++) {
		succcess &= ds1820_bank_update_temperature(ctx, i);
	}

	return succcess;
}
