#include <cstdio>
extern "C" {
#include "main.h"
}
#include "usart.h"

#include "threads/LedThread.hpp"

void LedThread::entry(ULONG arg)
{
    /* Blink 3x first to prove thread runs */
    for (int i = 0; i < 3; i++) {
        HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
        tx_thread_sleep(50);
        HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
        tx_thread_sleep(50);
    }

    const char msg[] = "Hello World\r\n";
    HAL_StatusTypeDef uart_ret = HAL_UART_Transmit(&huart3, (uint8_t*)msg, sizeof(msg)-1, 1000);
    if (uart_ret == HAL_OK) {
        HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);  /* LD1 ON = UART OK */
    } else {
        while (1) {
            HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
            tx_thread_sleep(sleepTicks);
        }
    }
}