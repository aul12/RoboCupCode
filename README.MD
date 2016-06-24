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


##Fuses
Auf jeden Xmega bei erster Benutzung brennen (Tools->Device Programming):
FF;00;FF;FF;F2
##LEDs
LED 1: Watchdog
	
LED 2: Brownout
	
LED 1 und 2: Software Reset
	
LED 3: Auf Linie oder Außerhalb
	
LED 4: Ballda
	
LED 5: -
	
LED 6: Spannung kleiner 11V
	
LED 7: Kompass startet (Wenn länger 1 sek dann Fehler)
	
LED 8: Kompass kalibriert
##PDI Stecker mit Kabel vom jtag ice 3
 	____
______	
	|3	4|
	 
	 X	X|
	
	|6	2|
	 
	----------