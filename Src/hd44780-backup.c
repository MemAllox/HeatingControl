//#include <hd44780-backup.h>
//#include <stm32f3xx_hal.h>
//
//#define LCD_PORT GPIOB
//#define LCD_RS GPIO_PIN_11
//#define LCD_CLOCK GPIO_PIN_10
//#define LCD_4 GPIO_PIN_12
//#define LCD_5 GPIO_PIN_13
//#define LCD_6 GPIO_PIN_14
//#define LCD_7 GPIO_PIN_15
//
//// Use _lcd_delay_ms as our default delay function
//void (*lcd_delay_ms)(uint32_t) = HAL_Delay;
//
//// Various displays exist, don't make assumptions
//uint8_t lcd_chars = 0;
//uint8_t lcd_lines = 0;
//uint8_t *lcd_line_addresses = 0;
//// "Private" globals
//uint8_t _lcd_char = 0;
//uint8_t _lcd_line = 0;
//
//void lcd_clock(void) {
//	// Pulse clock
//	HAL_GPIO_WritePin(LCD_PORT, LCD_CLOCK, 1);
//	lcd_delay_ms(1);
//	HAL_GPIO_WritePin(LCD_PORT, LCD_CLOCK, 0);
//	lcd_delay_ms(1);
//}
//
//void lcd_init(void) {
//	/* Init GPIO pin */
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.Pin = LCD_RS | LCD_CLOCK | LCD_4 | LCD_5 | LCD_6 | LCD_7;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
//	HAL_GPIO_Init(LCD_PORT, &GPIO_InitStruct);
//}
//
//void lcd_reset(void) {
//	// Resets display from any state to 4-bit mode, first nibble.
//
//	// Set everything low first
//	HAL_GPIO_WritePin(LCD_PORT,
//			LCD_RS | LCD_CLOCK | LCD_4 | LCD_5 | LCD_6 | LCD_7, 0);
//
//	// Reset strategy below based on Wikipedia description, should recover
//	// from any setting
//
//	// Write 0b0011 three times
//	// (Everyday Practical Electronics says 3 times, Wikipedia says 2 times,
//	// 3 seems to work better).
//	HAL_GPIO_WritePin(LCD_PORT, LCD_5 | LCD_4, 0);
//	lcd_clock();
//	lcd_clock();
//	lcd_clock();
//	// LCD now guaranteed to be in 8-bit state
//
//	// Now write 0b0010 (set to 4-bit mode, ready for first nibble)
//	HAL_GPIO_WritePin(LCD_PORT, LCD_4, 0);
//	lcd_clock();
//}
//
///* TODO This function should achieve the same as the lcd_write below, however
// * it appears to be a little problematic.
// * Rather than the LCD_4 and LCD_RS defines, direct integers have to be used
// * for proper masks to be calculated.
// * Aside from this, setting the RS bit seems to go wrong.
// void lcd_write(uint8_t byte, uint8_t rs)
// {
// // Writes a byte to the display (rs must be either 0 or 1)
//
// // Retrieve current port state
// uint16_t port_state = gpio_port_read(LCD_PORT);
//
// // Write second nibble and register select;
// port_state &= ~((0b1111 << 12) | 10);
// port_state |= ((((byte >> 4) & 0b1111) << 12) | (rs ? 10 : 0));
// gpio_port_write(LCD_PORT, port_state);
// lcd_clock();
//
// // Write first nibble
// port_state &= ~(0b1111 << 12);
// port_state |= ((byte & 0b1111) << 12);
// gpio_port_write(LCD_PORT, port_state);
// lcd_clock();
// }
// */
//
//void lcd_write(uint8_t byte, uint8_t rs) {
//	// Writes a byte to the display (rs must be either 0 or 1)
//
//	// Write second nibble and set RS
//
//	if ((byte >> 4) & 1)
//		HAL_GPIO_WritePin(LCD_PORT, LCD_4, 1);
//	else
//		HAL_GPIO_WritePin(LCD_PORT, LCD_4, 0);
//
//	if ((byte >> 5) & 1)
//		HAL_GPIO_WritePin(LCD_PORT, LCD_5, 1);
//	else
//		HAL_GPIO_WritePin(LCD_PORT, LCD_5, 0);
//
//	if ((byte >> 6) & 1)
//		HAL_GPIO_WritePin(LCD_PORT, LCD_6, 1);
//	else
//		HAL_GPIO_WritePin(LCD_PORT, LCD_6, 0);
//
//	if ((byte >> 7) & 1)
//		HAL_GPIO_WritePin(LCD_PORT, LCD_7, 1);
//	else
//		HAL_GPIO_WritePin(LCD_PORT, LCD_7, 0);
//
//	if (rs)
//		HAL_GPIO_WritePin(LCD_PORT, LCD_RS, 1);
//	else
//		HAL_GPIO_WritePin(LCD_PORT, LCD_RS, 0);
//
//	lcd_clock();
//
//	// Write first nibble
//
//	if (byte & 1)
//		HAL_GPIO_WritePin(LCD_PORT, LCD_4, 1);
//	else
//		HAL_GPIO_WritePin(LCD_PORT, LCD_4, 0);
//
//	if ((byte >> 1) & 1)
//		HAL_GPIO_WritePin(LCD_PORT, LCD_5, 1);
//	else
//		HAL_GPIO_WritePin(LCD_PORT, LCD_5, 0);
//
//	if ((byte >> 2) & 1)
//		HAL_GPIO_WritePin(LCD_PORT, LCD_6, 1);
//	else
//		HAL_GPIO_WritePin(LCD_PORT, LCD_6, 0);
//
//	if ((byte >> 3) & 1)
//		HAL_GPIO_WritePin(LCD_PORT, LCD_7, 1);
//	else
//		HAL_GPIO_WritePin(LCD_PORT, LCD_7, 0);
//
//	lcd_clock();
//}
//
//void lcd_clear(void) {
//	// Clears display, resets cursor
//	lcd_write(0b00000001, 0);
//	_lcd_char = 0;
//	_lcd_line = 0;
//}
//
//void lcd_display_settings(uint8_t on, uint8_t underline, uint8_t blink) {
//	// "Display On/Off & Cursor" command. All parameters must be either 0 or 1
//
//	lcd_write(0b00001000 | (on << 2) | (underline << 1) | blink, 0);
//}
//
//void lcd_display_address(uint8_t address) {
//	lcd_write(0b10000000 | address, 0);
//}
//
//void lcd_print(char string[]) {
//	uint8_t i;
//	for (i = 0; string[i] != 0; i++) {
//		// If we know the display properties and a newline character is
//		// present, print the rest of the string on the new line.
//		if (lcd_lines && string[i] == '\n') {
//			if (_lcd_line < lcd_lines) {
//				lcd_display_address(lcd_line_addresses[_lcd_line++]);
//				_lcd_char = 0;
//			}
//		} else {
//			// If we know the display properties and have reached the end of
//			// line, print the rest on the next line
//			if (lcd_chars)
//				if ((_lcd_char == lcd_chars) && (_lcd_line < lcd_lines)) {
//					lcd_display_address(lcd_line_addresses[_lcd_line++]);
//					_lcd_char = 0;
//				}
//			lcd_write(string[i], 1);
//			if (lcd_chars)
//				_lcd_char++;
//		}
//	}
//}
