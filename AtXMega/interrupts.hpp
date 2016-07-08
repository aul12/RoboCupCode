// interrupts.hpp: globale Interrupts

#ifndef _INTERRUPTS_INC_
#define _INTERRUPTS_INC_

//////////////////
// Definitionen //
//////////////////

#ifndef _MAIN_FILE_
#error interrupts.hpp shall only be included from main.cpp
#endif

////////////////
// Interrupts //
////////////////

// Interrupt für Soft-PWM & PID
ISR(TCC1_OVF_vect)
{
	if(Soft_Counter >= Soft_Counter_Max) { // Overflow von Soft_Counter, Neustart bei 0
		// Zähler zurücksetzen
		Soft_Counter = 0;
		if(!MOTORTASTER) { // Wenn Motorschalter auf OFF
			if(EEPROMTASTER) { // Wenn EEPROM-Schalter auf ein
				if(EEPROM_write) {
					#ifdef _IMU
						_gyroPhi = 0;
						Torrichtung = imu.eulHeading();	
					#else
						Torrichtung = CMPS/10;
					#endif
					eeprom_write_word(Torrichtung_EEPROM, (uint16_t)Torrichtung);
					EEPROM_write = false;
				}
			}
			else {
				EEPROM_write = true;
			}
			torwart2feldspieler = false;
			ball_counter = 0;
			
			// Linienkalibration
			if(!MOTORTASTER)
				atmega8_kalibration(LINIENTASTER?1:0);
			else
				atmega8_kalibration(0);
		}
	}else { // Anzahl erhöhen
		Soft_Counter++;
	}
	
	
	if(!MOTORTASTER){
		out = 0;
		lMuesli = 0;
	}
		
	if(dribblerTime++ > 3000){
		// Dribbler an/aus machen (externe Bedingungen)
		
		#ifdef _DRIBBLER
			if(ball_Distanz > 3000 && BETRAG(ball_Winkel) < 75 && (MOTORTASTER || SW_pos != 3) && !adc_kalib){
				dribbler::power(true);
			}else{
				dribbler::power(false);
			}
		#else
			dribbler::power(false);
		#endif
		
		
		dribblerTime = 3001;
	}else if(dribblerTime++ > 2000){
		TCC0.CCA = 500;
	}else if(dribblerTime++ > 1000){
		TCC0.CCA = 1000;
	}else{
		TCC0.CCA = 500;
	}
	

    // PID-Regelung //
    PID_Counter++;
    if(PID_Counter >= PID_Counter_Max) {
		PID_Counter = 0;
		// PID-Werte
		delta_phi_alt = delta_phi;
		#ifdef _IMU
			//Sensor-Fusion Algorithm running
			if(imu.systemStatus() == 5){
				CLEARLED(7);
				
				#ifdef _GYRO_ONLY
					volatile int16_t _phi_jetzt = imu.eulHeading() - Torrichtung;
				#else
					volatile int16_t _phi_jetzt = gyroPhi;
				#endif
				
				if(_phi_jetzt > 180)
					_phi_jetzt -= 360;
				else if(_phi_jetzt < -180)
					_phi_jetzt += 360;
					
				phi_jetzt = _phi_jetzt;
							
				//Voll Kalibriert
				if(imu.calibStatus() >= 0xF3){
					CLEARLED(8);
				}else{
					SETLED(8);
				}
			}
			else{
				imu.init();
				SETLED(7);
			}
		#endif
    }
	else if(PID_Counter == PID_Counter_Max/4) {	
		#ifdef _POWER_MEASURE
		voltage = spannung.getVoltage();
		
		if(voltage < 11000){
			lowVoltageCount++;
			
			if(lowVoltageCount > 20){
				SETLED(6);
				if(lowVoltageCount>40)
					lowVoltageCount = 40;
			}
		}
		else{
			lowVoltageCount--;
			
			if(lowVoltageCount<20){
				CLEARLED(6);
				if(lowVoltageCount < 0)
					lowVoltageCount = 0;
			}	
		}
		#endif
		
			
		// Ball-Winkel & Distanz bestimmen
		uint16_t weg = SCHALTER(6)?ADC_BALLWEG_AKTIV:ADC_BALLWEG_PASSIV;
		
		uint8_t ballWeg = false;
		
		if((ADC_Werte[0] < weg) && (ADC_Werte[1] < weg) && (ADC_Werte[2] < weg) && (ADC_Werte[3] < weg) && (ADC_Werte[4] < weg) && (ADC_Werte[5] < weg) && (ADC_Werte[6] < weg) && (ADC_Werte[7] < weg)) {
			// Ball nicht gefunden
			ball_DistanzWinkel = ball_Distanz = 5;
			ball_Winkel = 0;
			ballWeg = true;
		}
		else {
			// Ball-Winkel & Distanz normal bestimmen
			float Vektor_X = 0, Vektor_Y = 0;
			for(uint8_t i=0; i<8; i++) {
				Vektor_X += ADC_Werte[i]*sinus(ADC_Winkel[i]);
				Vektor_Y += ADC_Werte[i]*cosinus(ADC_Winkel[i]);
			}

			ball_Winkel = ((int16_t)(atan2(Vektor_Y, Vektor_X)*180.0/M_PI));
			
			if(ROBO==1){
				//ball_Winkel += 5;
			}
			
			ball_WinkelA = ball_Winkel - phi_jetzt;
				

			if(ball_Winkel <= -180)
				ball_Winkel += 360;
			else if(ball_Winkel > 180)
				ball_Winkel -= 360;
			ball_Distanz = (uint16_t)Round(hypot(Vektor_X, Vektor_Y));
			ball_DistanzWinkel = corrDistanz(ball_Distanz, ball_Winkel);
		}
		
		#ifdef _TSOP
			// Ball-Winkel & Distanz für TSOP-Sensoren
			/*float Vektor_X = 0, Vektor_Y = 0;
			for(uint8_t i=0; i<8; i++) {
				Vektor_X += tsopWerte[i]*sinus(tsopSensorWinkel[i]);
				Vektor_Y += tsopWerte[i]*cosinus(tsopSensorWinkel[i]);
			}

			tsopBallWinkel = ((int16_t)(atan2(Vektor_Y, Vektor_X)*180.0/M_PI));*/	
			
			uint8_t tsopMin = 0;
			
			for(uint8_t c=0; c<8; c++){
				if(tsopWerte[c]>tsopWerte[tsopMin]){
					tsopMin = c;
				}
			}
			
			tsopBallWinkel = tsopMin * 45 - 135;		

			if(tsopBallWinkel <= -180)
				tsopBallWinkel += 360;
			else if(tsopBallWinkel > 180)
				tsopBallWinkel -= 360;
				
			if(tsopWerte[0] < TSOP_BALLWEG &&
				tsopWerte[1] < TSOP_BALLWEG &&
				tsopWerte[2] < TSOP_BALLWEG &&
				tsopWerte[3] < TSOP_BALLWEG &&
				tsopWerte[4] < TSOP_BALLWEG &&
				tsopWerte[5] < TSOP_BALLWEG &&
				tsopWerte[6] < TSOP_BALLWEG &&
				tsopWerte[7] < TSOP_BALLWEG){
				tsopBallIntens = 0;	
			}else{
				tsopBallIntens = tsopWerte[tsopMin];//(uint16_t)Round(hypot(Vektor_X, Vektor_Y));
			}
			
		#endif 
		
		//##########################################
		if(!MOTORTASTER){
			ballGute = 0;
		}else if(BALL_IN_DRIBB){
			ballGute = 255;
		}else if(ballWeg){
			#ifdef SUPERFIELD
			if(tsopBallIntens>0){
				ballGute = tsopBallIntens/480;
			}
			#endif
			{
				ballGute = 1;
			}
			
		}else{
			ballGute = (ballIntens /60) * ((300-BETRAG(ball_Winkel))/120);
			
			if(BETRAG(ball_Winkel)>90)
				ballGute /= 2;
		}
		
		usart_putc(&bluetooth, (uint8_t)ballGute);
		
		
		// Ball-Inaktivität bestimmen
		if(ball_counter++ < 320) { // 4 Sekunden
			if(ball_Distanz >= ADC_BALLINAKTIV && ball_Distanz > ball_Distanz_alt-ADC_BALLINAKTIV && ball_Distanz < ball_Distanz_alt+ADC_BALLINAKTIV && ball_Winkel > ball_Winkel_alt-5 && ball_Winkel < ball_Winkel_alt+5) {
				// keine Aktion
			}
			else {
				ball_Distanz_alt = ball_Distanz;
				ball_Winkel_alt = ball_Winkel;
				ball_counter = 0;
			}
		}
		else if(ball_counter <= 640) {
			torwart2feldspieler = true;
		}
		else {
			torwart2feldspieler = false;
			ball_counter = 0;
		}
	}
    else if(PID_Counter == PID_Counter_Max*2/4) {
        // Drehung
		delta_phi = soll_phi - phi_jetzt;

        if(delta_phi > 180)
            delta_phi -= 360;
        else if(delta_phi < -180)
            delta_phi += 360;
            
		// Integral
        PID_flaeche += dt * delta_phi;
        if(PID_flaeche > PID_flaeche_Max)
            PID_flaeche = PID_flaeche_Max;
        else if(PID_flaeche < -PID_flaeche_Max)
            PID_flaeche = -PID_flaeche_Max;
            
        if(delta_phi_alt*delta_phi <= 0)
            PID_flaeche *= 0.5;
        if(delta_phi*PID_flaeche <= 0)
            PID_flaeche *= 0.99;
            
		// PID
		#ifdef _IMU
			roll = imu.gyrDataZ();
			if(imu.error())
				roll = 0;
				
			_gyroPhi += (int16_t)(roll*dt);
			_gyroPhi %= 5760;
			
			gyroPhi = (int16_t)(_gyroPhi/16);
			
			if(gyroPhi>180)
				gyroPhi -= 360;
			else if(gyroPhi<-180)
				gyroPhi += 360;
		#endif
		PID = PID_P * delta_phi + PID_I * PID_flaeche + PID_D * roll;

		// Langsamer drehen
		if(ballda::check()){
			if(PID > MAX_DREH_BALL)
				PID = MAX_DREH_BALL;
			else if(PID < -MAX_DREH_BALL)
				PID = -MAX_DREH_BALL;
		}else{
			if(PID > MAX_DREH)
				PID = MAX_DREH;
			else if(PID < -MAX_DREH)
				PID = -MAX_DREH;
		}
		
		PID_int = (int16_t)Round(PID);
		if(super_turn!=0  && out < 3)
			PID_int = super_turn;
			
		
		// Stärke der Regelung
		{ // Gültigkeitsbereich für lokale Variablen
			int16_t power[4];
			power[0] = k[0]+PID_int;
			power[1] = k[1]+PID_int;
			power[2] = k[2]+PID_int;
			power[3] = k[3]+PID_int;
			
			
			if((BETRAG(imu.eulRoll())>8 && BETRAG(imu.gyrDataX())> 800) ||
						(BETRAG(imu.eulPitch())>8 && BETRAG(imu.gyrDataX())>800)){
				power[0] = power[1] = power[2] = power[3] = PID_int;
			}

			// Optimierungsalgorithmus
			int16_t max_power = 0;
			uint8_t i;
			
			for(i=0; i<4; i++) {
				if(BETRAG(power[i]) > max_power) 
					max_power = BETRAG(power[i]);
			}
			
			if(max_power > MAX_PWM) {
				for(i=0; i<4; i++) {
					power[i] = Round(power[i]*(MAX_PWM/(float)max_power));
				}
			}
			
			// Fahren
			if(MOTORTASTER) { // Wenn Motorschalter auf ON
				MotorPower(0, power[0]);
				MotorPower(1, power[1]);
				MotorPower(2, power[2]);
				MotorPower(3, power[3]);
			}
			else {
				MotorPower(0, 0);
				MotorPower(1, 0);
				MotorPower(2, 0);
				MotorPower(3, 0);
			}
		}
        
		
    }
	
	#ifdef _US
		else if(PID_Counter == PID_Counter_Max*3/4) {		
				// Ultraschall auslesen
				if(US_Abfrage == 0) {
					if(US_sensor[0].ready() && US_sensor[2].ready()) {
						US_Werte[0] = US_sensor[0].read();
						US_Werte[2] = US_sensor[2].read();
						if(US_Werte[0] == 0) {
							if(US_Werte[2] == 0) {
								US_Werte[0] = 80;
								US_Werte[2] = 80;
							}
							else {
								#ifndef SOCCER_A
									US_Werte[0] = 162-US_Werte[2];
								#else
									US_Werte[0] = 103-US_Werte[2];
								#endif
							}
						}
						else if(US_Werte[2] == 0) {
							#ifndef SOCCER_A
								US_Werte[2] = 162-US_Werte[0];
							#else
								US_Werte[2] = 103-US_Werte[0];
							#endif
						}
						US_sensor[1].init_start();
						++US_Abfrage;
					}
				}
				else if(US_Abfrage == 1) {
					if(US_sensor[1].ready()) {
						US_Werte[1] = US_sensor[1].read();
						US_sensor[0].init_start();
						if(US_Werte[1] == 0) {
							US_Werte[1] = 100;
						}
						US_sensor[2].init_start();
						US_Abfrage = 0;
					}
				}
 
			// Positionsbestimmung
			#ifdef SOCCER_A
				US_n_pos[0] = (US_Werte[2]+123-US_Werte[0])/2;
			#else
				US_n_pos[0] = (US_Werte[2]+182-US_Werte[0])/2;
			#endif
			if(US_n_pos[0] > 1024) // Vergleichszahl muss größer 1000 sein
				US_n_pos[0] = 0;
			US_n_pos[1] = US_Werte[1] + 5;
			
			// Korrigierte US-Position
			US_tw_pos[1] = US_n_pos[1];
			if(US_Werte[0]+US_Werte[2] > 150) { // Summe links-rechts passt
				US_rechts_last[0] = US_Werte[0];
				US_links_last[0] = US_Werte[2];
				US_rechts_block = 0;
				US_links_block = 0;
				US_tw_pos[0] = (US_Werte[2]+182-US_Werte[0])/2;
			}
			else { // min ein US blockiert
				// Rechten US überprüfen
				if(US_Werte[0]+20 <= US_rechts_last[US_rechts_block]) { // rechts schlagartig kleiner
					if(++US_rechts_block > 2)
						US_rechts_block = 2;
					US_rechts_diff[US_rechts_block-1] = US_rechts_last[0]-US_rechts_last[US_rechts_block];
				}
				else if(US_Werte[0] >= US_rechts_last[US_rechts_block]+20) { // rechts schlagartig größer
					if(US_rechts_block-- == 0)
						US_rechts_block = 0;
				}
				US_rechts_last[US_rechts_block] = US_Werte[0];
				
				// Linken US überprüfen
				if(US_Werte[2]+20 <= US_links_last[US_links_block]) { // links schlagartig kleiner
					if(++US_links_block > 2)
						US_links_block = 2;
					US_links_diff[US_links_block-1] = US_links_last[0]-US_links_last[US_links_block];
				}
				else if(US_Werte[2] >= US_links_last[US_links_block]+20) { // links schlagartig größer
					if(US_links_block-- == 0)
						US_links_block = 0;
				}
				US_links_last[US_links_block] = US_Werte[2];
				
				// Position statisch berechnen
				uint16_t rechts_tmp = US_Werte[0];
				if(US_rechts_block > 0) {
					rechts_tmp += US_rechts_diff[US_rechts_block-1];
				}
				uint16_t links_tmp = US_Werte[2];
				if(US_links_block > 0) {
					links_tmp += US_links_diff[US_links_block-1];
				}
				US_tw_pos[0] = (links_tmp+182-rechts_tmp)/2;
			}
			
			if(out>0){
				switch(lRichtung){
					case 0:
						US_tw_pos[1] = 240 - Abstand_Linie[out];
						break;
					case 1:
						US_tw_pos[0] = 180 - Abstand_Linie[out];
						break;
					case 2:
						US_tw_pos[1] = Abstand_Linie[out];
						break;
					case 3:
						US_tw_pos[0] = Abstand_Linie[out];
						break;
				}
			}
		}
	#else
		// Default-Werte
		US_Werte[0] = 80;
		US_Werte[1] = 100;
		US_Werte[2] = 80;
		US_Werte[3] = 100;
		US_pos[0] = 90;
		US_pos[1] = 60;
	#endif
	
	// Ballkontrolle
	ballda::update();
	
	// Anti-Dauer-Schuss
	schuss::tick();
	
	// Interrupt läuft noch
	timer_fail = 0;
		
	
	// Debug-Ausgaben //
	if(DISPLAYTASTER && ++display_counter>=DISPLAY_COUNTER_WDH) {
		void debug_output(void); // Deklaration -> Anweisung an Compiler dass Funktion existiert (ganz unten in Datei)
		debug_output(); // Funktion aufrufen
		display_counter = 0;
	}else if(!DISPLAYTASTER){
		display_page = 0;
	}
	
	
	
	#ifdef _TSOP
		SPIC.DATA = maxChannel[0];
	#endif
	
	noObjTimer++;
	
	if(noObjTimer>100){
		noObjTimer = 101;
	}
}

