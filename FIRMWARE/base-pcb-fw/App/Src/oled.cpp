/* @file    oled.cpp
 * @brief   
 *
 * @author  Jakub Konior
 * @date    2026-03-20
 *
 * @details
 *
 */

#include "oled.hpp"
#include <stdio.h> // only PC
#include <chrono>
#include <thread>

// only PC
void writeSPI(uint8_t data) {
    printf("SPI: 0x%02X\n", data);
}

#define DC_PIN 1
#define CS_PIN 2

// only PC
void gpio_write(uint8_t pin, bool value) {
    printf("GPIO pin %d = %d\n", pin, value);
}

// only PC
void delay_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void OLED::sendCommand(uint8_t cmd) {
    gpio_write(DC_PIN, 0);
    gpio_write(CS_PIN, 0);
    writeSPI(cmd);
    gpio_write(CS_PIN, 1);
}

void OLED::sendData(uint8_t data) {
    gpio_write(DC_PIN, 1);
    gpio_write(CS_PIN, 0);
    writeSPI(data);
    gpio_write(CS_PIN, 1);
}

void OLED::print(const char* text) {
    while (*text) sendData(*text++);
}

void OLED::init() {
    delay_ms(50);
    sendCommand(0x28); // przykładowa inicjalizacja
    sendCommand(0x0C);
    sendCommand(0x01);
    delay_ms(2);
}