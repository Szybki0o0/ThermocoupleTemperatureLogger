#include "main.h"
#include "stm32h5xx_hal.h"
#include "i2c.h"
#include "lcd_i2c.hpp"

extern "C" void cpp_main(void)
{

    LCD_I2C lcd(&hi2c1, 0x27);
    if (lcd.begin())
    {
        lcd.print("LCD OK");
    }
    else
    {
        lcd = LCD_I2C(&hi2c1, 0x3F);
        if (lcd.begin())
            lcd.print("LCD OK (3F)");
    }

}
