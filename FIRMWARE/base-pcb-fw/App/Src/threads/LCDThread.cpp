#include "i2c.h"
#include "tim.h"
#include "threads/LCDThread.hpp"
#include "lcd_i2c.hpp"
#include "SystemState.hpp"

void LCDThread::entry(ULONG arg)
{
    if (hi2c1.Instance->ISR & I2C_ISR_BUSY) {
        HAL_I2C_DeInit(&hi2c1);
        HAL_I2C_Init(&hi2c1);
    }
    LCD_I2C lcd(&hi2c1, 0x27);

    if (lcd.begin()) {
        lcd.print("LCD OK");
        tx_thread_sleep(sleepTicks);
        lcd.clear();
    }

    for(uint8_t i = 1; i <= 8; i++) {
         uint8_t col = ((i - 1) % 4) * 4;
         uint8_t row = (i - 1) / 4;
         lcd.setCursor(col, row);
         lcd.printf("T%d", i);
    }

    while(1) {
        for (int i = 0; i < 8; i++)
        {
            lcd.setCursor(2 + (i % 4) * 4, i / 4);
            lcd.printf("%d", systemState.temperature[i]);
        }

        tx_thread_sleep(50);
    }
}
