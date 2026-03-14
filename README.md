# Thermocouple Temperature Logger

Projekt sprzętowo-programowy precyzyjnego wielokanałowego rejestratora temperatury. Urządzenie służy do odczytu, wyświetlania oraz logowania danych pomiarowych z maksymalnie 8 termopar.

## 📌 Opis Projektu

Głównym zadaniem układu jest akwizycja danych z czujników temperatury za pomocą dedykowanego układu **LTC2983**, a następnie ich przetwarzanie na mikrokontrolerze **STM32H5**. System oferuje możliwość lokalnego zapisu pomiarów na kartę pamięci lub przesyłania ich do komputera PC za pośrednictwem sieci Ethernet w popularnym standardzie automatyki pomiarowej (SCPI / VISA).

Projekt jest obecnie w fazie rozwoju i obejmuje zarówno projekt elektroniki (PCB), jak i oprogramowania układowego (Firmware).

## ✨ Główne funkcjonalności

* **Precyzyjny pomiar:** Odczyt temperatur z 8 niezależnych termopar przy użyciu specjalizowanego układu LTC2983.
* **Logowanie danych:** Zapis historii pomiarów bezpośrednio na kartę pamięci MicroSD.
* **Komunikacja z PC:** Zdalny odczyt i sterowanie przez interfejs Ethernet z wykorzystaniem protokołu **VISA / SCPI**.
* **Podgląd na żywo:** Wyświetlanie aktualnych temperatur oraz stanu urządzenia na monochromatycznym ekranie LCD/OLED.
* **Interfejs użytkownika (HMI):** Wygodne sterowanie za pomocą enkodera obrotowego oraz dedykowanych przycisków umieszczonych pod wyświetlaczem.

## 🛠️ Architektura Sprzętowa (Hardware)

* **Mikrokontroler:** Rodzina STM32H5 (ARM Cortex-M33)
* **Układ pomiarowy:** Analog Devices LTC2983 (Multi-Sensor High Accuracy Digital Temperature Measurement System)
* **Wyświetlacz:** Monochromatyczny LCD / OLED
* **Interfejsy:** 
    * Gniazdo Ethernet (RJ45)
    * Slot na kartę pamięci uSD
    * Złącza dla 8 termopar
* **Sterowanie lokalne:** Enkoder obrotowy z przyciskiem + przyciski funkcyjne pod ekranem.

## 💻 Oprogramowanie (Software)

* **System operacyjny (RTOS):** Eclipse ThreadX
* **Stos sieciowy:** Obsługa Ethernet do komunikacji z systemami nadrzędnymi.
* **Protokół komunikacyjny:** SCPI (Standard Commands for Programmable Instruments).
* **System plików:** Obsługa FAT/exFAT dla karty SD.

## 🚀 Status projektu

Projekt jest w trakcie realizacji. Obecnie trwają prace nad:
- [ ] Konfiguracją peryferiów dla STM32H5
- [ ] Opracowaniem schematu i projektu PCB
- [ ] Integracją systemu operacyjnego ThreadX
- [ ] Napisaniem sterownika do obsługi LTC2983
- [ ] Implementacją parsera komend SCPI
- [ ] Opracowaniem UI

## ➗ Podział prac
 - TODO || Zakładany podział dynamiczny podczas realizacji projektu.

---
*Autor: Mateusz Gąsiorowski && Jakub Konior*