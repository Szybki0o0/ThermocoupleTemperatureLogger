extern "C" {
#include "main.h"
}

#include "threads/LedThread.hpp"

void LedThread::entry(ULONG arg)
{
    while (1)
    {
        HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);

        tx_thread_sleep(sleepTicks);
    }
}