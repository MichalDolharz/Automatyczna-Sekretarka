#ifndef INC_JOYSTICK_H_
#define INC_JOYSTICK_H_

#include "stm32f4xx_hal.h"
#include "main.h"
#include "lcd_menu.h"

extern uint16_t Joystick[2];

uint8_t Joystick_State();

#endif /* INC_JOYSTICK_H_ */
