// atmega8.c: UART-Empfang für Atmega8-Liniensensor

////////////
// Header //
////////////

extern "C" {
	#include <avr/interrupt.h>	// ISR - Interrupt
	#include <math.h>
}
#include "../driver/usart.h"	// UART Kommunikation
#include "../defines.h"			// globale Definitionen
#include "atmega8.hpp"

///////////////
// Variablen //
///////////////

USART_data_t USART_line;			// UART Transmitter
volatile uint8_t linie[32] = { 0 };	// Linienwerte (0 -> grün, 1 -> weiß)
volatile int16_t out_winkel = 0;	// Aus-Winkel (relativ zum Roboter, wie Fahrtrichtung)
volatile uint8_t out = 0;			// Außerhalb des Feldes Zustand (0 -> innerhalb, 1 innere Hälfte, 2 -> äußere Hälfte, 3 -> raus)
volatile uint8_t out_pos1 = 0;		// Position außerhalb (0 -> vorn, 1 -> rechts, 2 -> hinten, 3 -> links)
volatile uint8_t out_pos = 0;		// Position außerhalb (0 -> vorn, 1 -> rechts, 2 -> hinten, 3 -> links)
volatile uint8_t mux = 0;			// Datenempfang -> welches Byte
volatile uint8_t timer_fail = 0;	// Timer-Fail Counter
#if _COMPLEX_LINE==3||_COMPLEX_LINE==4
	volatile int16_t out_winkel_alt = 0;
	volatile int16_t winkel_raus = 0;
	volatile int16_t out_winkel_raus = 0;
	volatile uint8_t out_alt = 0;
	volatile int16_t j = 0; 
	volatile int16_t kk = 0;
	volatile uint8_t platz[32] = { 0 };
	volatile uint8_t line_regler = 0;
#endif



int16_t mWinkel;		//korrigierter Messwinkel
int8_t preRichtung=-1; //erste Linienrichtung
int8_t lRichtung=-1; //Linienrichtung
bool lMuesli=false;		//#ReturnOfZeMüsli
int8_t lEcke=-1;		//aktuelle Ecke (return*90+45)
volatile uint8_t trans=0;		//Transferbyte
/////////////////
// Anmeldungen //
/////////////////

// Kompasswinkel
extern volatile int16_t phi_jetzt;

////////////////
// Funktionen //
////////////////

inline uint8_t richtung(int16_t winkel)
{
	if(winkel>44&&winkel<135) //rechts
	return 1;
	else if(winkel>134&&winkel<225)//hinten
	return 2;
	else if(winkel>224&&winkel<315)//links
	return 3;
	else								   //vorne
	return 0;
}

inline int8_t ecke(int16_t winkel)
{
	int8_t ret=-1;
	if((winkel>29&&winkel<60)||(winkel>209&&winkel<240))//Ecke vorne rechts/hinten links
	ret=0;
	else if((winkel>119&&winkel<150)||(winkel>299&&winkel<330))//Ecke hinten rechts/vorne links
	ret=1;
	if(ret!=-1)//Ecke erkannt
	{
		if(!ret)
		{
			if(preRichtung==2||preRichtung==3)
			ret=2;
		}
		else
		{
			if(!preRichtung||preRichtung==3)
			ret=3;
		}
	}
	return ret;
}

