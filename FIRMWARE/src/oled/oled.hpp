/* @file    oled.hpp
 * @brief   
 *
 * @author  Jakub Konior
 * @date    2026-03-20
 *
 * @details
 *
 */

#pragma once
#include <stdint.h>

class OLED {
public:
    void init();
    void sendCommand(uint8_t cmd);
    void sendData(uint8_t data);
    void print(const char* text);
};