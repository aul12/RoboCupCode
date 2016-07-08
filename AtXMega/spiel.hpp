 // spiel.hpp: Spielfunktionen

#ifndef _SPIEL_INC_
#define _SPIEL_INC_

//////////////////
// Definitionen //
//////////////////

#ifndef _MAIN_FILE_
#error spiel.hpp shall only be included from main.cpp
#endif

////////////////
// Funktionen //
////////////////

//Anfahrt Prototyp
inline void anfahrtB(void)
{
	#ifndef SUPERFIELD
	if(ballIntens>2000 && BETRAG(ball_WinkelA)<135 && false){
		darfHalbRaus = 1;
	}else{
		darfHalbRaus = 0;
	}
	#endif
		
	if(ballIntens > 3000 || (ballIntens > 2800 && BETRAG(ball_Winkel) < 45)) { // Nahbereich-Anfahrt
		if(ball_Winkel > 60){ // zwischen Ball und Tor rechts
			FahrtrichtungB(-300+ball_Winkel, SPEED_KREIS);
			soll_phi = TOR_WINKEL;
		}
		else if(ball_Winkel < -60){ // zwischen Ball und Tor links
			FahrtrichtungB(300+ball_Winkel, SPEED_KREIS);
			soll_phi = TOR_WINKEL;
		}
		else if(BETRAG(ball_Winkel) > 5){ // Parabel anfahrt
			FahrtrichtungB(1.9 * ball_Winkel, SPEED_NAH);
				
			soll_phi = TOR_WINKEL;
		}else{
			FahrtrichtungB(ball_Winkel, 1000 + (5-ball_Winkel)*100);
			
			soll_phi = TOR_WINKEL;
		}
	}
	else if(ballIntens < 120) { // Ball nicht erkennbar
		if(ROBO==0)
			Fahrtrichtung_XY(90, 40);
		else
			Fahrtrichtung_XY(90, 30);

		soll_phi = 0;
	}
	else { // Fernbereich-Anfahrt
		uint16_t abstand = 4500-ballIntens;
		uint16_t abstand_alt = 4500-ball_Distanz_alt;
		FahrtrichtungB(ball_Winkel, abstand * BALL_P + (abstand - abstand_alt) * BALL_D);
		soll_phi = 0;
	}
}

inline void zumTorDrehenUndSchiessen(){
	if(US_pos[0]>80 && US_pos[0] < 100) {//In Mitte -> schieﬂen
		if(BETRAG(phi_jetzt) < 10){
			schuss::Kick();
		}
		FahrtrichtungB(0, SPEED_BALL);
		soll_phi = TOR_WINKEL;
		super_turn = 0;
	}else{ //Am Rand -> Richtung Tor drehen
		darfHalbRaus = 1;
		super_turn = 0;
		FahrtrichtungB(0, SPEED_BALL);
		
		soll_phi = TOR_WINKEL;
		
		if(BETRAG(soll_phi-phi_jetzt)<12){
			schuss::Kick();
		}
	}
}

inline void trickshoot(){
	darfHalbRaus = 1;
	
	FahrtrichtungB(0,0);
	
	if(trick_shoot_turn == 0){
		if(lEcke==1 || lRichtung==1)
			trick_shoot_turn = 2;
		else
			trick_shoot_turn = 1;
		
		
		#ifndef _GYRO_ONLY
			_gyroPhi = phi_jetzt;
		#endif
	}else if(trick_shoot_turn == 1 || trick_shoot_turn == 2){
		super_turn = (trick_shoot_turn==1?-230:230);
		
		if(trick_shoot_turn==2){
			if(gyroPhi>-150 && gyroPhi<0){
				trick_shoot_turn += 2;
			}
		}else{
			if(gyroPhi<150 && gyroPhi>0){
				trick_shoot_turn += 2;	
			}
		}
		/*if(BETRAG(gyroPhi)>170)
			trick_shoot_turn += 2;*/
		
	}else if(trick_shoot_turn == 3 || trick_shoot_turn == 4){
		super_turn = trick_shoot_turn==3?-1500:1500;
		
		if(BETRAG(gyroPhi)<80){
			schuss::Kick();
		}
		
		
		if(BETRAG(gyroPhi)<10){
			trick_shoot_turn = 0;
			super_turn = 0;
			soll_phi = 0;
		}
	}
}

void ausweichenUndZuruck(void){
	if(ball_Winkel < -90){ // zwischen Ball und Tor links
		FahrtrichtungB(270+ball_Winkel, SPEED_KREIS);
		soll_phi = 0;
	}
	else if(ball_Winkel > 90){ // zwischen Ball und Tor rechts
		FahrtrichtungB(-270+ball_Winkel, SPEED_KREIS);
		
		soll_phi = 0;
	}else{
		if(US_Werte[1]<50){
			if(US_Werte[0] < 45 && US_Werte[2] < 90){
				FahrtrichtungB(0, 1200);
			}else if(US_Werte[2] < 45 && US_Werte[0] < 90){
				FahrtrichtungB(0, 1200);
			}else{
				Fahrtrichtung_XY(90, 42);
			}
		}else{
			Fahrtrichtung_XY(90, 42);
		}
		
		
		soll_phi = 0;
	}
}