// Rx-Complete -> Byte empfangen
ISR(USARTE1_RXC_vect)
{
	// Daten empfangen
	uint8_t data = USART_line.usart->DATA;
	
	// Umrechnen
	switch(mux) {
		case 0:
			for(uint8_t i=0; i<6; ++i)
				linie[i] = (data >> i) & 1;
			break;
		case 1:
			for(uint8_t i=0; i<7; ++i)
				linie[i+6] = (data >> i) & 1;
			break;
		case 2:
			for(uint8_t i=0; i<6; ++i)
				linie[i+13] = (data >> i) & 1;
			break;
		case 3:
			for(uint8_t i=0; i<7; ++i)
				linie[i+19] = (data >> i) & 1;
			break;
		case 4:
			for(uint8_t i=0; i<6; ++i)
				linie[i+26] = (data >> i) & 1;
			break;
	};
	
	// Fehlerüberprüfung
	if(!((mux == 0 && ((data >> 6) != 0b10)) || (mux == 4 && ((data >> 6) != 0b11)))) {
		// Multiplexer erhöhen
		if(++mux > 4) {
			mux = 0;
		
			// Verarbeiten //
			#if (_COMPLEX_LINE==1||_COMPLEX_LINE==2)
				// Zustände überprüfen
				uint8_t status = 0;
				for(uint8_t i=0; i<32; ++i) {
					if(linie[i])
						status |= 1;
					else
						status |= 2;
				}
				if(status == 1) {
					// alle Sensoren an -> Fehler
					status = 0;
				}
				else if(status == 2) {
					// kein Sensoren an -> passt
					status = 0;
				}
				else if(status == 3) {
					// Linie erkannt -> passt
					status = 1;
				}
		
				// Flächen ordnen
				uint8_t center = 0;
				if(status != 0) {
					// Größte Lücke finden
					uint8_t start = 0;
					uint8_t diff = 0;
					uint8_t diff_tmp = 0;
					for(uint8_t i=0; i<32; ++i) {
						if(!linie[i] && linie[(i-1)&0b11111]) {
							diff_tmp = 1;
							for(uint8_t j=i+1; j<(i+32); ++j) {
								if(!linie[j&0b11111])
									++diff_tmp;
								else
									break;
							}
							if(diff_tmp > diff) {
								diff = diff_tmp;
								start = i;
							}
						}
					}
				
					// Mitte berechnen
					uint8_t center_first = start;
					uint8_t center_second = (start + (diff-1)) & 0b11111;
					if(center_first > center_second) {
						center = center_first;
						center_first = center_second;
						center_second = center;
					}
					if(center_second - center_first > 16) {
						center = (((center_first + center_second) >> 1) + 16) & 0b11111;
					}
					else {
						center = (center_first + center_second) >> 1;
					}
				}
				
				// Aktueller Zustand
				if(out == 0) {
					if(status != 0) { // 1. Linienberührung
						// Winkel berechnen
						out_winkel = -((360 * center + 180) >> 5);
						if(out_winkel < -180)
							out_winkel += 360;
						out = 2;
				
						// Position bestimmen
						int16_t winkel_neu = out_winkel + phi_jetzt;
						if(winkel_neu > 180)
							winkel_neu -= 360;
						else if(winkel_neu < -180)
							winkel_neu += 360;
				
						// Speichern
						if(winkel_neu > -45 && winkel_neu < 45)
							out_pos = 0;
						else if(winkel_neu >= 45 && winkel_neu <= 135)
							out_pos = 3;
						else if(winkel_neu > 135 || winkel_neu < -135)
							out_pos = 2;
						else if(winkel_neu <= -45 && winkel_neu >= -135)
							out_pos = 1;
						out_pos1 = out_pos;
					}
				}
				else {
					// Linie wieder da?
					if(out == 4 && status != 0)
						out = 3;
			
					// Linie weg?
					if(status == 0) {
						if(out == 2) { // von Linie runter gefahren
							out = 0;
							out_pos = 0;
						}
						else { // Außerhalb vom Spielfeld
							out = 4;
						}
					}
			
					// Noch außerhalb
					if(out != 0) {
						// Winkel aktualisieren
						if(out == 4) {
							// Schätzwerte
							out_winkel = -((((int16_t)out_pos)*360) >> 2);
							if(out_winkel < -180)
								out_winkel += 360;
							out_winkel -= phi_jetzt;
							if(out_winkel < -180)
								out_winkel += 360;
							else if(out_winkel > 180)
								out_winkel -= 360;
						}
						else {
							// Neuen Winkel berechnen
							int16_t winkel_neu = -((360 * center + 180) >> 5);
							if(winkel_neu < -180)
								winkel_neu += 360;
					
							// Plausibilitätscheck mit Kompass
							int16_t alpha = winkel_neu + phi_jetzt;
							if(alpha < -180)
								alpha += 360;
							else if(alpha > 180)
								alpha -= 360;
							uint8_t tmp_pos = 0;
							if(alpha > -45 && alpha < 45)
								tmp_pos = 0;
							else if(alpha >= 45 && alpha <= 135)
								tmp_pos = 3;
							else if(alpha > 135 || alpha < -135)
								tmp_pos = 2;
							else if(alpha <= -45 && alpha >= -135)
								tmp_pos = 1;
						
							// Notfalls Drehen
							if(((tmp_pos+2) & 0b11) == out_pos || ((tmp_pos+2) & 0b11) == out_pos1) { // Drehen
								out_winkel = winkel_neu + 180;
								if(out_winkel > 180)
									out_winkel -= 360;
								out = 3;
							}
							else {
								out = 2;
								if(out_pos != tmp_pos) {
									if(BETRAG(out_winkel-winkel_neu) > 90) { // doch Drehen
										out_winkel = winkel_neu + 180;
										if(out_winkel > 180)
											out_winkel -= 360;
										out_pos = (tmp_pos+2) & 0b11;
									}
									else {
										out_winkel = winkel_neu;
										out_pos = tmp_pos;
									}
								}
								else {
									out_winkel = winkel_neu;
								}
							}
						}
					}
				}
			#elif (_COMPLEX_LINE==3||_COMPLEX_LINE==4)
			
				// HIER IST DEINE LOGIK HANNES
			
				out_alt = out;
				out_winkel_alt = out_winkel;
		
				j=0; 
				kk=0;
				out=0;					
				for (int i=0; i<32; i++)
				{
					platz[i]=0;
				}
					
				for(int i=0; i<64; i++) 
				{
					j++;
					kk=i;
					if (linie[(kk>31)?kk-=32:kk]==1)
					{
						platz[(kk>31)?kk-=32:kk]=j;
						j=0;
						out=1;
					}
				}
			
				if (out==0 && out_alt>=2)
				{
					out=3;
					if (out_alt!=3)
					{
						winkel_raus=phi_jetzt;
						out_winkel_raus=out_winkel;
					}					
					out_winkel=out_winkel_raus-winkel_raus+phi_jetzt;
				}
			
				if (out==1)
				{			
					int max=0;
					int ort=0;
					for (int i=0; i<32; i++)
					{
						if(platz[i]>max)
						{
							max=platz[i];
							ort=i;
						}
					}
					line_regler=max;
					out_winkel=(ort - platz[ort]/2.0) * 11.25;
						
					if ( (out_alt>=1) && (BETRAG(out_winkel-out_winkel_alt)>90) && (BETRAG(out_winkel-out_winkel_alt)<270) )//(out_winkel>out_winkel_alt+91 || out_winkel<out_winkel_alt-91)) //möglich wäre schauen welcher näher ist...
					{
						out=2;	
						out_winkel=out_winkel-180;			
					}
					if (out_winkel<-180)
						out_winkel+=360;
				}
			#elif _COMPLEX_LINE==8//#########################
				int16_t sensor_winkel[32]={0}; 	//Sensorpositionen 
			
				for(uint8_t i=0;i<32;i++)
					sensor_winkel[i]=5.625+11.25*i;
			
				//Winkelberechnung
				int16_t maxWinkel=-1;
				int16_t minWinkel=-1;
				for(uint8_t i=0;i<32;i++) //größten Sensorwinkel finden
				{
					if(linie[i])
					{
						if(sensor_winkel[i]>maxWinkel)
						maxWinkel=sensor_winkel[i];
					}
				}
				
				if(maxWinkel==-1)    //Kein Sensor aktiv
				trans=37;		 //Fehlercode
				else
				{
					int16_t winkelDiff=0;   //größte Winkeldifferenz finden und max/minWinkel ggf anpassen
					for(uint8_t c=0;c<2;c++)
					{
						for(uint8_t i=0;i<32;i++)
						{
							if(linie[i])
							{
								int16_t ttempDiff;
								if(!c)
								ttempDiff=BETRAG(maxWinkel-sensor_winkel[i]);
								else
								ttempDiff=BETRAG(minWinkel-sensor_winkel[i]);
								uint8_t tempDiff=ttempDiff <180 ? ttempDiff : 360-ttempDiff;
								if(tempDiff>winkelDiff)
								{
									if(!c)
									minWinkel=sensor_winkel[i];
									else
									maxWinkel=sensor_winkel[i];
									winkelDiff=tempDiff;
								}
							}
						}
						if(minWinkel==-1)  //Nur ein Sensor aktiv
						{
							trans=round(maxWinkel*0.1);
							break;
						}
					}
					
					if(minWinkel!=-1)
					{
						if(maxWinkel<minWinkel) //Werte nach Größe ordnen
						{
							maxWinkel+=minWinkel;
							minWinkel=maxWinkel-minWinkel;
							maxWinkel-=minWinkel;
						}
						winkelDiff=maxWinkel-minWinkel;
						trans = round(( winkelDiff>180 ? maxWinkel+((360-winkelDiff)>>1) : maxWinkel-(winkelDiff>>1) )*0.1);
						trans%=36;
					}
				}
				
				if(trans==37)
				{
					if(lMuesli)
					{
						//out_winkel = mWinkel+phi_jetzt;//(lRichtung*90+180)%360+phi_jetzt;
						if(lEcke!=-1)
						{
							out_winkel=(lEcke*90+225)%360 - phi_jetzt;
						}
						else
						{
							out_winkel=(lRichtung*90+180)%360 - phi_jetzt;
						}
						out_winkel*=-1;
						out = 4;
						return;
					}
					lRichtung=-1;
					preRichtung=-1;
					lEcke=-1;
					out = 0;
					return;
				}
				int16_t ret=0;
				mWinkel=trans*10+phi_jetzt;//@TODO
				if(mWinkel<0)
					mWinkel+=360;
				mWinkel%=360;
				if(preRichtung==-1)
					preRichtung=richtung(mWinkel);
	
				if(lEcke==-1)
					lEcke=ecke(mWinkel);
	
	
				if((BETRAG(lRichtung-richtung(mWinkel))==2)&&(lRichtung!=-1))//Sprung um zwei Segmente
					lMuesli=true; //halb raus
				if(!(lRichtung-richtung(mWinkel)))  //das gleiche Segment
					lMuesli=false;
		
				if(lEcke!=-1) //Wenn Ecke erkannt
				{
					int8_t tempR=richtung(mWinkel);
					if((!(lEcke-tempR))||(tempR-lEcke==1)||(lEcke==3&&!tempR)) //Wenn Richtung in der korrekten Ecke (auch Sonderfall)
						lMuesli=false;
					else
						lMuesli=true;	
				}
	
		
				if(lMuesli)
				{
					ret=trans*10;		//Fahrwinkel nicht invertiert
					lRichtung=(richtung(mWinkel)+2)%4;//Segment invertieren
				}
				else
				{
					ret=trans*10+180;		
					ret%=360;			//Fahrwinkel invertiert
					lRichtung=richtung(mWinkel);//Segment nicht invertiert
				}
	
				if(ret>180)
					ret -=360;
					
				ret*=-1;
				
				out_winkel = ret;
				out = lMuesli?3:2;
			#else
				// Sensoren überprüfen
				for(uint8_t i=0; i<32; ++i) {
					if(linie[i]) {
						// Winkel berechnen
						out_winkel = -((360 * i + 180) >> 5);
						if(out_winkel < -180)
							out_winkel += 360;
						out = 3;
						break;
					}
				}
			#endif
			
			// Failsave für Timer
			if(++timer_fail & 32) {
				// Timer läuft nicht mehr -> Software Reset
				CCP = CCP_IOREG_gc;
				RST.CTRL =  RST_SWRST_bm;
			}
		}
	}
}

// USART DataReg-Empty -> weiteres Byte senden
ISR(USARTE1_DRE_vect)
{
	USART_DataRegEmpty(&USART_line);
}

// UART initialisieren
void atmega8_init(void)
{
	// USART aktivieren
	usart_init(&USART_line, &USARTE1, USART_BAUD_SELECT(76800, F_CPU), USART_DREINTLVL_MED_gc, USART_RXCINTLVL_HI_gc, true);
}

// Kalibration am CoBoard
void atmega8_kalibration(uint8_t enable)
{
	static uint8_t zaehler_taster=0;
	if(enable == 0)
	{
		usart_putc(&USART_line, 0);
		zaehler_taster=0;
	}
	else
	{
		if(++zaehler_taster>5)
		{
			if(enable == 1)
				usart_putc(&USART_line, 0b10101010);
			else
				usart_putc(&USART_line, 0b01010101);
		}
	}
}
