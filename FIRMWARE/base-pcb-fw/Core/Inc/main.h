/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32h5xx_hal.h"

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
#define SD_Detect_Pin GPIO_PIN_13
#define SD_Detect_GPIO_Port GPIOC
#define HW_ID0_Pin GPIO_PIN_0
#define HW_ID0_GPIO_Port GPIOF
#define HW_ID1_Pin GPIO_PIN_1
#define HW_ID1_GPIO_Port GPIOF
#define HW_ID2_Pin GPIO_PIN_2
#define HW_ID2_GPIO_Port GPIOF
#define HW_ID3_Pin GPIO_PIN_3
#define HW_ID3_GPIO_Port GPIOF
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define USER_LED0_Pin GPIO_PIN_7
#define USER_LED0_GPIO_Port GPIOE
#define USER_LED1_Pin GPIO_PIN_8
#define USER_LED1_GPIO_Port GPIOE
#define USER_LED2_Pin GPIO_PIN_9
#define USER_LED2_GPIO_Port GPIOE
#define USER_BUTTON1_Pin GPIO_PIN_10
#define USER_BUTTON1_GPIO_Port GPIOE
#define USER_BUTTON2_Pin GPIO_PIN_11
#define USER_BUTTON2_GPIO_Port GPIOE
#define USER_BUTTON3_Pin GPIO_PIN_12
#define USER_BUTTON3_GPIO_Port GPIOE
#define USER_BUTTON4_Pin GPIO_PIN_13
#define USER_BUTTON4_GPIO_Port GPIOE
#define USER_BUTTON5_Pin GPIO_PIN_14
#define USER_BUTTON5_GPIO_Port GPIOE
#define USER_BUTTON6_Pin GPIO_PIN_15
#define USER_BUTTON6_GPIO_Port GPIOE
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define Encoder_CH1_Pin GPIO_PIN_6
#define Encoder_CH1_GPIO_Port GPIOC
#define Encoder_BUTTON_Pin GPIO_PIN_8
#define Encoder_BUTTON_GPIO_Port GPIOC
#define LTC_RST_Pin GPIO_PIN_4
#define LTC_RST_GPIO_Port GPIOD
#define LTC_INTERRUPT_Pin GPIO_PIN_5
#define LTC_INTERRUPT_GPIO_Port GPIOD
#define SPI1_CS_Pin GPIO_PIN_6
#define SPI1_CS_GPIO_Port GPIOD
#define Encoder_CH2_Pin GPIO_PIN_5
#define Encoder_CH2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define USER_LED1_Pin GPIO_PIN_8
#define USER_LED1_GPIO_Port GPIOE
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
