#include "lcd_i2c.hpp"
#include <cstdarg>
#include <cstdio>

#define PIN_RS  0
#define PIN_RW  1
#define PIN_E   2
#define PIN_BL  3
#define PIN_D4  4
#define PIN_D5  5
#define PIN_D6  6
#define PIN_D7  7

#define MASK_BL (1 << PIN_BL)

static const uint8_t row_offsets[] = {0x00, 0x40};

static const uint8_t common_addrs[] = {0x27, 0x3F, 0x20, 0x38, 0x21, 0x3E};

LCD_I2C::LCD_I2C(I2C_HandleTypeDef* hi2c, uint8_t address)
    : _hi2c(hi2c), _addr(address << 1), _bl_mask(MASK_BL)
{
}

bool LCD_I2C::begin()
{
    _addr = _addr >> 1;

    bool found = false;
    for (auto a : common_addrs)
    {
        if (HAL_I2C_IsDeviceReady(_hi2c, a << 1, 1, 50) == HAL_OK)
        {
            _addr = a;
            found = true;
            break;
        }
    }

    if (!found)
    {
        for (uint32_t trial = 0; trial < 10; trial++)
        {
            if (HAL_I2C_IsDeviceReady(_hi2c, _addr << 1, 1, 50) == HAL_OK)
            {
                found = true;
                break;
            }
            HAL_Delay(10);
        }
    }

    if (!found)
        return false;

    _addr = _addr << 1;
    _bl_mask = MASK_BL;
    initSeq();
    return true;
}

void LCD_I2C::delay_us(uint32_t us)
{
    for (uint32_t i = 0; i < us * 64; i++)
        __NOP();
}

void LCD_I2C::pulseEnable(uint8_t data)
{
    uint8_t out = data | (1 << PIN_E);
    HAL_I2C_Master_Transmit(_hi2c, _addr, &out, 1, 100);
    delay_us(1);

    out = data & ~(1 << PIN_E);
    HAL_I2C_Master_Transmit(_hi2c, _addr, &out, 1, 100);
    delay_us(1);
}

void LCD_I2C::writeNibble(uint8_t nibble, bool rs)
{
    uint8_t data = _bl_mask;
    if (rs)
        data |= (1 << PIN_RS);

    if (nibble & 0x01) data |= (1 << PIN_D4);
    if (nibble & 0x02) data |= (1 << PIN_D5);
    if (nibble & 0x04) data |= (1 << PIN_D6);
    if (nibble & 0x08) data |= (1 << PIN_D7);

    pulseEnable(data);
}

void LCD_I2C::writeByte(uint8_t data, bool rs)
{
    writeNibble(data >> 4, rs);
    writeNibble(data & 0x0F, rs);
}

void LCD_I2C::command(uint8_t cmd)
{
    writeByte(cmd, false);
    delay_us((cmd <= 0x03) ? 1640 : 50);
}

void LCD_I2C::initSeq()
{
    HAL_Delay(50);

    writeNibble(0x03, false);
    delay_us(5000);
    writeNibble(0x03, false);
    delay_us(5000);
    writeNibble(0x03, false);
    delay_us(200);
    writeNibble(0x02, false);
    delay_us(100);

    command(0x28);
    command(0x08);
    command(0x01);
    delay_us(2000);
    command(0x06);
    command(0x0C);

    HAL_Delay(10);
}

void LCD_I2C::clear()
{
    command(0x01);
    delay_us(2000);
}

void LCD_I2C::home()
{
    command(0x02);
    delay_us(2000);
}

void LCD_I2C::setCursor(uint8_t col, uint8_t row)
{
    if (row >= LCD_ROWS) row = LCD_ROWS - 1;
    if (col >= LCD_COLS) col = LCD_COLS - 1;
    command(0x80 | (row_offsets[row] + col));
}

void LCD_I2C::print(const char* str)
{
    while (*str)
    {
        writeByte(*str, true);
        delay_us(50);
        str++;
    }
}

void LCD_I2C::printf(const char* fmt, ...)
{
    char buf[LCD_COLS + 1];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    print(buf);
}

void LCD_I2C::backlight(bool on)
{
    _bl_mask = on ? MASK_BL : 0;
    uint8_t data = _bl_mask;
    HAL_I2C_Master_Transmit(_hi2c, _addr, &data, 1, 100);
}