//UART F1-TX Interrupt (debug)
ISR(USARTF1_DRE_vect)
{
	USART_DataRegEmpty(&debug);
}

//UART F1-TX Interrupt (bluetooth)
ISR(USARTD0_DRE_vect)
{
	USART_DataRegEmpty(&bluetooth);
}

ISR(USARTD0_RXC_vect){
	// Daten empfangen
	ballGuteEmpfang = bluetooth.usart->DATA;
}

// twiMasterD Interrupt
ISR(TWID_TWIM_vect)
{
	TWI_MasterInterruptHandler(&twiMasterD);
}

// twiMasterF Interrupt
ISR(TWIF_TWIM_vect)
{
	TWI_MasterInterruptHandler(&twiMasterF);
}

ISR(SPIC_INT_vect)
{
	maxMux++;
	
	if(maxMux == 1){
		while(!MAX_READY);
		SPIC.DATA = 0;
	}else if(maxMux == 2){
		maxVal = SPIC.DATA<<8;
		SPIC.DATA = 0;
	}else{
		maxVal |= SPIC.DATA;
		tsopWerte[tsopMux] = 5000 - maxVal;
		maxMux = 0;
		
		tsopMux++;
		if(tsopMux>7)
			tsopMux = 0;
		else
			SPIC.DATA = maxChannel[tsopMux];
	}
}

