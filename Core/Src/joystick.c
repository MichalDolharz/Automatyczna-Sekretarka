#include "joystick.h"

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