void torwart(void){
	if(ballIntens>2500 && US_Werte[1] < 30){
		if(BETRAG(ball_Winkel) < 60){
			FahrtrichtungB(ball_Winkel, 1500);
			}else{
			anfahrtB();
		}
		}else{
		ausweichenUndZuruck();
	}
}

void anfahrenMitBluetooth(void){
	#ifdef _BLUETOOTH
	if(ballGute < ballGuteEmpfang){
		torwart();
	}else
	#endif
	{
		darfHalbRaus = 0;
		trick_shoot_turn = 0;
		super_turn = 0;
		anfahrtB();
	}
}



// Spielfunktion B-Feld
void spielB1(void)
{
   if(BALL_IN_DRIBB) {
		zumTorDrehenUndSchiessen();
	}else{
		anfahrenMitBluetooth();		
	}
}

// Spielfunktion B-Feld (Drehung zum Tor)
void spielB2(void)
{	
	if(BALL_IN_DRIBB) {	
		if(out>0 || trick_shoot_turn>0){		//Auf Linie -> Trickshoot	 		
			if(trick_shoot_turn>0 || lEcke==0 || lEcke==3){
				trickshoot();
			}else{
				darfHalbRaus = false;	
			}
			 
		}else{
			zumTorDrehenUndSchiessen();
		}
		trickshoot();
		darfHalbRaus = 1;	 
	}else{
		if(out>0  && (lEcke==1 || lEcke==3) && BETRAG(ball_WinkelA) < 135){
			soll_phi = ball_WinkelA;
		}
		
			anfahrenMitBluetooth();
	}
}


void torwartB(void){
	if(ballda::check()) { // Ball nicht erkennbar
        FahrtrichtungB(0, SPEED_BALL);
		soll_phi = TOR_WINKEL;
		
		if(BETRAG(soll_phi-phi_jetzt)<10)
			schuss::Kick();
    }else{
		
		soll_phi = 0;
		if(noObjTimer>50){
			Fahrtrichtung_XY(90, 10);
		}else if(pixyVorneData->breite<60){
			FahrtrichtungB(-(pixyVorneData->xPos-160)/4, 1000);
		}else{
			FahrtrichtungB(-(pixyVorneData->xPos-160)/2, 1000);
		}
    }
}


// Nullprogramm
void PIDprogramm(void)
{		
	if(SCHALTER(6)){
		soll_phi = TOR_WINKEL;
	}else{
		soll_phi = 0;
	}
		
	
	FahrtrichtungB(0,0);
}

// Spielfunktion Superfield Linie (Legosensor)
void spielSuper1(void)
{
	if(ballGute >= ballGuteEmpfang){
		if(BALL_IN_DRIBB){
			soll_phi = TOR_WINKEL;
			FahrtrichtungB(0, 1500);
		}else if(ballIntens < 120) { // Ball nicht erkennbar via IR
			if(tsopBallIntens>0){
					FahrtrichtungB(tsopBallWinkel, 1500);
					soll_phi = TOR_WINKEL;
				}else{
					FahrtrichtungB(0,0);
					soll_phi = TOR_WINKEL;
				}
			}else { // Fernbereich-Anfahrt
				anfahrtB();
				soll_phi = TOR_WINKEL;
			}
	}else{
		soll_phi = TOR_WINKEL;
		FahrtrichtungB(0,0);
	}
	
}

// Spielfunktion Superfield (Legosensor)
void spielSuper2(void)
{
	if(LICHTSCHRANKE) {
		super_turn = 0;
		#ifdef _SCHUSS
			if(BETRAG(phi_jetzt) < 45) {
				schuss::Kick();
			}
		#else
			FahrtrichtungB(0, SPEED_WEIT);
		#endif
	}
	#ifdef _SCHUSS
		else if(ballIntens > 4500 && BETRAG(ball_Winkel) < 60) { // Nahbereich-Anfahrt verlangsamen
			super_turn = 0;
			FahrtrichtungB(2*ball_Winkel, 0);
			soll_phi = 0;
		}
	#endif
	else if(ballIntens > 3700 || (ballIntens > 3000 && BETRAG(ball_Winkel) < 45)) { // Nahbereich-Anfahrt
		super_turn = 0;
		if(ball_Winkel > 90) // zwischen Ball und Tor rechts
			FahrtrichtungB(-270+ball_Winkel, SPEED_KREIS);
		else if(ball_Winkel < -90) // zwischen Ball und Tor links
			FahrtrichtungB(270+ball_Winkel, SPEED_KREIS);
		else // hinter Ball								
			FahrtrichtungB(2*ball_Winkel, SPEED_NAH);
		soll_phi = 0;
	}
	else if(ballIntens > 800) { // Fernbereichanfahrt
		super_turn = 0;
		FahrtrichtungB(ball_Winkel, SPEED_WEIT);
		soll_phi = 0;
	}
	#ifdef _LEGO
		// Legosensor
		else if(lego_IR != 0) { // Ball weit weg
			super_turn = 0;
			soll_phi = phi_jetzt;
			if(lego_IR < 6) { // rechts
				FahrtrichtungB((6-lego_IR)*10, SPEED_SUPER);
				if(BETRAG(soll_phi-phi_jetzt) < 5)
					soll_phi -= 5;
			}
			else if(lego_IR > 6) { // links
				FahrtrichtungB((6-lego_IR)*10, SPEED_SUPER);
				if(BETRAG(soll_phi-phi_jetzt) < 5)
					soll_phi += 5;
			}
			else {
				FahrtrichtungB(0, SPEED_WEIT);
			}
		}
	#else
		// TSOP-Diode
		else if(TSOP < 2000) {
			super_turn = 0;
			soll_phi = phi_jetzt;
			FahrtrichtungB(0, SPEED_SUPER);
		}
	#endif
	else { // Ball nicht erkennbar
		FahrtrichtungB(0, 0);
		super_turn = 1;
	}
}

