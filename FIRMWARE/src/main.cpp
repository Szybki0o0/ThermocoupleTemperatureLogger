/* @file    main.cpp
 * @brief   Główny plik programu sterującego systemem pomiaru temperatury
 *          z wykorzystaniem termopary i wyświetlacza OLED.
 *
 * @author  Jakub Konior
 * @date    2026-03-20
 *
 * @details
 * Plik zawiera funkcję main(), w której realizowana jest inicjalizacja
 * mikrokontrolera oraz peryferiów (interfejs SPI/I2C, wyświetlacz OLED,
 * układ pomiarowy termopary). W głównej pętli programu wykonywany jest
 * cykliczny odczyt temperatury oraz jej prezentacja na wyświetlaczu.
 */

#include "oled/oled.hpp"

int main() {
    OLED oled;
    oled.init();

    oled.print("Temp: 25C");
    oled.print("Hum: 50%");

    // symulacja odświeżania co sekundę
    while (true) {
        //TODO
    }

    return 0;
}