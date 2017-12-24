/**
 ******************************************************************************
 * File Name          : CAN.c
 * Description        : This file provides code for the configuration
 *                      of the CAN instances.
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2017 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "can.h"

#include "gpio.h"

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void) {
	//refer to http://www.diller-technologies.de/stm32.html#can
	hcan.Instance = CAN;
	hcan.Init.Prescaler = 16;
	hcan.Init.Mode = CAN_MODE_NORMAL;
	hcan.Init.SJW = CAN_SJW_1TQ;
	hcan.Init.BS1 = CAN_BS1_1TQ;
	hcan.Init.BS2 = CAN_BS2_1TQ;
	hcan.Init.TTCM = DISABLE; // time triggered communication mode (tx and rx timestamps)
	hcan.Init.ABOM = DISABLE;			// automatic bus-off management
	hcan.Init.AWUM = DISABLE;// automatic wakeup mode (how to exit sleep mode)
	hcan.Init.NART = DISABLE;		// no automatic retransmission (on tx error)
	hcan.Init.RFLM = DISABLE;// receive FIFO locked mode (if new msgs are kept)
	hcan.Init.TXFP = DISABLE;// transmit FIFO priority (if id or chronological)
	if (HAL_CAN_Init(&hcan) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle) {
	GPIO_InitTypeDef GPIO_InitStruct;
	if (canHandle->Instance == CAN) {
		__HAL_RCC_CAN1_CLK_ENABLE()
		;

		/**CAN GPIO Configuration
		 PD0     ------> CAN_RX
		 PD1     ------> CAN_TX
		 */
		GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_CAN;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	}
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle) {
	if (canHandle->Instance == CAN) {
		__HAL_RCC_CAN1_CLK_DISABLE();

		/**CAN GPIO Configuration
		 PD0     ------> CAN_RX
		 PD1     ------> CAN_TX
		 */
		HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0 | GPIO_PIN_1);
	}
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
