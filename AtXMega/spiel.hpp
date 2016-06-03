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
inline void anfahrtB(uint8_t drehung)
{
	if(ballGute >= ballGuteEmpfang || !BLUETOOTH_ENABLE){
		if(ball_WinkelA < 90 && ballIntens>2400)
			darfHalbRaus = 1;
		
		if(ballIntens > 3000 || (ballIntens > 2800 && BETRAG(ball_Winkel) < 45)) { // Nahbereich-Anfahrt
			
			if(ball_Winkel > 60){ // zwischen Ball und Tor rechts
				FahrtrichtungB(-300+ball_Winkel, SPEED_KREIS);
				soll_phi = 0;
			}
			else if(ball_Winkel < -60){ // zwischen Ball und Tor links
				FahrtrichtungB(300+ball_Winkel, SPEED_KREIS);
				soll_phi = 0;
			}
			else{ // Parabel anfahrt
				FahrtrichtungB(1.8 * ball_Winkel, SPEED_NAH);
				
				if(BETRAG(ball_WinkelA) < 29 ||(BETRAG(ball_WinkelA<120) && out>0))
					soll_phi = -ball_WinkelA;
			}
		}
		else if(ballIntens < 120) { // Ball nicht erkennbar
			#ifdef SUPERFIELD
			FahrtrichtungB(0, 0);
			#else
			if(ROBO==0)
			Fahrtrichtung_XY(90, 40);
			else
			Fahrtrichtung_XY(90, 30);
			#endif
			soll_phi = 0;
		}
		else { // Fernbereich-Anfahrt
			uint16_t abstand = 4500-ballIntens;
			uint16_t abstand_alt = 4500-ball_Distanz_alt;
			FahrtrichtungB(ball_Winkel*1.2, abstand * BALL_P + (abstand - abstand_alt) * BALL_D);
			soll_phi = 0;
		}
	}else{
		if(ball_Winkel > 90){ // zwischen Ball und Tor rechts
			FahrtrichtungB(-270+ball_Winkel, SPEED_KREIS);
			soll_phi = 0;
		}
		else if(ball_Winkel < -90){ // zwischen Ball und Tor links
			FahrtrichtungB(270+ball_Winkel, SPEED_KREIS);
			soll_phi = 0;
		}else{
			if(ROBO==0)
			Fahrtrichtung_XY(90, 40);
			else
			Fahrtrichtung_XY(90, 30);
		}
	}
	
}

// Spielfunktion B-Feld
void spielB1(void)
{
   if(BALL_IN_DRIBB) {
		if(BETRAG(phi_jetzt) < 30) 
			schuss::Kick();
		
		FahrtrichtungB(0, SPEED_BALL);
		
		soll_phi = 0;
	}else{
		darfHalbRaus = 0;
		anfahrtB(0);	
	}
}

// Spielfunktion B-Feld (Drehung zum Tor)
void spielB2(void)
{	
	if(BALL_IN_DRIBB) {	
		if(out>0 || trick_shoot_turn!=0){		//Auf Linie -> Trickshoot	 
			 darfHalbRaus = 1;
			 
			 FahrtrichtungB(0,0);
			 
			 if(trick_shoot_turn == 0){
				 if(out>0){
					if(lRichtung==1 || lEcke==0)
						trick_shoot_turn = 2;
					else if(lRichtung == 3 || lEcke==3)
						trick_shoot_turn = 1;
					else{
						if(US_Werte[0]>US_Werte[2])
							trick_shoot_turn = 1;
						else
							trick_shoot_turn = 2;	
					}
				 }else{
					 if(US_Werte[0]>US_Werte[2])
						trick_shoot_turn = 1;
					 else
						trick_shoot_turn = 2;
				 }
					
				#ifndef _GYRO_ONLY
					gyroPhi = phi_jetzt;
				#endif				 				 
			 }else if(trick_shoot_turn == 1 || trick_shoot_turn == 2){
				super_turn = (trick_shoot_turn==1?-250:250);
					
				if(BETRAG(gyroPhi)>110)
					trick_shoot_turn += 2;
				
			 }else if(trick_shoot_turn == 3 || trick_shoot_turn == 4){
				super_turn = trick_shoot_turn==3?-1500:1500;
				
				if(BETRAG(gyroPhi)<100){
					schuss::Kick();
				}
					
				
				if(BETRAG(gyroPhi)<20){
					trick_shoot_turn = 0;
					super_turn = 0;
					soll_phi = 0;
				}
			 }
		}else if(US_pos[0]>60 && US_pos[0] < 120) {//In Mitte -> schie�en
			if(BETRAG(phi_jetzt) < 30){
				schuss::Kick();
			}
			FahrtrichtungB(0, SPEED_BALL);
			super_turn = 0;
		}else{ //Am Rand -> Richtung Tor drehen
			super_turn = 0;
			FahrtrichtungB(0, SPEED_BALL);
			 if(US_pos[0] < 90){
				 FahrtrichtungB(90, 600);
				// soll_phi = -20;
			 }else{
				 FahrtrichtungB(90, 600);
				// soll_phi = 20;
			 }
		}		 
	}else{
		darfHalbRaus = 0;
		trick_shoot_turn = 0;
		super_turn = 0;
		anfahrtB(0);
	}
}


