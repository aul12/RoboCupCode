// pixie.hpp: C++ Klassensimplementation für Pixie-Kamera via I²C
#ifndef _PIXIE_IMP
#define _PIXIE_IMP
//////////////////
// Definitionen //
//////////////////

#ifndef __cplusplus
#error pixie.hpp must be used from C++-file!
#endif

#include "../driver/twimaster.h"

#define WORD(x,y) (x | (y<<8))

#define PIXY_START_WORD             0xaa55
#define PIXY_START_WORD_CC          0xaa56
#define PIXY_CAM_BRIGHTNESS_SYNC    0xfe
#define PIXY_LED_SYNC               0xfd

//#define _CHECKSUM

#define PIXIE_RESULT_OK				0
#define PIXIE_ERROR_CHECKSUM		1
#define PIXIE_ERROR_SYNC			2
#define PIXIE_ERROR_TWI				3
#define PIXIE_ERROR_CC				4
#define PIXIE_ERROR_OBJECT			5


///////////////////////////
// Klassenimplementation //
///////////////////////////

class Pixie{
	public:
		//Konstruktor
		Pixie(TWI_Master_t* twi, uint8_t addr);
		~Pixie();

		//Datenabfrage via I²C
		uint8_t pollData(void);

		//Kamera-Helligkeit
		void setBrightness(uint8_t brightness);

		//Multicolor-LED (Rot, Grün, Blau)
		void ledColor(uint8_t r, uint8_t g, uint8_t b);

		//Fehlerabfrage
		uint8_t getResult(void);

		//Daten abfrage
		uint16_t getWidth(void);
		uint16_t getHeight(void);
		uint16_t getX(void);
		uint16_t getY(void);
		uint16_t getSignature(void);
		
	private:
		// Variablen
		TWI_Master_t* twi;
		uint8_t addr;
		uint16_t x_pos, y_pos, width, height, checksum, number;
		uint8_t result;
		
		//I²C Wordweise abfrage
		uint16_t getWord(void);
};

#endif