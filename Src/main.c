/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
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

#include <HD44780.h>
#include "main.h"	// STM32F303 DISCOVERY defines

#include <string.h>
#include "stm32f3xx_hal.h"
#include "can.h"
#include "usart.h"
#include "gpio.h"
#include "ds1820_bank.h"

//#define CAN_MCP2551 1
#define CAN_ID 100
//#define SENDER 1

void SystemClock_Config(void);

int main(void) {
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_USART1_Init();

	__HAL_RCC_GPIOE_CLK_ENABLE()
	;
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11
			| GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
	MX_CAN_Init();

	char str[] = "Hello World\n";
	HAL_USART_Transmit(&husart1, (unsigned char*) str, sizeof(str) - 1, 1000);

//	DS1820_Bank_Context ds1820_ctx;
//	ds1820_bank_init(&ds1820_ctx, 15, GPIOB);
//
//	char buf[10] = { 0 };
//	while (1) {
//
//		for (int j = 0; j < 6; j++) {
//			ds1820_bank_start_conversions(&ds1820_ctx);
//			//HAL_Delay(750);
//			ds1820_bank_update_temperatures(&ds1820_ctx);
//
//			for (int i = 0; i < ds1820_ctx.n; i++) {
//				sprintf(buf, "(%d) %d   ", i, ds1820_ctx.slots[i].rom_state);
//				HAL_USART_Transmit(&husart1, (uint8_t*) buf, strlen(buf), 1000);
//			}
//			HAL_USART_Transmit(&husart1, "\n", 1, 1000);
//			for (int i = 0; i < ds1820_ctx.n; i++) {
//				sprintf(buf, "(%d)%4.1f ", i, ds1820_ctx.slots[i].temperature);
//				HAL_USART_Transmit(&husart1, (uint8_t*) buf, strlen(buf), 1000);
//			}
//			HAL_USART_Transmit(&husart1, "\n", 1, 1000);
//		}
	//ds1820_bank_deinit(&ds1820_ctx);

#ifdef hd44780
	/* Define 6 custom characters to display bar graph*/
	char STCustom[48] = {0x04, 0x0E, 0x0E, 0x0E, 0x1F, 0x00, 0x04, 0x00, // Blank
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,// 1column  |
		0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,// 2columns ||
		0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c,// 3columns |||
		0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,// 4columns ||||
		0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,// 5columns |||||
	};
	char bell[8] = {0x04, 0x0E, 0x0E, 0x0E, 0x1F, 0x00, 0x04, 0x00};

	LCD_PWRON();
	/* Min. delay to wait before initialization after LCD power ON */
	/* Value is ms units*/
	HAL_Delay(100);
	/* Initialization of LCD */
	LCD_INIT();

	LCD_CLEAR_DISPLAY();

	/* Loading 6 characters @CGRAM address from STCustom tab */
	LCD_LOAD_CGRAM(bell, 0);
	LCD_LOAD_CGRAM(STCustom+8, 1);

	/* Set cursor to the chosen position*/
	LCD_LOCATE(0, 0);
	/* Print string on LCD (must be ended with \n)*/
	LCD_printstring((unsigned char*) "STM32\nDiscovery");
	LCD_LOCATE(2, 0);
	LCD_printchar('\x0');	// custom char at slot 0x00

//	for (int i = 0; i < 256/(4*16); i++) {
//		for (int row = 0; row < 4; row++) {
//			LCD_LOCATE(row, 0);
//			for (int col = 0; col < 16; col++) {
//				LCD_printchar(i*4*16+row*16+col);
//			}
//		}
//		HAL_Delay(5000);
//		LCD_CLEAR_DISPLAY();
//	}
#endif

#define CAN_MCP2551
#define SENDER

#ifdef CAN_MCP2551
	CanTxMsgTypeDef msg;
	msg.StdId = CAN_ID;
	msg.ExtId = 0;
	msg.IDE = CAN_ID_STD;
	msg.RTR = CAN_RTR_DATA;
	msg.DLC = 0x8;
	strcpy(&(msg.Data), "Hallo Wo");

#ifdef SENDER
	hcan.pTxMsg = &msg;
	volatile int result = HAL_CAN_Transmit(&hcan, 1000);
	if (result != HAL_TIMEOUT) {
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 1); // green
		while (1)
			;
	} else {
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1); // green
	}

#else
	CAN_FilterConfTypeDef sFilterConfig;
	static CanTxMsgTypeDef TxMessage;
	static CanRxMsgTypeDef RxMessage;

//	/*##-2- Configure the CAN Filter ###########################################*/
//	sFilterConfig.FilterNumber = 0;
//	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
//	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
//	sFilterConfig.FilterIdHigh = 0x0000;
//	sFilterConfig.FilterIdLow = 0x0000;
//	sFilterConfig.FilterMaskIdHigh = 0x0000;
//	sFilterConfig.FilterMaskIdLow = 0x0000;
//	sFilterConfig.FilterFIFOAssignment = CAN_FIFO0;
//	sFilterConfig.FilterActivation = ENABLE;
//	sFilterConfig.BankNumber = 0;
//
//	if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK) {
//		/* Filter configuration Error */
//		Error_Handler();
//	}

	while (1) {
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_8);
		volatile int result = HAL_CAN_Receive(&hcan, CAN_FIFO0, 1000);
		if (result != HAL_TIMEOUT) {
			HAL_USART_Transmit(&husart1, hcan.pRxMsg->Data, 8, 1000);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 1); // green
			while (1)
			;
		}
	}
#endif
#endif
}

/** System Clock Configuration
 */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void _Error_Handler(char * file, int line) {
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 1);

	while (1) {
	}
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */

}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