void torwartB(void){
	if(ballIntens < 2500) { // Ball nicht erkennbar
        Fahrtrichtung_XY(90, 30);
        soll_phi = 0;
    }else{
        if(US_pos[0] > 60 && US_pos[0] < 120 && US_pos[1] < 80){
            if(BETRAG(ball_Winkel) < 90){
				FahrtrichtungB(ball_Winkel*1.2, SPEED_TORWART);
			}
            else{
				if(ball_Winkel > 0){ // zwischen Ball und Tor rechts
					FahrtrichtungB(-300+ball_Winkel, SPEED_KREIS);
					soll_phi = 0;
				}
				else{ // zwischen Ball und Tor links					FahrtrichtungB(300+ball_Winkel, SPEED_KREIS);
					soll_phi = 0;
			
			}
			}
        }else{
            Fahrtrichtung_XY(90, 30);
        }
    }
}


// Nullprogramm @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void PIDprogramm(void)
{		
	soll_phi = 0;		
	
	FahrtrichtungB(0,0);
}

// Spielfunktion Superfield Linie (Legosensor)
void spielSuper1(void)
{
	while(ballda::check()) {
		wdt_reset();
		#if _COMPLEX_LINE==3||_COMPLEX_LINE==4
			// Auf der Linie fahren
			super_turn = 0;
			int16_t speed = (line_regler-15) * 600;
			if(speed < 0)
				speed = 0;
			if(speed > 5000)
				speed = 5000;
			if(out == 1 || out == 2) {
				if(out == 1)
					Fahrtrichtung(-out_winkel + 180, speed);
				else
					Fahrtrichtung(-out_winkel, speed);
				int16_t k_tmp[4];
				k_tmp[0] = k[0];
				k_tmp[1] = k[1];
				k_tmp[2] = k[2];
				k_tmp[3] = k[3];
				// Ecke erkennen
				if(BETRAG(phi_jetzt-out_winkel) < 45 || BETRAG(phi_jetzt-out_winkel) > 135) {
					soll_phi = 0;
					Fahrtrichtung(-90-phi_jetzt, SPEED_NAH);
				}
				else {
					soll_phi = 90;
					Fahrtrichtung(-phi_jetzt, SPEED_NAH);
				}
				k[0] += k_tmp[0];
				k[1] += k_tmp[1];
				k[2] += k_tmp[2];
				k[3] += k_tmp[3];
			}
			else if(out >= 3) {
				Fahrtrichtung(-out_winkel, SPEED_LINIE);
			}
			else {
				soll_phi = 90;
				Fahrtrichtung(90-phi_jetzt, SPEED_SEHRNAH);
			}
		#else
			// Schie�en
			#ifdef _SCHUSS
				if(BETRAG(phi_jetzt) < 45) {
					schuss::Kick();
				}
				break;
			#else
				FahrtrichtungB(0, SPEED_WEIT);
				break;
			#endif
		#endif
	}
	if(ballIntens > 3700 || (ballIntens > 3000 && BETRAG(ball_Winkel) < 45)) { // Nahbereich-Anfahrt
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
		if(BETRAG(phi_jetzt-tor_winkel) < 1 || (NO_OBJECT && BETRAG(phi_jetzt<45))) {
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
