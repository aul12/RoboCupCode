# Bodenseehaie - Programme

## Dateiübersicht

### AtXMega

[AtXMega/main.cpp](AtXMega/main.cpp): Maindatei: Initialisierung und Programmverwaltung


[AtXMega/interrupts.cpp](AtXMega/interrupts.cpp): Timerinterrupt und Interrupt für Peripherie (UART, Twi)

[AtXMega/defines.h](AtXMega/defines.h): Konstante Programmparameter und präprozessor Konfigurationen


[AtXMega/global.hpp](AtXMega/global.hpp): Globale Variablen


[AtXMega/spiel.hpp](AtXMega/spiel.hpp): Spiellogiken (Soccer A, B, Superfield, Torwart)


[AtXMega/driver](AtXMega/driver): Treiber für Hardware-Busprotokolle (Twi, UART, SPI)


[AtXMega/sensor](AtXMega/sensor): Klassen für I²C Sensoren


[AtXMega/libs](AtXMega/libs): Sonstige Bibliotheken (I/O, PWM, ADC, Kommunikation zur Linienplatine, Trigonometrische Funktionen)


### AtMega48 (Linienplatine)

[Bodenplatine/main.c](Bodenplatine/main.c): Maindatei: Initialisierung, Programmlogik und SPI


[Bodenplatine/uart.c](Bodenplatine/uart.c): Treiber für die UART-Schnittstelle zur Kommunikation mit der Hauptplatine


[Bodenplatine/twimaster.c](Bodenplatine/twimaster.c): Treiber für das I²C-Interface für das Display


[Bodenplatine/stddefs.h](Bodenplatine/stddefs.h): Konstante Definitionen und globale Variablen


## Fuses
Auf jeden Xmega bei erster Benutzung brennen (Tools->Device Programming):

**FF;00;FF;FF;F2**
## LEDs
LED 1: Watchdog
	
LED 2: Brownout
	
LED 1 und 2: Software Reset
	
LED 3: Auf Linie oder Außerhalb
	
LED 4: Ballda
	
LED 5: -
	
LED 6: Spannung kleiner 11V
	
LED 7: Kompass startet (Wenn länger 1 sek dann Fehler)
	
LED 8: Kompass kalibriert

## Schalter
Schalter 1: Motoren

Schalter 2 und 4: Spielprogramm:

| Schalter 2 | Schalter 4  | Programm                                    |
|:----------:|:-----------:|---------------------------------------------|
| 0          | 0           | Normales Spielprogramm ohne Zusatzfunktionen|
| 1          | 0           | Spielprogramm mit Zusatzfunktionen          |
| 0          | 1           | Torwartprogramm (nicht getestet)            |
| 1          | 1           | PID Test-Programm                           |

Schalter 3: Display-Ausgabe an Aus (wenn Motoren aus auch USB)

Schalter 5: Aktiv-Passiv (wenn an dann Aktiv)

Schalter 6: Linie kalibrieren (Roboter über Linie ziehen, Roboter nimmt höchsten und tiefsten Wert, Schwelle in der Mitte)

## PDI Stecker mit Kabel vom jtag ice 3
	______
	|3	4| 
	 X	X|
	|6	2|
	------

## Mögliche Fehler
- Der Roboter fährt schlecht/gar nicht an und fährt nicht zurück vor das Tor (IR Werte am Display checken, Erdung, US checken)
- Der Roboter fährt komisch/langsam (Ein Motor funktioniert nicht, ADC Werte schlecht kalibriert, Akku leer)
- Der Roboter verdreht sich mit der Zeit (Kompass funktioniert nicht, nicht ordentlich kalibriert)
- Der Roboter fährt manchmal raus (Neu kalibrieren, Panzertape am Rand nach unten biegen, schauen ob alle LEDs und Sensoren funktionieren)
- Der Roboter reagiert nicht auf die Linie (Kabel überprüfen, Kalibrierung überprüfen)
- Watchdogs / Software Resets (Überprüfen ob alle Spannungen aktiv sind, Sensoren ordentlich einstecken)
- Linienplatine lässt sich nicht programmieren (ein Max-Chip ist defekt)
- Roboter schießt wenn nah am Ball (Lichtschranke defekt, Schuss hängt in Lichtschranke)