/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "config.h"
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void leds_blink(uint8_t count, uint16_t interval);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define COL1_Pin GPIO_PIN_0
#define COL1_GPIO_Port GPIOC
#define COL2_Pin GPIO_PIN_1
#define COL2_GPIO_Port GPIOC
#define COL3_Pin GPIO_PIN_2
#define COL3_GPIO_Port GPIOC
#define COL4_Pin GPIO_PIN_3
#define COL4_GPIO_Port GPIOC
#define ROW1_Pin GPIO_PIN_0
#define ROW1_GPIO_Port GPIOA
#define ROW2_Pin GPIO_PIN_1
#define ROW2_GPIO_Port GPIOA
#define ROW3_Pin GPIO_PIN_2
#define ROW3_GPIO_Port GPIOA
#define ROW4_Pin GPIO_PIN_3
#define ROW4_GPIO_Port GPIOA
#define ROW5_Pin GPIO_PIN_4
#define ROW5_GPIO_Port GPIOA
#define ROW6_Pin GPIO_PIN_5
#define ROW6_GPIO_Port GPIOA
#define ROW7_Pin GPIO_PIN_6
#define ROW7_GPIO_Port GPIOA
#define ROW8_Pin GPIO_PIN_7
#define ROW8_GPIO_Port GPIOA
#define COL5_Pin GPIO_PIN_4
#define COL5_GPIO_Port GPIOC
#define COL6_Pin GPIO_PIN_5
#define COL6_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_0
#define KEY1_GPIO_Port GPIOB
#define KEY2_Pin GPIO_PIN_1
#define KEY2_GPIO_Port GPIOB
#define KEY3_Pin GPIO_PIN_2
#define KEY3_GPIO_Port GPIOB
#define KEY11_Pin GPIO_PIN_10
#define KEY11_GPIO_Port GPIOB
#define KEY12_Pin GPIO_PIN_11
#define KEY12_GPIO_Port GPIOB
#define KEY13_Pin GPIO_PIN_12
#define KEY13_GPIO_Port GPIOB
#define KEY14_Pin GPIO_PIN_13
#define KEY14_GPIO_Port GPIOB
#define KEY15_Pin GPIO_PIN_14
#define KEY15_GPIO_Port GPIOB
#define KEY16_Pin GPIO_PIN_15
#define KEY16_GPIO_Port GPIOB
#define COL7_Pin GPIO_PIN_6
#define COL7_GPIO_Port GPIOC
#define COL8_Pin GPIO_PIN_7
#define COL8_GPIO_Port GPIOC
#define HO_UP_Pin GPIO_PIN_8
#define HO_UP_GPIO_Port GPIOC
#define HO_UP_EXTI_IRQn EXTI9_5_IRQn
#define HO_RIGHT_Pin GPIO_PIN_9
#define HO_RIGHT_GPIO_Port GPIOC
#define HO_RIGHT_EXTI_IRQn EXTI9_5_IRQn
#define BL_CTRL_Pin GPIO_PIN_8
#define BL_CTRL_GPIO_Port GPIOA
#define HO_DOWN_Pin GPIO_PIN_10
#define HO_DOWN_GPIO_Port GPIOC
#define HO_DOWN_EXTI_IRQn EXTI15_10_IRQn
#define HO_LEFT_Pin GPIO_PIN_11
#define HO_LEFT_GPIO_Port GPIOC
#define HO_LEFT_EXTI_IRQn EXTI15_10_IRQn
#define KEY0_Pin GPIO_PIN_12
#define KEY0_GPIO_Port GPIOC
#define KEY4_Pin GPIO_PIN_3
#define KEY4_GPIO_Port GPIOB
#define KEY5_Pin GPIO_PIN_4
#define KEY5_GPIO_Port GPIOB
#define KEY6_Pin GPIO_PIN_5
#define KEY6_GPIO_Port GPIOB
#define KEY7_Pin GPIO_PIN_6
#define KEY7_GPIO_Port GPIOB
#define KEY8_Pin GPIO_PIN_7
#define KEY8_GPIO_Port GPIOB
#define KEY9_Pin GPIO_PIN_8
#define KEY9_GPIO_Port GPIOB
#define KEY10_Pin GPIO_PIN_9
#define KEY10_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define KEY_PRESSED 1
#define KEY_RELEASED 0

#define MODIFIER_KEY_FLAG 0x1000
#define CONSUMER_KEY_FLAG 0x2000
#define MOUSE_BUTTON_FLAG 0x4000
#define GAMEPAD_BUTTON_FLAG 0x0800
#define SPECIAL_KEY_FLAG  0x8000

#define LEDS_BLINK_PERIOD_SHORT 200
#define LEDS_BLINK_PERIOD_LONG 500
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
