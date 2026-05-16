#ifndef LCD_I2C_HPP
#define LCD_I2C_HPP

#include "stm32h5xx_hal.h"

#define LCD_COLS 16
#define LCD_ROWS 2

class LCD_I2C
{
public:
    LCD_I2C(I2C_HandleTypeDef* hi2c, uint8_t address);

    bool begin();
    void clear();
    void home();
    void setCursor(uint8_t col, uint8_t row);
    void print(const char* str);
    void printf(const char* fmt, ...);
    void backlight(bool on);

    uint8_t detectedAddress() const { return _addr >> 1; }

private:
    void writeNibble(uint8_t nibble, bool rs);
    void writeByte(uint8_t data, bool rs);
    void pulseEnable(uint8_t data);
    void command(uint8_t cmd);
    void initSeq();
    void delay_us(uint32_t us);

    I2C_HandleTypeDef* _hi2c;
    uint8_t _addr;
    uint8_t _bl_mask;
};

#endif