//Hall-Timer Overflow -> keine Bewegung
ISR(TCD0_OVF_vect){
	rpmHall[0] = 0;
	
	

	//15-Sekunden kein Feedback -> Sensor-Defekt
	if(hallNoRotationCounter[0]++ > 7)
		hallNoRotationCounter[0] = 8;
}

ISR(TCD1_OVF_vect){
	rpmHall[1] = 0;

	//15-Sekunden kein Feedback -> Sensor-Defekt
	if(hallNoRotationCounter[1]++ > 7)
		hallNoRotationCounter[1] = 8;
}

ISR(TCF0_OVF_vect){
	rpmHall[2] = 0;

	//15-Sekunden kein Feedback -> Sensor-Defekt
	if(hallNoRotationCounter[2]++ > 7)
		hallNoRotationCounter[2] = 8;
}

ISR(TCF1_OVF_vect){
	rpmHall[3] = 0;

	//15-Sekunden kein Feedback -> Sensor-Defekt
	if(hallNoRotationCounter[3]++ > 7)
		hallNoRotationCounter[3] = 8;
}


//Hall-Interrupts
ISR(PORTB_INT0_vect){
	//Rad sehr langsam
	if(hallNoRotationCounter[0] > 0){
		TCD0.CNT = 0;
		hallNoRotationCounter[0] = 0;
	}

	if(hallCounter[0]++%2 == 0){
		rpmHall[0] = (11250000/4)/TCD0.CNT;
		TCD0.CNT = 0;
	}
}

