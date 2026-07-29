# Funktionen

Der Arcade-Prototyp besitzt ein festes Menü zur Auswahl der vorhandenen Spiele.

Die Steuerung erfolgt über den Joystick und die Taster. Dadurch können sowohl
die Eingaben als auch die Darstellung auf dem Display im gemeinsamen Betrieb
getestet werden.

## Menü

Das Hauptmenü dient als Ausgangspunkt für alle Funktionen. Es ermöglicht die
Auswahl der Spiele und kann später um weitere Einträge ergänzt werden.

## Spiele

Die vorhandenen Spiele dienen als Funktionstest für unterschiedliche
Bedienabläufe:

- Snake: Steuerung über den Joystick
- Flappy Bird: Eingabe über einen Taster
- Wolken-Sprungspiel: seitliche Steuerung über den Joystick

Highscores werden im Speicher des ESP32 gesichert und bleiben nach einem
Neustart erhalten.