// Spielfunktion Superfield (TSOP schnell)
void spielSuper3(void)
{
	soll_phi = tor_winkel;
	if(ballda::check()  && ballIntens>3000 && BETRAG(ball_Winkel)<20) {
		#ifdef _SCHUSS
		if(BETRAG(phi_jetzt-tor_winkel) < 1 || (BETRAG(phi_jetzt<45))) {
			schuss::Kick();
			FahrtrichtungB(tor_winkel-phi_jetzt,SPEED_BALL);
		}
		#else
		FahrtrichtungB(tor_winkel-phi_jetzt, SPEED_BALL);
		#endif
		soll_phi = tor_winkel;
		super_back = 1;
	}
	#ifdef _SCHUSS
	else if(ballIntens > 4200 && BETRAG(ball_Winkel) < 45) { // Nahbereich-Anfahrt verlangsamen
		FahrtrichtungB(int16_t(1.6*ball_WinkelR), 0);
		soll_phi = tor_winkel;
		super_back = 1;
	}
	#endif
	else if(ballIntens > 3000 || (ballIntens > 2800 && BETRAG(ball_Winkel) < 45)) { // Nahbereich-Anfahrt
		soll_phi = tor_winkel;
		super_back = 1;
		if(ball_WinkelR > 90){ // zwischen Ball und Tor rechts
			FahrtrichtungB(-270+ball_WinkelR, SPEED_KREIS);
			//FahrtrichtungB(0.0005*(ball_Winkel-130)*(ball_Winkel-130)*(ball_Winkel-130)+0.6*ball_Winkel+100, SPEED_KREIS);
		}
		else if(ball_Winkel < -90){ // zwischen Ball und Tor links
			FahrtrichtungB(270+ball_WinkelR, SPEED_KREIS);
			//FahrtrichtungB(-(0.0005*(ball_Winkel-130)*(ball_Winkel-130)*(ball_Winkel-130)+0.6*ball_Winkel+100), SPEED_KREIS);
		}
		else{ // hinter Ball
			FahrtrichtungB(1.8 *ball_WinkelR, SPEED_NAH);
		}
	}
	else if(ballIntens>280) { // Fernbereich-Anfahrt
		uint16_t BETRAGtand = 4500-ballIntens;
		FahrtrichtungB(ball_WinkelR, BETRAGtand * BALL_P);
		soll_phi = tor_winkel;
	}
	#ifdef _LEGO
		// Legosensor
		else if(lego_IR != 0) { // Ball weit weg
			super_turn = 0;
			soll_phi = phi_jetzt;
			if(lego_IR < 6) { // rechts
				FahrtrichtungB((6-lego_IR)*10, SPEED_SUPER2);
				if(BETRAG(soll_phi-phi_jetzt) < 5)
					soll_phi -= 5;
			}
			else if(lego_IR > 6) { // links
				FahrtrichtungB((6-lego_IR)*10, SPEED_SUPER2);
				if(BETRAG(soll_phi-phi_jetzt) < 5)
					soll_phi += 5;
			}
			else {
				FahrtrichtungB(0, SPEED_WEIT);
			}
		}
		#else
		else{
			/*if(TSOP_Werte[0]>1300 && TSOP_Werte[1]>1300 && TSOP_Werte[2]>1300 && TSOP_Werte[3]>1300){
				FahrtrichtungB(0,0);	
			}
			else{
				FahrtrichtungB(ball_Winkel_TSOP, SPEED_SUPER);
			}*/
			soll_phi = tor_winkel;
		}
		#endif
	/*#else
		// TSOP-Diode
		else if(TSOP < 2000) {
			super_turn = 0;
			soll_phi = phi_jetzt;
			FahrtrichtungB(0, SPEED_SUPER2);
		}
	#endif
	else { // Ball nicht erkennbar
		FahrtrichtungB(0, 0);
		super_turn = 1;
	}*/
}

#endif
