/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
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

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "threads/LedThread.hpp"
#include "threads/LCDThread.hpp"
#include "threads/EncoderThread.hpp"
//#include "threads/EthThread.hpp"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define THREAD_STACK_SIZE 1024
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t thread_stack1[THREAD_STACK_SIZE];
uint8_t thread_stack2[THREAD_STACK_SIZE];
uint8_t thread_stack3[THREAD_STACK_SIZE];
uint8_t thread_stack4[THREAD_STACK_SIZE];
TX_THREAD ledThread;
TX_THREAD lcdThread;
TX_THREAD encThread;
//TX_THREAD ethThread;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  /* USER CODE BEGIN App_ThreadX_MEM_POOL */
  /* USER CODE END App_ThreadX_MEM_POOL */
  /* USER CODE BEGIN App_ThreadX_Init */
  tx_thread_create(&ledThread,(CHAR*)"LED",LedThread::entry,0x1234,thread_stack1,THREAD_STACK_SIZE,15,15,1,TX_AUTO_START);
  tx_thread_create(&lcdThread,(CHAR*)"LCD",LCDThread::entry,0x1234,thread_stack2,THREAD_STACK_SIZE,15,15,1,TX_AUTO_START);
  tx_thread_create(&encThread,(CHAR*)"ENC",EncoderThread::entry,0x1234,thread_stack3,THREAD_STACK_SIZE,15,15,1,TX_AUTO_START);
  //  //tx_thread_create(&ethThread,(CHAR*)"ETH",EthThread::entry,0x5678,thread_stack4,THREAD_STACK_SIZE,15,15,1,TX_AUTO_START);
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN Before_Kernel_Start */

  /* USER CODE END Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN Kernel_Start_Error */

  /* USER CODE END Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
