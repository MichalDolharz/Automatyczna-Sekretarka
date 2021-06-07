/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_E_Pin GPIO_PIN_13
#define LCD_E_GPIO_Port GPIOC
#define LCD_RS_Pin GPIO_PIN_15
#define LCD_RS_GPIO_Port GPIOC
#define VBUS_Pin_Pin GPIO_PIN_0
#define VBUS_Pin_GPIO_Port GPIOC
#define Joystick_Button_Pin GPIO_PIN_1
#define Joystick_Button_GPIO_Port GPIOA
#define Joystick_Button_EXTI_IRQn EXTI1_IRQn
#define ADC1_6_VRx_Pin GPIO_PIN_6
#define ADC1_6_VRx_GPIO_Port GPIOA
#define ADC1_7_VRy_Pin GPIO_PIN_7
#define ADC1_7_VRy_GPIO_Port GPIOA
#define LED_Green_Pin GPIO_PIN_12
#define LED_Green_GPIO_Port GPIOD
#define LED_Orange_Pin GPIO_PIN_13
#define LED_Orange_GPIO_Port GPIOD
#define LED_Red_Pin GPIO_PIN_14
#define LED_Red_GPIO_Port GPIOD
#define LED_Blue_Pin GPIO_PIN_15
#define LED_Blue_GPIO_Port GPIOD
#define AUDIO_RST_Pin GPIO_PIN_4
#define AUDIO_RST_GPIO_Port GPIOD
#define LCD_D7_Pin GPIO_PIN_6
#define LCD_D7_GPIO_Port GPIOD
#define LCD_D6_Pin GPIO_PIN_3
#define LCD_D6_GPIO_Port GPIOB
#define LCD_D5_Pin GPIO_PIN_5
#define LCD_D5_GPIO_Port GPIOB
#define LCD_D4_Pin GPIO_PIN_7
#define LCD_D4_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
