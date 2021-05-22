#include "lcd_menu.h"

void printMenu(char *menu[], int n) {

	setCursor(0, 0);
	printChar(CHOICE);

	for (int i = 0; i < n; i++) {
		setCursor(i, 1);
		printText(menu[i]);
	}
}

void printText(char *string) {
	Lcd_string(&lcd, string);
}

void printNum(int num) {
	Lcd_int(&lcd, num);
}

void printChar(int charcode) {
	Lcd_char(&lcd, charcode);
}

void setCursor(uint8_t row, uint8_t col) {
	Lcd_cursor(&lcd, row, col);
}

void clearLCD() {
	Lcd_clear(&lcd);
}

