#include "joystick.h"

void joystickMoved(int *menu, int joystickState) {
	HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, RESET);
	HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, RESET);
	HAL_GPIO_WritePin(LED_Blue_GPIO_Port, LED_Blue_Pin, RESET);
	HAL_GPIO_WritePin(LED_Orange_GPIO_Port, LED_Orange_Pin, RESET);

	switch (joystickState) {
	case 1: // gora
		HAL_GPIO_WritePin(LED_Blue_GPIO_Port, LED_Blue_Pin, SET);
		*menu += 1;
		break;
	case 2: // dol
		HAL_GPIO_WritePin(LED_Orange_GPIO_Port, LED_Orange_Pin, SET);
		if (menu != 0)
			*menu -= 1;
		break;
	case 3: //prawo
		HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, SET);
		break;
	case 4: // lewo
		HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, SET);
		break;
	default:
		HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, RESET);
		HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, RESET);
		HAL_GPIO_WritePin(LED_Blue_GPIO_Port, LED_Blue_Pin, RESET);
		HAL_GPIO_WritePin(LED_Orange_GPIO_Port, LED_Orange_Pin, RESET);
	}

	setCursor(0, 15);
	printNum(*menu);

	// Oczekiwanie na wypuszczenie joysticka do pozycji 0, zapobiega ciaglemu odpytywaniu i przebiegu while(1)
	while (joystickState != 0) {
		joystickState = Joystick_State();
	}
}

//Odczyt stanu joysticka
uint8_t Joystick_State() {

	//Góra
	if (Joystick[0] < 300) {
		return 1;
	}
	//Dół
	else if (Joystick[0] > 3700) {
		return 2;
	}
	//Prawo
	else if (Joystick[1] > 3700) {
		return 3;
	}
	//Lewo
	else if (Joystick[1] < 300) {
		return 4;
	}
	//Neutralne
	else {
		return 0;
	}
}
