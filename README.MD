# Bodenseehaie - Programme
## Dateiübersicht
### AtXMega
[AtXMega/main.cpp](AtXMega/main.cpp): Maindatei: Initialisierung und Programmverwaltung

[AtXMega/interrupts.cpp](AtXMega/interrupts.cpp): Timerinterrupt und Interrupt für Peripherie (UART, Twi)

[AtXMega/main.cpp](AtXMega/defines.h): Konstante Programmparameter und präprozessor Konfigurationen

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

