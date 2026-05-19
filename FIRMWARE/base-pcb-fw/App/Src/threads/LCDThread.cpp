#include "i2c.h"
#include "threads/LCDThread.hpp"
#include "lcd_i2c.hpp"

void LCDThread::entry(ULONG arg)
{
    LCD_I2C lcd(&hi2c1, 0x27);

    if (lcd.begin())
    {
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
        lcd.setCursor(2, 0);
        lcd.printf("%d", testvalue);
        lcd.setCursor(6, 0);
        lcd.printf("%d", testvalue);
        lcd.setCursor(10, 0);
        lcd.printf("%d", testvalue);
        lcd.setCursor(14, 0);
        lcd.printf("%d", testvalue);
        
        lcd.setCursor(2, 1);
        lcd.printf("%d", testvalue);
        lcd.setCursor(6, 1);
        lcd.printf("%d", testvalue);
        lcd.setCursor(10, 1);
        lcd.printf("%d", testvalue);
        lcd.setCursor(14, 1);
        lcd.printf("%d", testvalue);

        tx_thread_sleep(50);
    }
}