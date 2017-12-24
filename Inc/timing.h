/*
 * timing.h
 *
 *  Created on: 14.12.2017
 *      Author: Johannes
 */

#ifndef TIMING_H_
#define TIMING_H_

#include "stm32f3xx.h"


void timing_init(void);

void timing_delay_us(unsigned int us);

/* see also stm32f3xx_hal.h for timing with ms-ticks
 * void HAL_Delay(__IO uint32_t Delay);
 * uint32_t HAL_GetTick(void);
 */


#endif /* TIMING_H_ */