ISR(PORTB_INT1_vect){
	//Rad sehr langsam
	if(hallNoRotationCounter[1] > 0){
		TCD1.CNT = 0;
		hallNoRotationCounter[1] = 0;
	}

	if(hallCounter[1]++%2 == 0){
		rpmHall[1] = (11250000/4)/TCD1.CNT;
		TCD1.CNT = 0;
	}
}

ISR(PORTE_INT0_vect){
	//Rad sehr langsam
	if(hallNoRotationCounter[2] > 0){
		TCF0.CNT = 0;
		hallNoRotationCounter[2] = 0;
	}

	if(hallCounter[2]++%2 == 0){
		rpmHall[2] = (11250000/4)/TCF0.CNT;
		TCF0.CNT = 0;
	}
}

ISR(PORTE_INT1_vect){
	//Rad sehr langsam
	if(hallNoRotationCounter[3] > 0){
		TCF1.CNT = 0;
		hallNoRotationCounter[3] = 0;
	}

	if(hallCounter[3]++%2 == 0){
		rpmHall[3] = (11250000/4)/TCF1.CNT;
		TCF1.CNT = 0;
	}
}

// Fehler im Oszilator
ISR(OSC_XOSCF_vect)
{
	while(true) {
		wdt_reset();
		PORTQ.OUTSET = 0b00001111;
		for(uint8_t i=0; i<64; ++i) {
			wdt_reset();
			_delay_ms(1);
		}
		wdt_reset();
		PORTQ.OUTCLR = 0b00001111;
		for(uint8_t i=0; i<64; ++i) {
			wdt_reset();
			_delay_ms(1);
		}
	}
}

