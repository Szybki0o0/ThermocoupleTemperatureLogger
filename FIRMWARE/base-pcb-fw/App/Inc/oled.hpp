#ifndef OLED_HPP
#define OLED_HPP

#include "stm32h5xx_hal.h"

#define OLED_WIDTH  128
#define OLED_HEIGHT 64
#define OLED_COLS   128
#define OLED_PAGES  8
#define OLED_BUF_SIZE (OLED_COLS * OLED_PAGES)

class OLED
{
public:
    OLED(I2C_HandleTypeDef* hi2c, uint8_t address);

    void init();
    void clear();
    void clearBuffer();
    void update();

    void setPixel(uint8_t x, uint8_t y, bool on);
    void drawChar(uint8_t x, uint8_t y, char c);
    void drawString(uint8_t x, uint8_t y, const char* str);

    void sendCommand(uint8_t cmd);
    void sendBuffer();

private:
    I2C_HandleTypeDef* _hi2c;
    uint8_t _addr;
    uint8_t _buf[OLED_BUF_SIZE];
};

#endif
