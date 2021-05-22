#ifndef INC_JOYSTICK_H_
#define INC_JOYSTICK_H_

#include "stm32f4xx_hal.h"
#include "main.h"

/* zamiast dawac extern mozna przekazac joystick[] przez wskaznik,
 ale na jedno wyjdzie skoro uzywamy tylko jednego joysticka*/
extern uint16_t Joystick[2];

void joystickMoved(int *menu, int joystickState);
uint8_t Joystick_State();

#endif /* INC_JOYSTICK_H_ */
