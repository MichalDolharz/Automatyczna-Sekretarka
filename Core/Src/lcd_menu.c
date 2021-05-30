#include "lcd_menu.h"

void menuClicked(int menuNum) {

	switch (menuNum) {
	case 0:
		handleRecording();
		break;
	case 1: // wiadomosci
		handleRecordsMenu();
		break;
	case 2: // zachowane
		handleSavedMenu();
		break;
	case 3: // ustawienia
		break;
	case 4: // informacje
		handleInformationMenu();
		break;
	}

	clearLCD();
	setCursor(0, 0);
}
void handleRecording() {

	int recordNum = -1;

	// Szukanie wolnego miejsca
	for (int i = 0; i <= 9; i++) {
		if (recordStatus[i] == 0) {
			recordNum = i;
			recordStatus[i] = 1;
			break;
		}
	}

	// Informacja o braku miejsca i koniec funkcji (bez nagrania)
	if (recordNum == -1) {
		setCursor(1, 4);
		printText("Brak miejsca.");
		HAL_Delay(3000);
		HAL_GPIO_WritePin(LED_Blue_GPIO_Port, LED_Blue_Pin, RESET);
		return;
	} else {
		setCursor(1, 4);
		printText("Nagrywanie...");
	}

	BSP_AUDIO_IN_Init(DEFAULT_AUDIO_IN_FREQ, DEFAULT_AUDIO_IN_BIT_RESOLUTION,
	DEFAULT_AUDIO_IN_CHANNEL_NBR);

	// Czerwona dioda LED wlaczona
	HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, SET);

	// Nagrywanie
	WavRecordingProcess(recordNum);

	// Koniec nagrywania
	StopRecording();

	// Czerwona dioda LED wylaczona
	HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, RESET);
	//if (++recordsCounter >= MAX_RECORDS) // maksymalnie 10 nagran, REC0 - REC9
	//	recordsCounter = 0;
	clearLCD();
	setCursor(0, 5);
	printText("Zapisano!");
	setCursor(1, 2);
	printText("Numer nagrania:");
	setCursor(2, 9);
	printNum(recordNum);
	HAL_Delay(3000);
}

void handleRecordClicked(int menuNum, char *record) {

	switch (menuNum) {
	case 2: // wiadomosci
		handlePlay(record);
		break;
	case 3: // zachowane
		handleSave(record);
		break;
	case 4: // informacje
		handleRemove(record);
		break;
	}

	clearLCD();
	setCursor(0, 0);
}

void handlePlay(char *record) {
	setCursor(1, 4);
	printText("Odtwarzam");
	HAL_Delay(2000);
}

//
void handleSave(char *record) {
	setCursor(1, 4);
	printText("Zapisuje");
	HAL_Delay(2000);
}

void handleRemove(char *record) {
	char path[] = "/REC/RECn.wav";
	int num = record[9] - '0';
	sprintf(path, "/REC/REC%d.wav", num);
	f_unlink(record);
	recordStatus[num] = 0;
	setCursor(1, 5);
	printText("Usunieto:");
	setCursor(2, 4);
	printText("Nagranie ");
	printNum(num);
	HAL_Delay(3000);
}

void handleRecordsMenuClicked(int menuNum, char *record) {

	char *info[] = { "Powrot", record, " - Odtworz", " - Zachowaj", " - Usun" };

//char tmp;
//sprintf(tmp, "Powrot | rec: %s", record);
//info[0] = tmp;

	int menuUpDown = 0;

	int joystickState = 0;
	int joystickButtonState = 0;
	int *menuStartingPoint = 0;

	setCursor(0, 0);
	printMenu(info, sizeof(info) / sizeof(info[0]), 0);

// Petla trwa dopoki nie wybierze sie opcji "Powrot"
	while (!(menuUpDown == 0 && joystickButtonState == 1)) {

		// Jezeli poruszono joystickiem w dol lub w gore, to aktualizuje menu
		if (joystickState == 1 || joystickState == 2) {
			updateMenu(&info, sizeof(info) / sizeof(info[0]), &menuUpDown,
					&menuStartingPoint, joystickState);
		}

		//Jezeli wcisnieto	przycisk
		if (joystickButtonState) {
			//menuSet = mainMenuUpDown;

			while (joystickButtonState != 0) { // zapobiega ciaglemu przebiegowi while(1)
				joystickButtonState = !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
			}

			clearLCD();
			// akcja
			handleRecordClicked(menuUpDown, record);

			// reset:
			handleRecordsMenu();
			menuStartingPoint = 0;             // zresetowanie punktu startowego
			menuUpDown = 0;         // zresetowanie aktualnie wybranej opcji
		}

		joystickState = Joystick_State(); // poruszenie joystickiem
		joystickButtonState = (!HAL_GPIO_ReadPin(Joystick_Button_GPIO_Port,
		Joystick_Button_Pin)); // wcisniecie joysticka
	}
}

