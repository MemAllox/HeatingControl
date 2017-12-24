/**
 ******************************************************************************
 * @file    HD44780.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    16_January-2012
 * @brief   HD44780.c
 * @link    https://my.st.com/content/my_st_com/en/products/embedded-software/mcus-embedded-software/stm8-embedded-software/stsw-stm8063.license%3d1513968852433.html
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * FOR MORE INFORMATION PLEASE READ CAREFULLY THE LICENSE AGREEMENT FILE
 * LOCATED IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
 *
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "HD44780.h"

/** @addtogroup STM8S_StdPeriph_Driver
 * @{
 */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Delay(ms)						HAL_Delay(ms)
#define GPIO_WriteLow(port, pin)		HAL_GPIO_WritePin(port, pin, 0)
#define GPIO_WriteHigh(port, pin)		HAL_GPIO_WritePin(port, pin, 1)
#define GPIO_Write(port, bitmask)		{port->ODR = bitmask;}
#define GPIO_ReadOutputData(port)		(port->IDR)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t LCD_cursor_col_count = 16;
uint8_t LCD_cursor_row_count = 4;
uint8_t LCD_cursor_col = 0;		// x 	(0 is leftmost)
uint8_t LCD_cursor_row = 0;		// y 	(0 is uppermost)

/* Private function prototypes -----------------------------------------------*/
#if defined(_RAISONANCE_)
/* Do not remove for Raisonance compiler */
void dummy(void)
{
	/*printf declaration enables vsprintf definition*/
	printf("");
}
#endif

/**
 * @addtogroup HD44780_Functions
 * @{
 */

/**
 * @brief  Initializes Character Generator CGRAM with custom characters data
 * @param  Table containing the character definition values
 * @param  Index of the slot the character should be stored in (0-7)
 * @retval None
 */
void LCD_LOAD_CGRAM(char tab[8], uint8_t slot) {
	uint8_t index;

	if (slot > 7) {
		return;
	}

	LCD_CMD(CGRAM_address_start + 8 * slot);

	/* Each character contains 8 definition values (one for each pixel row)*/
	for (index = 0; index < 8; index++) {
		/* Store values in LCD*/
		LCD_printchar(tab[index]);
		Delay(1);
	}
}

/**
 * @brief  Activate Power Pin that supplies LCD module
 * @param  None
 * @param  None
 * @retval None
 */
void LCD_PWRON(void) {
	GPIO_WriteHigh(LCDPwrPort, LCDPwrPin);
}

/**
 * @brief  Disable Power Pin that supplies LCD module
 * @param  None
 * @param  None
 * @retval None
 */
void LCD_PWROFF(void) {
	GPIO_WriteLow(LCDPwrPort, LCDPwrPin);
}

/**
 * @brief  Activate Enable Pin from LCD module
 * @param  None
 * @param  None
 * @retval None
 */
void LCD_ENABLE(void) {
	GPIO_WriteHigh(LCDControlPort, LCD_Enable);
	Delay(2);
	GPIO_WriteLow(LCDControlPort, LCD_Enable);
}

/**
 * @brief  Command data sent to LCD module
 * @param  command value to be sent
 * @param  None
 * @retval None
 */
void LCD_CMD(unsigned char cmd_data) {
	GPIO_WriteLow(LCDControlPort, LCD_RS);
	GPIO_Write(LCDPort,
			(GPIO_ReadOutputData(LCDPort) & 0xF0) | ((cmd_data >> 4) & 0x0F));
	LCD_ENABLE();
	GPIO_Write(LCDPort,
			(GPIO_ReadOutputData(LCDPort) & 0xF0) | (cmd_data & 0x0F));
	LCD_ENABLE();
	Delay(2);
}

/**
 * @brief  Initializes HD44780 LCD module in 4-bit mode
 * @param  None
 * @param  None
 * @retval None
 */
void LCD_INIT(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Configure LCDPort D0-3 output push-pull low LCD Bus*/
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LCDPort, &GPIO_InitStruct);

	/* Configure LCDPort E output push-pull low LCD Enable Pin*/
	GPIO_InitStruct.Pin = LCD_RS | LCD_Enable;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LCDControlPort, &GPIO_InitStruct);

	/* Configure LCDPwrOnPort VDD output push-pull low LCD Power Supply*/
	GPIO_InitStruct.Pin = LCDPwrPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LCDPwrPort, &GPIO_InitStruct);

	GPIO_WriteHigh(LCDControlPort, LCD_Enable);
	GPIO_WriteLow(LCDControlPort, LCD_RS);
	//Initialization of HD44780-based LCD (4-bit HW)
	LCD_CMD(0x33);
	Delay(4);
	LCD_CMD(0x32);
	Delay(4);
	//Function Set 4-bit mode
	LCD_CMD(0x28);
	//Display On/Off Control
	LCD_CMD(0x0C);
	//Entry mode set
	LCD_CMD(0x06);
	LCD_CLEAR_DISPLAY();
	//Minimum delay to wait before driving LCD module
	Delay(200);
}

