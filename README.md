# ESP32 Arcade

Ein selbst aufgebauter Arcade-Automat mit ESP32, TFT-Display, Joystick,
vier Tastern, Buzzer und eigenem Gehäuse.

[Foto des finalen Endergebnisses](bilder/02-gesamtaufbau/FINALER-DATEINAME.jpg)

Der Schwerpunkt des Projekts liegt auf dem praktischen Hardwareaufbau:
Komponenten einzeln testen, korrekt verkabeln und in ein selbst
konstruiertes Gehäuse integrieren.

## Hardware und Aufbau

Der ESP32 steuert Display, Eingaben und Buzzer. Vor dem Zusammenbau
wurden die einzelnen Komponenten separat getestet; anschließend entstand
ein vollständiger Breadboard-Aufbau als Grundlage für das Gehäuse.

- [Taster testen](bilder/01-einzeltests/buttons.jpg)
- [Buzzer testen](bilder/01-einzeltests/buzzer.jpg)
- [TFT-Display testen](bilder/01-einzeltests/display.jpg)
- [Joystick testen](bilder/01-einzeltests/joystick.jpg)
- [Gesamtaufbau im Test](bilder/02-gesamtaufbau/gesamtaufbau-test.jpg)
- [Finale Verkabelung](bilder/02-gesamtaufbau/gesamtaufbau-final.jpg)

Weitere technische Informationen:

- [Stückliste](hardware/stückliste.md)
- [Pinbelegung](hardware/pinbelegung.md)
- [Systemaufbau](dokumentation/aufbau.md)

## Gehäuse

Das Gehäuse wurde passend zu Display, Bedienelementen und Elektronik
konstruiert. Die 3D-Modelle dokumentieren den Weg von den Einzelteilen
bis zum zusammengesetzten Gehäuse.

- [Gehäuse: Einzelteile](bilder/04-gehaeuse/3d-einzelteile.png)
- [Gehäuse: Gesamtansicht](bilder/04-gehaeuse/3d-gehaeuse-gesamt.png)

## Bedienung und Software

Die Software stellt ein Menü, die Spielanzeige und die Eingabeverarbeitung
bereit. Sie wurde KI-unterstützt entwickelt und dient vor allem dazu, die
selbst aufgebaute Hardware praktisch zu testen. Deshalb liegt der Fokus
dieser Dokumentation bewusst auf Verkabelung, Komponentenprüfung und
Gehäusekonstruktion.

- [Startbildschirm](bilder/03-menuelogik/01-startbildschirm.jpg)
- [Schwierigkeitswahl](bilder/03-menuelogik/02-schwierigkeitswahl.jpg)
- [Spielbetrieb](bilder/03-menuelogik/03-spielbetrieb.jpg)
- [Spielende](bilder/03-menuelogik/04-verloren.jpg)
- [Menü und Bedienung](dokumentation/funktionen.md)

## Projektziel

Ziel war es, das Zusammenspiel von Mikrocontroller, Ein- und Ausgaben
sowie mechanischer Konstruktion anhand eines eigenen, nutzbaren
Projekts nachvollziehbar umzusetzen.

- [Projektziel](dokumentation/projektziel.md)
