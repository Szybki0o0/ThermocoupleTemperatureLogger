#include "threads/EncoderThread.hpp"

#include "tim.h"
#include "SystemState.hpp"

void EncoderThread::entry(ULONG arg)
{
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

    __HAL_TIM_SET_COUNTER(&htim3, 0);

    int32_t last = 0;

    while (1)
    {
        int32_t cnt = __HAL_TIM_GET_COUNTER(&htim3);

        if (cnt != last)
        {
            last = cnt;
            systemState.temperature[0] = cnt;
        }

        tx_thread_sleep(5);
    }
}