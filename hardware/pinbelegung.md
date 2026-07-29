# Pinbelegung

## Bedienelemente

| Komponente | ESP32-Pin |
|---|---:|
| Joystick X | GPIO 1 |
| Joystick Y | GPIO 2 |
| Button Start / Bestätigen | GPIO 4 |
| Button Pause | GPIO 5 |
| Button Zurück | GPIO 6 |

## TFT-Display

Das Display ist über SPI angeschlossen. Für die Datenübertragung werden die
Standard-SPI-Pins des ESP32 verwendet.

| Display-Signal | ESP32-Pin |
|---|---:|
| MOSI | GPIO 23 |
| SCLK | GPIO 18 |
| MISO | GPIO 19, falls verwendet |
| CS | Freier GPIO, in TFT_eSPI festgelegt |
| DC | Freier GPIO, in TFT_eSPI festgelegt |
| RST | Freier GPIO, in TFT_eSPI festgelegt |
| VCC | 3,3 V |
| GND | GND |

Die genaue Zuordnung von CS, DC und RST ist in der verwendeten
TFT_eSPI-Konfiguration hinterlegt.