#include "main.h"
#include "stm32h5xx_hal.h"
#include "i2c.h"
#include "tim.h"
#include "lcd_i2c.hpp"
#include "SystemState.hpp"

SystemState systemState = {};

extern "C" void cpp_main(void)
{
}