void handleRecordsMenu() {
	char *info[11]; // = { "Powrot", "wolne miejsce", "wolne miejsce", "wolne miejsce", "wolne miejsce", "wolne miejsce", "wolne miejsce", "wolne miejsce", "wolne miejsce", "wolne miejsce", "wolne miejsce"};
	char tmp[11][20];

	strcpy(tmp[0], "Powrot");
	info[0] = tmp[0];

	for (int i = 0; i <= 9; i++) {
		if (recordStatus[i] == 1) {
			sprintf(tmp[i + 1], "Nagranie %d", i);
		} else {
			sprintf(tmp[i + 1], "Wolne miejsce");
		}
		info[i + 1] = tmp[i + 1];
	}

	int menuUpDown = 0;

	int joystickState = 0;
	int joystickButtonState = 0;
	int *menuStartingPoint = 0;

	setCursor(0, 0);
	printMenu(info, sizeof(info) / sizeof(info[0]), 0);

// Petla trwa dopoki nie wybierze sie opcji "Powrot"
	while (!(menuUpDown == 0 && joystickButtonState == 1)) {

		// Jezeli poruszono joystickiem w dol lub w gore, to aktualizuje menu
		if (joystickState == 1 || joystickState == 2) {
			updateMenu(&info, sizeof(info) / sizeof(info[0]), &menuUpDown,
					&menuStartingPoint, joystickState);
		}

		// Jezeli wcisnieto przycisk
		if (joystickButtonState) {
			//menuSet = mainMenuUpDown;

			while (joystickButtonState != 0) { // zapobiega ciaglemu przebiegowi while(1)
				joystickButtonState = !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
			}

			// brak reakcji na opcję "wolne miejsce"
			if (info[menuUpDown][0] != 'W') {
				clearLCD();
				// akcja
				handleRecordsMenuClicked(menuUpDown, info[menuUpDown]);

				// reset:
				handleRecordsMenu();
				menuStartingPoint = 0; // zresetowanie punktu startowego
				menuUpDown = 0; // zresetowanie aktualnie wybranej opcji
			}
		}

		joystickState = Joystick_State(); // poruszenie joystickiem
		joystickButtonState = (!HAL_GPIO_ReadPin(Joystick_Button_GPIO_Port,
		Joystick_Button_Pin)); // wcisniecie joysticka
	}
	return;
}

void handleSavedMenu() {
	char *info[] = { "Powrot", "Zachowane 1", "Zachowane 2", "Zachowane 3",
			"Zachowane 4", "Zachowane 5", "Zachowane 6", "Zachowane 7",
			"Zachowane 8", "Zachowane 9", "Zachowane 10" };

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

void checkFiles() {
// Przeszukiwanie folderu
	FRESULT fresult;
	DIR dir;
	FILINFO filinfo;
	char *path = "/REC";
	int num;
	int licznik = 0;

// Otworzenie katalogu
	fresult = f_opendir(&dir, path);
	if (fresult != FR_OK) {
		return;
	}

// Zapełnienie zerami
	for (int i = 0; i <= 9; i++) {
		recordStatus[i] = 0;
	}

// maksymalnie moze byc 10 nagran
	for (int i = 0; i <= 9; i++) {

		fresult = f_readdir(&dir, &filinfo);
		if (fresult != FR_OK || filinfo.fname[0] != 'R') { // musza zaczynac sie od litery R (REC)
			break;
		}

		num = filinfo.fname[3] - '0'; // konwersja char do int
		recordStatus[num] = 1; // oznaczenie, ze nagranie numer num istnieje

	}

	f_closedir(&dir);
// koniec przeszukiwania folderu
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
		clearLCD();              // czysci lcd
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
		clearLCD();              // czysci lcd
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