// Displayausgaben
void debug_output(void)
{
	uint16_t keypad = display.read_key();
	if(keypad != keypad_last) {
		if((keypad_last & (1 << 9)) && ((~keypad) & (1 << 9))) { // *-Taste
			--display_page;
			if(display_page > PAGE_COUNT)
				display_page = PAGE_COUNT;
		}
		if((keypad_last & (1 << 11)) && ((~keypad) & (1 << 11))) { // #-Taste
			++display_page;
			if(display_page > PAGE_COUNT)
				display_page = 0;
		}
		for(uint8_t i=0; i<=10; ++i) { // Alle anderen Tasten
			if(i == 9)
				continue;
			if((keypad_last & (1 << i)) && ((~keypad) & (1 << i))) {
				uint8_t nr = i + 1;
				if(nr > 9)
					nr = 0;
				if(select_page >= 0x200) // letzte (3.) Zahl
					select_page += 0x400 | (nr);
				else if(select_page >= 0x100) // 2. Zahl
					select_page += 0x200 | (nr * 10);
				else if(select_page == 0) // 1. Zahl
					select_page = 0x100 | ((nr > 2 ? 0 : nr) * 100);
				if(select_page >= 0x400) { // Zu Seite Wechseln
					display_page = select_page & 0xFF;
					select_page = 0;
				}
			}
		}
		keypad_last = keypad;	
	}
	display.clear();
	
	switch(display_page) {
		// Inhaltsverzeichnis
		case 0:
			display.out_str(1, 1, "Inhaltsverzeichnis ");
			display.write_char(ROBO + 0x30);
			display.out_str(2, 1, __DATE__);
			display.out_str(2, 13, __TIME__);
			display.out_str(3, 1, "Seite 000 bis 009");
			display.out_str(4, 1, "aktuell Seite 000");	
		break;
		case 1:
			display.out_str(1, 1, "010 Fahrtrichtung");
			display.out_str(2, 1, "011 PID und BNO");
			display.out_str(3, 1, "012 ADC Werte");
			display.out_str(4, 1, "013 ADC Tiefpass");
		break;
		case 2:
			display.out_str(1, 1, "014 Ballposition");
			display.out_str(2, 1, "015 Ballinaktiv");
			display.out_str(3, 1, "016 US und Position");
			display.out_str(4, 1, "017 Filterpositionen");
		break;
		case 3:
			display.out_str(1, 1, "018 Linie");
			display.out_str(2, 1, "019 Pixy");
			display.out_str(3, 1, "020 IMU");
			display.out_str(4, 1, "021 Hall-Sensoren");
		break;
		case 4:
			display.out_str(1, 1, "022 Spannungmesser");
		break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			display.out_str(1, 1, " Inhaltsverzeichnis");
			display.out_str(2, 1, "--------------------");
			display.out_str(3, 1, "PLATZHALTER");
			display.out_str(4, 4, "aktuell Seite 00");
			display.write_char(display_page + 0x30);
		break;
				
		// Fahrtrichtung DEBUG
		case 10:
			display.out_str(1, 1, "Motor 0:");
			display.out_int(1, 10, (int16_t)k[0]);
			display.out_str(2, 1, "Motor 1:");
			display.out_int(2, 10, (int16_t)k[1]);
			display.out_str(3, 1, "Motor 2:");
			display.out_int(3, 10, (int16_t)k[2]);
			display.out_str(4, 1, "Motor 3:");
			display.out_int(4, 10, (int16_t)k[3]);
		break;

		// PID DEBUG
		case 11:
			display.out_int(1, 1, (int16_t)(k[0]+PID_int));
			display.out_int(2, 1, (int16_t)(k[1]+PID_int));
			display.out_int(3, 1, (int16_t)(k[2]+PID_int));
			display.out_int(4, 1, (int16_t)(k[3]+PID_int));
			display.out_str(1, 9, "s_phi:");
			display.out_int(1, 17, (int16_t)soll_phi);
			display.out_str(2, 9, "phi_j:");
			display.out_int(2, 17, (int16_t)phi_jetzt);
			display.out_str(3, 9, "CMPS:");
			display.out_int(3, 17, (int16_t)imu.eulHeading());
			display.out_str(4, 9, "Tor:");
			display.out_int(4, 17, (int16_t)Torrichtung);
		break;
		
		// ADC DEBUG
		case 12:
			display.out_str(1, 1, "ADC0:");
			display.out_int(1, 7, (int16_t)ADC_Werte[0]);
			display.out_str(2, 1, "ADC1:");
			display.out_int(2, 7, (int16_t)ADC_Werte[1]);
			display.out_str(3, 1, "ADC2:");
			display.out_int(3, 7, (int16_t)ADC_Werte[2]);
			display.out_str(4, 1, "ADC3:");
			display.out_int(4, 7, (int16_t)ADC_Werte[3]);
			display.out_str(1, 11, "ADC4:");
			display.out_int(1, 17, (int16_t)ADC_Werte[4]);
			display.out_str(2, 11, "ADC5:");
			display.out_int(2, 17, (int16_t)ADC_Werte[5]);
			display.out_str(3, 11, "ADC6:");
			display.out_int(3, 17, (int16_t)ADC_Werte[6]);
			display.out_str(4, 11, "ADC7:");
			display.out_int(4, 17, (int16_t)ADC_Werte[7]);
		break;
		case 13:
			display.out_str(1, 1, "ADC0:");
			display.out_int(1, 7, (int16_t)ADC_Werte_TP[0]);
			display.out_str(2, 1, "ADC1:");
			display.out_int(2, 7, (int16_t)ADC_Werte_TP[1]);
			display.out_str(3, 1, "ADC2:");
			display.out_int(3, 7, (int16_t)ADC_Werte_TP[2]);
			display.out_str(4, 1, "ADC3:");
			display.out_int(4, 7, (int16_t)ADC_Werte_TP[3]);
			display.out_str(1, 11, "ADC4:");
			display.out_int(1, 17, (int16_t)ADC_Werte_TP[4]);
			display.out_str(2, 11, "ADC5:");
			display.out_int(2, 17, (int16_t)ADC_Werte_TP[5]);
			display.out_str(3, 11, "ADC6:");
			display.out_int(3, 17, (int16_t)ADC_Werte_TP[6]);
			display.out_str(4, 11, "ADC7:");
			display.out_int(4, 17, (int16_t)ADC_Werte_TP[7]);
		break;
		
		// Ball DEBUG
		case 14:
			display.out_str(1, 1, "BallwinkelA:");
			display.out_int(1, 12, (int16_t)ball_WinkelA);
			display.out_str(2, 1, "BallwinkelR:");
			display.out_int(2, 12, (int16_t)ball_WinkelR);
			display.out_str(3, 1, "Balldistanz:");
			display.out_int(3, 12, (int16_t)ball_Distanz);
			/*display.out_str(3, 1, "Balldistcm:");
			display.out_int(3, 12, (int16_t)ball_Distanz_cm);*/
			display.out_str(4, 1, "Balldistwin:");
			display.out_int(4, 12, (int16_t)ball_DistanzWinkel);
			display.out_str(1, 17, "TSOP");
			display.out_int(2, 17, (int16_t)TSOP);
			display.out_str(3, 17, "Lego");
			display.out_int(4, 17, (int16_t)lego_IR);
		break;
		
		// Ball-Inaktivität
		case 15:
			display.out_str(1, 1, "Torwart2Feld:");
			display.out_str(2, 1, "ball_counter:");
			display.out_str(3, 1, "SW_pos      :");
			display.out_int(1, 15, (int16_t)torwart2feldspieler);
			display.out_int(2, 15, (int16_t)ball_counter);
			display.out_int(3, 15, (int16_t)SW_pos);
			display.out_int(4, 1, (int16_t)MOTORTASTER?1:0);
			display.out_int(4, 5, (int16_t)EEPROMTASTER?1:0);
			display.out_int(4, 9, (int16_t)LICHTSCHRANKE?1:0);
			display.out_int(4, 13, (int16_t)LINIENTASTER?1:0);
			display.out_int(4, 17, (int16_t)DISPLAYTASTER?1:0);
		break;
		
		// US-DEBUG
		case 16:
			/*display.out_str(1, 1, "US-vorne:");
			display.out_int(1, 11, (int16_t)US_Werte[3]);*/
			display.out_str(2, 1, "US-rechts:");
			display.out_int(2, 11, (int16_t)US_Werte[0]);
			display.out_str(3, 1, "US-hinten:");
			display.out_int(3, 11, (int16_t)US_Werte[1]);
			display.out_str(4, 1, "US-links:");
			display.out_int(4, 11, (int16_t)US_Werte[2]);
			display.out_str(1, 15, "posX:");
			display.out_int(2, 15, (int16_t)(US_pos[0]));
			display.out_str(3, 15, "posY:");
			display.out_int(4, 15, (int16_t)(US_pos[1]));
		break;
		
		// Torwart-Filterposition
		case 17:
			display.out_str(1, 1, "US_n_pos X:");
			display.out_str(2, 1, "US_n_pos Y:");
			display.out_str(3, 1, "US_tw_pos X:");
			display.out_str(4, 1, "US_tw_pos Y:");
			display.out_int(1, 14, (int16_t)US_n_pos[0]);
			display.out_int(2, 14, (int16_t)US_n_pos[1]);
			display.out_int(3, 14, (int16_t)US_tw_pos[0]);
			display.out_int(4, 14, (int16_t)US_tw_pos[1]);
		break;
		
		
		// Linienerkennung
		case 18:
			display.set_cursor(1, 1);
			display.write_char(linie[25] + 0x30);
			display.write_char(linie[26] + 0x30);
			display.write_char(linie[27] + 0x30);
			display.write_char(linie[28] + 0x30);
			display.write_char(linie[29] + 0x30);
			display.write_char(linie[30] + 0x30);
			display.write_char(linie[31] + 0x30);
			display.write_char(linie[0] + 0x30);
			display.write_char(linie[1] + 0x30);
			display.write_char(linie[2] + 0x30);
			display.write_char(linie[3] + 0x30);
			display.write_char(linie[4] + 0x30);
			display.write_char(linie[5] + 0x30);
			display.write_char(linie[6] + 0x30);
			display.out_int(2, 1, linie[24]);
			display.out_int(2, 14, linie[7]);
			display.out_int(3, 1, linie[23]);
			display.out_int(3, 14, linie[8]);
			display.set_cursor(4, 1);
			display.write_char(linie[22] + 0x30);
			display.write_char(linie[21] + 0x30);
			display.write_char(linie[20] + 0x30);
			display.write_char(linie[19] + 0x30);
			display.write_char(linie[18] + 0x30);
			display.write_char(linie[17] + 0x30);
			display.write_char(linie[16] + 0x30);
			display.write_char(linie[15] + 0x30);
			display.write_char(linie[14] + 0x30);
			display.write_char(linie[13] + 0x30);
			display.write_char(linie[12] + 0x30);
			display.write_char(linie[11] + 0x30);
			display.write_char(linie[10] + 0x30);
			display.write_char(linie[9] + 0x30);
			display.out_str(1, 16, "outp:");
			display.out_int(2, 16, (int16_t)out_pos);
			#if _COMPLEX_LINE==3
				display.out_int(3, 16, (int16_t)line_regler);
			#else
				display.out_str(3, 16, "phi:");
			#endif
			display.out_int(4, 16, (int16_t)out_winkel);
		break;
		
		// Pixie-Werte
		case 19:
			#ifdef _PIXIE
				display.out_str(1, 1, "phi:");
				display.out_int(1, 8, (int16_t)tor_winkel);
				display.out_str(2, 1, "xdiff:");
			//	display.out_int(2, 8, (int16_t)_xdiff);
				display.out_str(3, 1, "xPos:");
				display.out_int(3, 8, (int16_t)pixyVorneData->xPos);
				display.out_str(4, 1, "phiJ:");
				display.out_int(4, 8, (int16_t)phi_jetzt);
			#else
				display.out_str(1, 1, "_PIXIE not compiled123");
			#endif
		break;
		
		// IMU-Werte
		case 20:
			#ifdef _IMU
			display.out_str(1, 1, "Phi-Jetzt:");
			display.out_int(1, 7, (int16_t)phi_jetzt);
			display.out_str(2, 1, "Roll:");
			display.out_int(2, 7, (int16_t)roll);
			display.out_str(3, 1, "Pitch:");
			display.out_int(3, 7, (int16_t)imu.eulPitch());
			display.out_str(1, 11, "GyrX:");
			display.out_int(1, 17, (int16_t)imu.gyrDataX());
			display.out_str(2, 11, "GyrY:");
			display.out_int(2, 17, (int16_t)imu.gyrDataY());
			display.out_str(3, 11, "GyP:");
			display.out_int(3, 15, (int16_t)gyroPhi);
			#else
			display.out_str(1, 1, "_IMU not compiled");
			#endif
		break;
	
		
		// Hall Sensoren
		case 21:
			display.out_str(1, 1, "Count:");
			display.out_int(1, 8, (uint16_t)hallCounter);		
			display.out_str(2, 1, "RPM:");
			display.out_int(2, 7, (uint16_t)rpmHall);
			display.out_str(3, 1, "HallPwr:");
			display.out_int(3, 7, (uint16_t)hallPower);
		break;
		
		// Spannungsmesser
		case 22:
			display.out_str(1, 1, "Volt:");
			display.out_int(1, 8, (uint16_t)voltage);
		break;
		
		case 23:
			display.out_str(1,1,"RAW");
			display.out_str(1,15,"Linie");
			display.out_str(2,1,"PROC");
			display.out_str(2,13,"Fahr");
			display.out_str(3,1,"Richtung");
			display.out_str(3,13,"Ecke");
			display.out_str(4,1,"Muesli");
			display.out_int(1,5, trans);
			display.out_int(2,6, mWinkel);
			display.out_int(2,18, out_winkel);
			display.out_int(3,10, lRichtung);
			display.out_int(3,18, lEcke==-1?lEcke:lEcke*90+45);
			display.out_int(4,8, lMuesli);
		break;
		
		case 24:
			display.out_str(1, 1, "TSOP0:");
			display.out_int(1, 7, (int16_t)tsopWerte[0]);
			display.out_str(2, 1, "1:");
			display.out_int(2, 7, (int16_t)tsopWerte[1]);
			display.out_str(3, 1, "2:");
			display.out_int(3, 7, (int16_t)tsopWerte[2]);
			display.out_str(4, 1, "3:");
			display.out_int(4, 7, (int16_t)tsopWerte[3]);
			display.out_str(1, 12, "4:");
			display.out_int(1, 17, (int16_t)tsopWerte[4]);
			display.out_str(2, 12, "5:");
			display.out_int(2, 17, (int16_t)tsopWerte[5]);
			display.out_str(3, 12, "6:");
			display.out_int(3, 17, (int16_t)tsopWerte[6]);
			display.out_str(4, 12, "7:");
			display.out_int(4, 17, (int16_t)tsopWerte[7]);
			break;
			
		case 25:
			display.out_str(1, 1, "TSOP Winkel:");
			display.out_int(1, 12, (int16_t)tsopBallWinkel);
			display.out_str(2, 1, "TSOP Intens:");
			display.out_int(2, 12, (int16_t)tsopBallIntens);
			break;
		
		case 26:
			display.out_str(1, 1, "Hier:");
			display.out_int(1, 12, (int16_t)ballGute);
			display.out_str(2, 1, "Da:");
			display.out_int(2, 12, (int16_t)ballGuteEmpfang);
			display.out_str(3, 1, ballGuteEmpfang>ballGute?"Passiv":"Aktiv");
			break;
			
		
		default:
			display.out_str(1, 1, "Ungueltige Seite:");
			display.out_int(2, 1, (int16_t)display_page);
		break;
	}
}

#endif
