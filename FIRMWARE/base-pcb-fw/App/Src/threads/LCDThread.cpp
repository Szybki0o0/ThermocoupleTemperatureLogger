#include "i2c.h"
#include "threads/LCDThread.hpp"
#include "lcd_i2c.hpp"
#include <cstdio>

void LCDThread::entry(ULONG arg) {
    uint8_t col = 0;
    uint8_t row = 0;
    LCD_I2C lcd(&hi2c1, 0x27);
    if (lcd.begin())
    {
        lcd.print("LCD OK");
        tx_thread_sleep(sleepTicks);
        lcd.clear();
    }

    lcd.setCursor(col,row);
    for(int i = 1;i <= 8;i++) {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "T%d", i);
        lcd.print(buffer);
        lcd.setCursor(col+4,row);
        col += 4;
        if(i == 4) {
            row = 1;
            col = 0;
            lcd.setCursor(col,row);
        }
    } 
}

