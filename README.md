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

* **Mikrokontroler:** Rodzina STM32H563ZI (ARM Cortex-M33)
* **Układ pomiarowy:** Analog Devices LTC2983 (Multi-Sensor High Accuracy Digital Temperature Measurement System)
* **Wyświetlacz:** Monochromatyczny LCD / OLED
* **Interfejsy:** 
    * Gniazdo Ethernet (RJ45)
    * Slot na kartę pamięci uSD
    * Konfigurowalny panel przedni do podłączenia czujników temperatory
* **Sterowanie lokalne:** Enkoder obrotowy z przyciskiem + przyciski funkcyjne pod ekranem.
* **Zasilanie:** Zasilacz 230VAC → 5VDC-12VDC w postaci gotowego modułu na PCB lub wykorzystanie zasilania przez gniazdo DC.

## 💻 Oprogramowanie (Software)

* **System operacyjny (RTOS):** Eclipse ThreadX
* **Stos sieciowy:** Obsługa Ethernet do komunikacji z systemami nadrzędnymi.
* **Protokół komunikacyjny:** SCPI (Standard Commands for Programmable Instruments).
* **System plików:** Obsługa FAT/exFAT dla karty SD.

## 🚀 Status projektu

Hardware:
- [x] Konfiguracją peryferiów dla STM32H5
- [X] Schemat
- [X] projekt PCB
- [ ] Montaż PCB
- [ ] Uruchomienie PCB

Software:
- [ ] Integracją systemu operacyjnego ThreadX
- [ ] Napisaniem sterownika do obsługi LTC2983
- [ ] Implementacją parsera komend SCPI
- [ ] Opracowaniem UI

## ➗ Podział prac
- Mateusz Gąsiorowski: 
  - Konfiguracja Peryferiów STM32
  - Schemat
  - Projekt PCB
 
- Jakub Konior:
  - TODO

## Link do dokumentacji
https://aghedupl-my.sharepoint.com/:w:/g/personal/jakonior_student_agh_edu_pl/IQCC4sxuc3X9QIpdb9umY-btAZW3T9In2jfk0HmtnqKJi5U?e=CjhGaD

---
*Autorzy: Mateusz Gąsiorowski && Jakub Konior*