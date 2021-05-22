#ifndef INC_LCD_MENU_H_
#define INC_LCD_MENU_H_

#include "lcd.h"

#define CHOICE 0x7E
#define MAIN_MENU_LEN sizeof(mainMenu) / sizeof(mainMenu[0])

extern Lcd_HandleTypeDef lcd;

void printMenu(char *menu[], int n);
void printText(char *string);
void printNum(int num);
void printChar(int charcode);
void setCursor(uint8_t row, uint8_t col);
void clearLCD();

#endif /* INC_LCD_MENU_H_ */
