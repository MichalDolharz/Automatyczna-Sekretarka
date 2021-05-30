#ifndef INC_LCD_MENU_H_
#define INC_LCD_MENU_H_

#include "lcd.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "usb_host.h"
#include "fatfs.h"

#define CHOICE 0xA5 //0x7E //
#define NO_CHOICE 0x2D
#define ROWS 4

extern Lcd_HandleTypeDef lcd;
extern int debug;

void printMenu(char *menu[], int menuLen, int startingPoint);
void printText(char *string);
void printNum(int num);
void printChar(int charcode);
void setCursor(uint8_t row, uint8_t col);
void clearLCD();

void updateMenu(char *menu[], int menuLen, int *menuUpDown,
		int *menuStartingPoint, int joystickState);
void menuClicked(int menuNum);
void handleInformationMenu();
void handleRecordsMenu();
void handleSavedMenu();
void handleRecordsMenuClicked(int menuNum, char *record);
void handleRecordClicked(int menuNum, char *record);
void handlePlay();
void handleSave();
void handleRemove();
void setLight(int OnOff);

#endif /* INC_LCD_MENU_H_ */
