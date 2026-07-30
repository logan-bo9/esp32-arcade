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

| Display-Signal | ESP32-Pin / Anschluss |
|---|---|
| VCC | 3,3 V |
| GND | GND |
| CS | GPIO 10 |
| RST | GPIO 9 |
| DC | GPIO 8 |
| SDI / MOSI | GPIO 11 |
| SCK | GPIO 12 |
| SDO / MISO | GPIO 13 |
| LED | 3,3 V |

Das Display ist über SPI angeschlossen. Die LED-Leitung versorgt die
Hintergrundbeleuchtung des Displays mit 3,3 V.
