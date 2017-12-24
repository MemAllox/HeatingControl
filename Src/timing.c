/*
 * timing.c
 *
 *  Created on: 14.12.2017
 *      Author: Johannes
 */

#include "timing.h"

/**
 * Initializes the timing module capable of waiting for periods down to 1us.
 * If periods bigger than 1ms are needed, please use HAL_Delay() and HAL_GetTick().
 */
void timing_init(void) {
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;	// enable

	DWT->CYCCNT = 0; // reset the counter
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // enable the counter
}

/**
 * Waits a given time in us.
 * If periods bigger than 1ms are needed, please use HAL_Delay() and HAL_GetTick().
 * @param period of time in us
 */
void timing_delay_us(unsigned int us) {
	unsigned int start, ticks;

	// TODO when does the CYCCNT overflow? is there an overflow flag to deal with it?
	start = DWT->CYCCNT;
	ticks = us * (SystemCoreClock/1000000);

	while ((DWT->CYCCNT - start) < ticks);
}