/**
 * @brief  Print Character on LCD module
 * @param  Ascii value of character
 * @param  None
 * @retval None
 */
void LCD_printchar(unsigned char ascode) {
	if (LCD_cursor_col >= LCD_cursor_col_count) {
		return;
	}

	GPIO_WriteHigh(LCDControlPort, LCD_RS);
	GPIO_Write(LCDPort,
			(GPIO_ReadOutputData(LCDPort) & 0xF0) | ((ascode >> 4) & 0x0F));
	LCD_ENABLE();
	GPIO_Write(LCDPort, (GPIO_ReadOutputData(LCDPort) & 0xF0) | (ascode & 0x0F));
	LCD_ENABLE();
	Delay(2);

	LCD_cursor_col++;
}

/**
 * @brief  Display of a characters string
 * @param  Text to be displayed (terminated by '\0')
 * @param  None
 * @retval None
 */
void LCD_printstring(unsigned char *text) {
	while (*text) {
		if (*text == '\n') {
			LCD_LOCATE(LCD_cursor_row + 1, 0);	// next line
			text++;
		} else {
			LCD_printchar(*text++);
		}
	}
}

/**
 * @brief  Clear LCD module display
 * @param  None
 * @param  None
 * @retval None
 */
void LCD_CLEAR_DISPLAY(void) {
	LCD_CMD(0x01);
	Delay(2);
}

/**
 * @brief  Set Cursor on second row 1st digit
 * @param  None
 * @param  None
 * @retval None
 */
void LCD_2ndROW(void) {
	LCD_CMD(0xC0);
}

/**
 * @brief  Set Cursor to Home position
 * @param  None
 * @param  None
 * @retval None
 */
void LCD_HOME(void) {
	LCD_CMD(0x02);
	Delay(2);
}

/**
 * @brief  Shift display to left
 * @param  None
 * @param  None
 * @retval None
 */
void LCD_LSHIFT(void) {
	LCD_CMD(0x18);
}

/**
 * @brief  Shift display to right
 * @param  Text to be displayed
 * @param  None
 * @retval None
 */
void LCD_RSHIFT(void) {
	LCD_CMD(0x1C);
}

/**
 * @brief  Set Display On
 * @param  None
 * @param  None
 * @retval None
 */
void LCD_DISP_ON(void) {
	LCD_CMD(0x0C);
}

/**
 * @brief  Set Display Off
 * @param  None
 * @param  None
 * @retval None
 */
void LCD_DISP_OFF(void) {
	LCD_CMD(0x08);
}

/**
 * @brief  Set Cursor to a specified location given by row and column information
 * @param  Row Number (0-1)
 * @param  Column Number (0-15) Assuming a 2 X 16 characters display
 * @retval None
 */
void LCD_LOCATE(uint8_t row, uint8_t column) {
	if (column >= LCD_cursor_col_count) {
		return;
	}

	uint8_t row_bits = 0;

	switch (row) {
	case 0:
		row_bits = 0x00 | 0x80;
		break;
	case 1:
		row_bits = 0x40 | 0x80;
		break;
	case 2:
		row_bits = 0x10 | 0x80;
		break;
	case 3:
		row_bits = 0x50 | 0x80;
		break;
	default:
		return;
	}

	LCD_cursor_row = row;
	LCD_cursor_col = column;

	LCD_CMD(column | row_bits);
}

/**
 * @brief  lcd printf function
 * @param  string with standard defined formats
 * @param
 * @retval None
 */
void LCD_printf(const char *fmt, ...) {
	int32_t i;
	uint32_t text_size, letter;
	static char text_buffer[32];
	va_list args;

	va_start(args, fmt);
	text_size = vsprintf(text_buffer, fmt, args);

	// Process the string
	for (i = 0; i < text_size; i++) {
		letter = text_buffer[i];

		if (letter == '\n')
			LCD_LOCATE(LCD_cursor_row + 1, 0);
		else {
			if ((letter > 0x1F) && (letter < 0x80))
				LCD_printchar(letter);
		}
	}
}

/**
 * @}
 */

/**
 * @}
 */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
