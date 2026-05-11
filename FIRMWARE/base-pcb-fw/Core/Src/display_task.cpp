#include "oled.hpp"
#include "app_threadx.h"

extern "C" void display_thread_entry(ULONG input)
{
    extern I2C_HandleTypeDef hi2c1;

    OLED oled(&hi2c1, 0x3C);

    oled.init();

    while (1)
    {
        oled.clear();
        oled.drawString(4, 8, "ThreadX OLED");
        oled.drawString(10, 24, "Hello!");
        oled.drawString(6, 48, "World");
        oled.update();

        tx_thread_sleep(200);

        oled.clear();
        oled.drawString(4, 8, "Display OK");
        oled.drawString(14, 32, "STM32H5");
        oled.drawString(8, 48, "SSD1306 I2C");
        oled.update();

        tx_thread_sleep(200);
    }
}
