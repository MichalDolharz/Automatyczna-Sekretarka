#include "lcd_menu.h"

void menuClicked(int menuNum) {

	switch (menuNum) {
	case 0: // wiadomosci
		break;

	case 1: // zachowane
		break;
	case 2: // ustawienia
		break;
	case 3: // informacje
		handleInformationMenu();
		break;
	case 4: // zakoncz
		break;
	}

	clearLCD();
	setCursor(0, 0);
}

void handleInformationMenu() {
	char *info[] = { "Powrot", "Data wydania:", " - 03.06.2021", "Autorzy:",
			" - Dawid Jarzabek", " - Michal Dolharz" };

	int menuUpDown = 0;

	int joystickState = 0;
	int joystickButtonState = 0;
	int *menuStartingPoint = 0;

	setCursor(0, 0);
	printMenu(info, sizeof(info) / sizeof(info[0]), 0);

	// Petla trwa dopoki nie wybierze sie opcji "Powrot"
	while (!(menuUpDown == 0 && joystickButtonState == 1)) {

		joystickState = Joystick_State(); // poruszenie joystickiem
		joystickButtonState = (!HAL_GPIO_ReadPin(Joystick_Button_GPIO_Port,
		Joystick_Button_Pin)); // wcisniecie joysticka

		// Jezeli poruszono joystickiem w dol lub w gore, to aktualizuje menu
		if (joystickState == 1 || joystickState == 2) {
			updateMenu(&info, sizeof(info) / sizeof(info[0]), &menuUpDown,
					&menuStartingPoint, joystickState);
		}
	}
}

void updateMenu(char *menu[], int menuLen, int *menuUpDown,
		int *menuStartingPoint, int joystickState) {

	int position = 0;
	//int info = 0;

	// Tylko zmiana opcji w pionie
	switch (joystickState) {
	case 1: // gora
		if (*menuUpDown != menuLen - 1) {

			// Wyczyszczenie miejsca po poprzednim znaku
			setCursor(*menuUpDown - *menuStartingPoint, 0);
			printText(" ");

			// Zwiekszenie indeksu opcji menu
			*menuUpDown += 1;
		}
		break;
	case 2: // dol
		if (*menuUpDown != 0) {

			// Wyczyszczenie miejsca po poprzednim znaku
			setCursor(*menuUpDown - *menuStartingPoint, 0);
			printText(" ");

			// Zmniejszenie indeksu opcji menu
			*menuUpDown -= 1;
		}
		break;
	}
	/* Ponizej trzy opcje:
	 * - przesuniecie menu w dol,
	 * - przesuniecie menu w gore,
	 * - przesuniecie samego wskaznika bez przesuwania menu
	 */

	// Przesuniecie menu w dol
	if (*menuUpDown < *menuStartingPoint) {
		clearLCD(); // czysci lcd
		*menuStartingPoint -= 1; // menu zostaje przesuniete

		// Menu na nowo wypisane
		setCursor(0, 0);
		for (int i = 0; i < 4; i++) {
			setCursor(i, 1);
			printText(menu[*menuStartingPoint + i]);
		}

		// Ustawienie znacznika wyboru
		setCursor(0, 0);
		printChar(CHOICE);
		//info = 1;
	}
	// Przesuniecie menu w gore
	else if (*menuUpDown > *menuStartingPoint + 3) {
		clearLCD(); // czysci lcd
		*menuStartingPoint += 1; // menu zostaje przesuniete

		// Menu na nowo wypisane
		setCursor(0, 0);
		for (int i = 0; i < 4; i++) {
			setCursor(i, 1);
			printText(menu[*menuStartingPoint + i]);
		}

		// Ustawienie znacznika wyboru
		setCursor(3, 0);
		printChar(CHOICE);
		//info = 2;
	}
	// Przesuniecie wskaznika opcji
	else {
		position = *menuUpDown - *menuStartingPoint; // nowa pozycja wskaznika opcji

		// Ustawienie znacznika wyboru
		setCursor(position, 0);
		printChar(CHOICE);
		//info = 3;
	}

	// Oczekiwanie na wypuszczenie joysticka do pozycji 0
	while (joystickState != 0) {
		joystickState = Joystick_State();
	}
}

void printMenu(char *menu[], int menuLen, int startingPoint) {

	setCursor(0, 0);
	printChar(CHOICE);

	int endPoint = 4;

	if (menuLen < endPoint)
		endPoint = menuLen;

	for (int i = 0; i < endPoint; i++) {
		setCursor(i, 1);
		printText(menu[startingPoint + i]);
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

