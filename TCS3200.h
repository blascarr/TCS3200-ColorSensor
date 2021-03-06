/*
  Design and created by Blascarr
  TCS3200
  Name    : Blascarr
  Description: TCS3200.h
  version : 1.0
	TCS3200 is a library for color detection with TCS3200 module.
	The main objective is improve the performance detection of differente range colors
	for simple robotic applications.
	Compute time event intervals to read input signal for non-blocking systems.
	Compute buffer based on mean values and filtering errors.
	Mapping in different range values for colors.
	EEPROM memory for save some calibration data and color list.
	Assign interruption pint to the INPUT signal to calculate samples. (WIP)
  
  This library is a modification of MD_TCS230 created by Marco Colli
    https://github.com/MajicDesigns/MD_TCS230
  	
  	Blascarr invests time and resources providing this open source code like some other libraries, please
  	respect the job and support open-source software.
    
    Written by Adrian for Blascarr
*/



#ifndef _TCS3200_H
#define _TCS3200_H

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "Arduino.h"
	#else
		#include "WProgram.h"
	#endif

    #include <EEPROM.h>

    // Frequency setting defines
	#define TCS3200_FREQ_HI  0 // 100% prescaler
	#define TCS3200_FREQ_MID 1 // 20%
	#define TCS3200_FREQ_LO  2 // 2%
	#define TCS3200_FREQ_OFF 3 // off

	// Indices for any RGB data/ filter selection
	#define TCS3200_RGB_R  0
	#define TCS3200_RGB_G  1
	#define TCS3200_RGB_B  2
	#define TCS3200_RGB_X  3 // 'Clear' filter selection

	#define RGB_SIZE  4 // array index counter limit
    #define SIZENAME 10
    #define SIZECOLORS 8
	
	//Pattern for DEBUG detection
	#ifndef DEBUG_STARTCMD 
		#define  DEBUG_STARTCMD "{" 
	#endif
	#ifndef DEBUG_ENDCMD 
		#define  DEBUG_ENDCMD "}"
	#endif
	#ifndef DEBUG_SEPCM 
		#define  DEBUG_SEPCMD ","
	#endif

	//Pattern for CALIBRATION detection
	#ifndef DEBUGCAL_STARTCMD 
		#define  DEBUGCAL_STARTCMD "<"
	#endif
	#ifndef DEBUGCAL_ENDCMD
		#define  DEBUGCAL_ENDCMD ">"
	#endif
	#ifndef DEBUGCAL_SEPCMD
		#define  DEBUGCAL_SEPCMD "|"
	#endif

	#if defined(SERIAL_DEBUG)
		#define SERIALDEBUG Serial
	#endif

	#if defined( MEGA_DEBUG) && ( defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__))
		#if MEGA_DEBUG == 1
			#define SERIALDEBUG Serial1
		#elif MEGA_DEBUG == 2
			#define SERIALDEBUG Serial2
		#elif MEGA_DEBUG == 3
			#define SERIALDEBUG Serial3
		#else 
			#define SERIALDEBUG Serial
		#endif
	#endif

	//Change Mode for receive and transmit data in different UART Transmission Systems like SoftwareSerial
	#if defined( REMOTE )
		#define SERIALREMOTE Serial

		#if defined( MEGA_DEBUG) && ( defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__))
			#if MEGA_DEBUG == 1
				#define SERIALREMOTE Serial1
			#elif MEGA_DEBUG == 2
				#define SERIALREMOTE Serial2
			#elif MEGA_DEBUG == 3
				#define SERIALREMOTE Serial3
			#else 
				#define SERIALREMOTE Serial
			#endif
		#endif
	#endif

	#if defined(SERIAL_DEBUG)
		#define  DUMP(s, v)  { SERIALDEBUG.print(F(s)); SERIALDEBUG.print(v); }
		#define  DUMP_CMD(s, v)  { SERIALDEBUG.print(DEBUG_STARTCMD); SERIALDEBUG.print(F(s)); SERIALDEBUG.print(v); SERIALDEBUG.print(DEBUG_ENDCMD); }
		#define  DUMPS(s)    {  SERIALDEBUG.print(F(s)); }
		#define  DUMPS_CMD(s)    { SERIALDEBUG.print(DEBUG_STARTCMD); SERIALDEBUG.print(F(s)); SERIALDEBUG.print(DEBUG_ENDCMD);}
		#define  DUMPPRINTLN() { SERIALDEBUG.println();}
	#else
		#define  DUMP(s, v)
		#define  DUMP_CMD(s, v) 
		#define  DUMPS(s)
		#define  DUMPS_CMD(s)
		#define  DUMPPRINTLN() 
	#endif

	//Calibration functions in DEBUG
	#ifdef DEBUG_CAL 
		#define  DUMPCAL(s, v)  { SERIALDEBUG.print(DEBUGCAL_STARTCMD); SERIALDEBUG.print(F(s)); SERIALDEBUG.print(v);SERIALDEBUG.print(DEBUGCAL_ENDCMD); }
		#define  DUMPSCAL(s)    { SERIALDEBUG.print(DEBUGCAL_STARTCMD); SERIALDEBUG.print(F(s)); SERIALDEBUG.print(DEBUGCAL_ENDCMD);}
		#define  DUMPDEBUGCAL(s, v)  { SERIALDEBUG.print(DEBUG_STARTCMD);SERIALDEBUG.print(F(s)); SERIALDEBUG.print(v);SERIALDEBUG.print(DEBUG_ENDCMD); }
		#define  DUMPSDEBUGCAL(s)    { SERIALDEBUG.print(DEBUG_STARTCMD);SERIALDEBUG.print(F(s)); SERIALDEBUG.print(DEBUG_ENDCMD);}
		#define  DUMPSAVAILABLE()    SERIALDEBUG.available()
		#define  DUMPCOLOR(code) { SERIALDEBUG.print(COLOR_STARTCMD);SERIALDEBUG.print(code);SERIALDEBUG.print(COLOR_ENDCMD);}
		#define  DUMPREADSTRING() {SERIALDEBUG.readString();}
		#define  DUMPREAD(str) { str =SERIALDEBUG.read();}
	#else
		#define  DUMPCAL(s, v)
		#define  DUMPSCAL(s)
		#define  DUMPDEBUGCAL(s, v)
		#define  DUMPSDEBUGCAL(s)    
		#define  DUMPSAVAILABLE() false
		#define  DUMPCOLOR(code) 
		#define  DUMPREADSTRING()   
		#define  DUMPREAD(str) 
	#endif

	/*SENDCOLOR
	0 - No Serial for DUMPCOLOR
	1 - Serial
	*/
	#ifndef SENDCOLOR
		#define  SENDCOLOR 0
	#endif

	//Pattern for COLOR detection
	#ifndef COLOR_STARTCMD 
		#define  COLOR_STARTCMD "#"
	#endif
	#ifndef COLOR_ENDCMD 
		#define  COLOR_ENDCMD "*"
	#endif
	#ifndef COLOR_SEPCMD
		#define  COLOR_SEPCMD "|"
	#endif

	#if SENDCOLOR == 1 && defined( SERIALREMOTE )
		#define  DUMPCOLOR(code) { SERIALREMOTE.print(COLOR_STARTCMD); SERIALREMOTE.print(code);SERIALREMOTE.print(COLOR_ENDCMD);}
	#elif SENDCOLOR == 0
		#define  DUMPCOLOR(code) { }
	#endif

    typedef struct{
		float value[RGB_SIZE];  // Raw data from the sensor
	} sensorData;

	typedef struct{
		uint8_t value[RGB_SIZE]; // the evaluated colour data (RGB value 0-255 or other)
	} colorData;
	typedef struct	{
		char    name[SIZENAME];  // color name 
		colorData rgb;    // RGB value
	} colorTable;

	class TCS3200 {
		public:

			uint8_t   _OUT;    // output enable pin
			uint8_t   _S0, _S1; // frequency scaler
			uint8_t   _S2, _S3; // photodiode filter selection
			uint8_t   _LED;
			uint8_t   _freqSet; 
			uint8_t   _nEEPROM=0;
			int 	  _lastColor = 0;
			int	      _nSamples = 10;
			char	  _ID[SIZENAME];
			
			colorTable _ct[SIZECOLORS]={
				colorTable {"WHITE", {255, 255, 255} },
				colorTable {"BLACK", {0, 0, 0} },
				colorTable {"YELLOW", {250, 250, 225} },
				colorTable {"ORANGE", {240, 200, 180} },
				colorTable {"RED", {250, 175, 190} },
				colorTable {"GREEN", {180, 220, 195} },
				colorTable {"BLUE", {150, 190, 220} },
				colorTable {"BROWN", {190, 170, 150} }
			};

			//typedef void ( TCS3200::*_MODE )();
				//_f_LID f_LIDUP  = &TCS3200::RGB_MODE;
  
			bool _interruption = false;

			unsigned long currentMillis,oldMillis;
			unsigned long refreshTime= 50; 

			sensorData  _raw;    // current raw sensor reading
			
			//Calibration white and Black
			sensorData  _relraw;    // current relative raw sensor reading
			sensorData  _darkraw;    // Dark Calibration values
			sensorData  _whiteraw;    // White Calibration values

			colorData _rgb;   // colour based data for current reading
			sensorData  _relrgb;    // current relative raw sensor reading
				
			TCS3200();
			TCS3200(uint8_t S2, uint8_t S3, uint8_t OUT, uint8_t nEEPROM = 0 );
			TCS3200(uint8_t S2, uint8_t S3, uint8_t OUT, uint8_t LED, uint8_t nEEPROM = 0 );
			TCS3200(uint8_t S2, uint8_t S3, uint8_t OUT, uint8_t S0, uint8_t S1,uint8_t LED, uint8_t nEEPROM = 0 );

			//  Data handling
			void  setPins(uint8_t S2, uint8_t S3, uint8_t OUT, uint8_t nEEPROM = 0);
			void  setPins(uint8_t S2, uint8_t S3, uint8_t OUT, uint8_t LED, uint8_t nEEPROM = 0);
			void  setPins(uint8_t S2, uint8_t S3, uint8_t OUT, uint8_t S0, uint8_t S1,uint8_t LED, uint8_t nEEPROM = 0 );
			void  setID(String ID);

			void  begin();        // used to initialise hardware
			void  LEDON(bool ledON);
			void  nSamples(int nSamples){_nSamples = nSamples;}
			void  setEEPROMaddress( uint8_t nEEPROM );
			
			void  voidRAW(sensorData *d);

			void  setRefreshTime(unsigned long refreshTime);
			void  setInterrupt(bool ON);

			void  setFilter(uint8_t f); // set the photodiode filter
			void  setFrequency(uint8_t f);// set frequency prescaler - default 100%	
			void  setRGBMode(bool _RGBMode);// set RGB Mode (true) or RAW Mode (false) in readings	

			void read(bool RGB = true);      // synchronously non-blocking reading value
			bool onChangeColor();
			sensorData  color();	//Single Reading
			sensorData  relativeColor(bool RGB = true);
			void  getRGB(colorData *rgb); // return RGB color data for the last reading
			void  getRaw(sensorData *d);  // return the raw data from the last reading
			sensorData readRAW();      // Read RAW Values
			colorData readRGB();      // Read RGB Values
			
			void sendColor();      
			String readColor();
			uint8_t readColorID();

			//Events for Calibration
			void  setDarkCal();
			void  setWhiteCal();
			void  calibration(uint8_t nEEPROM = 0);
			void  setColorCal();

			colorData  raw2RGB(void);  // convert raw data to RGB
			uint8_t checkColor(colorData *rgb);

			//EEPROM Saving Values
			void  saveCal(uint8_t nEEPROM = 0);
			void  loadCal(uint8_t nEEPROM = 0);
			void  loadBW(uint8_t nEEPROM = 0);
			void  saveCT(uint8_t nEEPROM = 0);
			void  loadCT(uint8_t nEEPROM = 0);
			void  readCT();
	};


	TCS3200::TCS3200(){
	  
	}

	TCS3200::TCS3200(uint8_t S2, uint8_t S3, uint8_t OUT, uint8_t nEEPROM ){
		_S2 = S2;
		_S3 = S3;
		_OUT = OUT;
		_freqSet = TCS3200_FREQ_HI;
		_nEEPROM = nEEPROM;
	}

	TCS3200::TCS3200(uint8_t S2, uint8_t S3, uint8_t OUT,uint8_t LED, uint8_t nEEPROM ){
		_S2 = S2;
		_S3 = S3;
		_OUT = OUT;
		_LED = LED;
		_freqSet = TCS3200_FREQ_HI;
		_nEEPROM = nEEPROM;
	}

	TCS3200::TCS3200(uint8_t S2, uint8_t S3, uint8_t OUT , uint8_t S0, uint8_t S1, uint8_t LED, uint8_t nEEPROM ){
		_S0 = S0;
		_S1 = S1;
		_S2 = S2;
		_S3 = S3;
		_OUT = OUT;
		_LED = LED;
		_freqSet = TCS3200_FREQ_HI;
		_nEEPROM = nEEPROM;
	}

	void TCS3200::begin(){
		
		#if defined(SERIAL_DEBUG)
			if(!SERIALDEBUG){
				SERIALDEBUG.begin(BPS);
				DUMPS(" NORMAL SERIAL DEBUG");
			}
		#endif

		#if MEGA_DEBUG || BT_DEBUG

			if(!SERIALDEBUG){
				SERIALDEBUG.begin(BPS);
			}

			#ifdef MEGA_DEBUG
				DUMPS(" MEGA DEBUG");
			#endif

			#ifdef BT_DEBUG
				DUMPS(" Bluetooth DEBUG");
			#endif

			#ifdef DEBUG_CAL
				DUMPS(" CALIBRATION DEBUG MODE ");
			#endif
		#endif

		if (_S0 != NULL )pinMode(_S0,OUTPUT);
		if (_S1 != NULL )pinMode(_S1,OUTPUT);
		pinMode( _S2, OUTPUT);
		pinMode( _S3, OUTPUT);
		pinMode( _LED, OUTPUT);
		pinMode( _OUT, INPUT);

		LEDON(true);
		setFrequency( _freqSet );

	}

	void  TCS3200::setRefreshTime(unsigned long refreshTime){
		TCS3200::refreshTime = refreshTime;
	}

	void  TCS3200::setEEPROMaddress( uint8_t nEEPROM ){
		TCS3200::_nEEPROM = nEEPROM;
	}

	void  TCS3200::LEDON(bool ledON){
		digitalWrite(_LED, (ledON) ? HIGH : LOW );
	}

	void  TCS3200::setID(String ID){
		ID.toCharArray(TCS3200::_ID, SIZENAME);
	}

	void TCS3200::setFrequency( uint8_t f ){
		_freqSet = f;
		/* set the sensor Frequiency 
			|	Freq 	|	S0 		|	S1 		|
			|	OFF 	|	LOW 	|	LOW 	|
			|	LOW 	|	LOW 	|	HIGH 	|
			|	MEDIUM 	|	HIGH 	|	LOW 	|
			|	HIGH 	|	HIGH 	|	HIGH 	|
		*/
		switch (f){
			case TCS3200_FREQ_HI:  DUMPS("HI");  digitalWrite(_S0, HIGH);  digitalWrite(_S1, HIGH);  break;
			case TCS3200_FREQ_MID: DUMPS("MID"); digitalWrite(_S0, HIGH);  digitalWrite(_S1, LOW);   break;
			case TCS3200_FREQ_LO:  DUMPS("LO");  digitalWrite(_S0, LOW);   digitalWrite(_S1, HIGH);  break;
			case TCS3200_FREQ_OFF: DUMPS("OFF"); digitalWrite(_S0, LOW);   digitalWrite(_S1, LOW);   break;
			default:  DUMP("Unknown freq option", f); break;
		}
	}

	void TCS3200::setFilter(uint8_t f){
		/* set the sensor color filter 
			|	RGB 	|	S2 		|	S3 		|
			|	R 		|	LOW 	|	LOW 	|
			|	G 		|	HIGH 	|	HIGH 	|
			|	B 		|	LOW 	|	HIGH 	|
			|	X 		|	HIGH 	|	LOW 	|
		*/

		switch (f){
			case TCS3200_RGB_R:   digitalWrite(_S2, LOW);   digitalWrite(_S3, LOW);   break;
			case TCS3200_RGB_G:   digitalWrite(_S2, HIGH);  digitalWrite(_S3, HIGH);  break;
			case TCS3200_RGB_B:   digitalWrite(_S2, LOW);   digitalWrite(_S3, HIGH);  break;
			case TCS3200_RGB_X:   digitalWrite(_S2, HIGH);  digitalWrite(_S3, LOW);   break;
			default:  DUMP("Unknown filter option", f); break;
		}
	}

	/*
		-------------------------------------------------------
		---------------------READINGS--------------------------
		-------------------------------------------------------
	*/
	void TCS3200::setRGBMode(bool _RGBMode){

	}

	void TCS3200::read(bool RGB) {
		TCS3200::currentMillis = millis();
		if(TCS3200::currentMillis-TCS3200::oldMillis >= TCS3200::refreshTime){

			if(RGB){
				TCS3200::readRGB();
			}else{
				TCS3200::readRAW();
			}

			TCS3200::oldMillis  = TCS3200::currentMillis;

		}
	}

	/*
		-------------------------------------------------------
		------------Non - Blocking Functions-------------------
		-------------------------------------------------------
	*/
	bool TCS3200::onChangeColor(){
		TCS3200::read();
		int cli= TCS3200::checkColor( &_rgb );

		if(cli != TCS3200::_lastColor){

			TCS3200::_lastColor = cli;
			#if SENDCOLOR && defined(SERIAL_DEBUG)
				DUMPCOLOR( readColor() );
			#endif
			return true;
		}else{
			return false;
		}
	}

	String TCS3200::readColor(){
		return _ct[ _lastColor ].name;
	}

	uint8_t TCS3200::readColorID(){
		return _lastColor;
	}

	sensorData TCS3200::color(){
		sensorData sensorcolor;   

		for (int i = 0; i < RGB_SIZE; ++i){

			float value = 0;
			float fvalue = 0;
			int n = 0;

				for (int j = 0; j < _nSamples; ++j){
					setFilter(i); 
					float color = pulseIn(_OUT, digitalRead(_OUT) == HIGH ? LOW : HIGH) ;

					// 1.5 value of threshold
					if (color/_nSamples > fvalue / 1.5) {

						fvalue = (fvalue + color)/_nSamples;
						value = value + color;
						n++;
					}

					sensorcolor.value[i] = value/n;  

				}

			}

			/*DUMP(" Red: ", color.value[0]);
			DUMP(" Green: ", color.value[1]);
			DUMP(" Blue: ", color.value[2]);  */
			return sensorcolor;
		}

		void TCS3200::voidRAW(sensorData *d) {
			for (int i = 0; i < RGB_SIZE; ++i){
				d->value[i] = 0;
			}
		}

		colorData TCS3200::readRGB() {
			TCS3200::readRAW();
			colorData color = TCS3200::raw2RGB();

			//DUMP(" Red : ",_rgb.value[TCS3200_RGB_R]);
			//DUMP(" Green : ",_rgb.value[TCS3200_RGB_G]);
			//DUMP(" Blue : ",_rgb.value[TCS3200_RGB_B]);
			return color;
		}

		sensorData TCS3200::readRAW() {
			sensorData cl;
			sensorData rawcl;
			TCS3200::voidRAW(&rawcl);

			rawcl = TCS3200::color();
			_raw.value[TCS3200_RGB_R] = rawcl.value[0];
			_raw.value[TCS3200_RGB_G] = rawcl.value[1];
			_raw.value[TCS3200_RGB_B] = rawcl.value[2];
			_raw.value[TCS3200_RGB_X] = rawcl.value[3];

			//DUMP(" Red : ",_raw.value[TCS3200_RGB_R]);
			//DUMP(" Green : ",_raw.value[TCS3200_RGB_G]);
			//DUMP(" Blue : ",_raw.value[TCS3200_RGB_B]);
			//DUMP(" Clear : ",_raw.value[TCS3200_RGB_X]);
			//DUMPPRINTLN();

			return rawcl;
		}

		sensorData  TCS3200::relativeColor(bool RGB){
			if (RGB){
				uint32_t sumcolor = _rgb.value[0]+_rgb.value[1]+_rgb.value[2];
				_relrgb.value[TCS3200_RGB_R] = _rgb.value[TCS3200_RGB_R]/sumcolor;
				_relrgb.value[TCS3200_RGB_G] = _rgb.value[TCS3200_RGB_G]/sumcolor;
				_relrgb.value[TCS3200_RGB_B] = _rgb.value[TCS3200_RGB_B]/sumcolor;
				//_relraw.value[TCS3200_RGB_X] = _raw.value[TCS3200_RGB_X]/sumcolor;

				DUMP(" RelRed : ",_relrgb.value[TCS3200_RGB_R]);
				DUMP(" RelGreen : ",_relrgb.value[TCS3200_RGB_G]);
				DUMP(" RelBlue : ",_relrgb.value[TCS3200_RGB_B]);
				//DUMP(" RelClear : ",_relraw.value[TCS3200_RGB_X]);
				return _relrgb;
			}else{
				uint32_t sumcolor = _raw.value[0]+_raw.value[1]+_raw.value[2];
				_relraw.value[TCS3200_RGB_R] = _raw.value[TCS3200_RGB_R]/sumcolor;
				_relraw.value[TCS3200_RGB_G] = _raw.value[TCS3200_RGB_G]/sumcolor;
				_relraw.value[TCS3200_RGB_B] = _raw.value[TCS3200_RGB_B]/sumcolor;
				//_relraw.value[TCS3200_RGB_X] = _raw.value[TCS3200_RGB_X]/sumcolor;

				DUMP(" RelRed : ",_relraw.value[TCS3200_RGB_R]);
				DUMP(" RelGreen : ",_relraw.value[TCS3200_RGB_G]);
				DUMP(" RelBlue : ",_relraw.value[TCS3200_RGB_B]);
				//DUMP(" RelClear : ",_relraw.value[TCS3200_RGB_X]);
				return _relraw;
			}
		}

		void TCS3200::getRGB(colorData *rgb){
			if (rgb == NULL)
			return;

			DUMPS("\ngetRGB");
			for (uint8_t i=0; i<RGB_SIZE; i++){
				rgb->value[i] = _rgb.value[i];
				DUMP(" ", rgb->value[i]);
			}
		}

		void TCS3200::getRaw(sensorData *d){
			// get the raw data of the current reading
			// useful to set dark and white calibration data
			if (d == NULL) return;

			DUMPS("\ngetRAW");
			for (uint8_t i=0; i<RGB_SIZE; i++){
				d->value[i] = _raw.value[i];
				DUMP(" ", d->value[i]);
			}
		}

		colorData TCS3200::raw2RGB(void){
			// Exploiting linear relationship to remap the range 
			int32_t x;
			colorData color;
			for (uint8_t i=0; i<RGB_SIZE; i++){
				x = (_raw.value[i] - _darkraw.value[i]) * 255;
				x /= (_whiteraw.value[i] - _darkraw.value[i]);

				// copy results back into the global structures
				if (x < 0) color.value[i] = 0; 
				else if (x > 255) color.value[i] = 255;
				else color.value[i] = x;
			}
			_rgb = color;
			return color;
		}

		uint8_t TCS3200::checkColor(colorData *rgb){
			int32_t   d;
			uint32_t  v, minV = 999999L;
			uint8_t   minI;

			for (uint8_t i=0; i< SIZECOLORS; i++){
				v = 0;
				for (uint8_t j=0; j<RGB_SIZE; j++){
					d = _ct[i].rgb.value[j] - rgb->value[j];
					v += (d * d);
				}
				if (v < minV) {
					minV = v;
					minI = i;
				}
				if (v == 0) break;   // perfect match, no need to search more
			}
			//DUMP("Color Checked : ", minI);
			//DUMPPRINTLN();
			return(minI);
		}

		/*void TCS3200::sendColor(){
		String code ;
		String str (TCS3200::_ID);
		if(str != ""){
		code += TCS3200::_ID;
		code += COLOR_SEPCMD;
		}
		code += TCS3200::_ct[TCS3200::_lastColor].name;
		DUMPCOLOR(code); 
		} */

		/*
			-------------------------------------------------------
			---------------------CALIBRATION MODE--------------------------
			-------------------------------------------------------
		*/

		void TCS3200::calibration(uint8_t nEEPROM){
			TCS3200::setDarkCal();
			TCS3200::setWhiteCal();
			DUMPREADSTRING();

			DUMPSDEBUGCAL("\nDo you want to save Calibration in EEPROM?");
			while(!DUMPSAVAILABLE()){
			}

			char readY;
			DUMPREAD(readY);
			DUMPDEBUGCAL("Char Read : ",readY);
			if (readY == 'Y'){
				DUMPDEBUGCAL("\nBlack and White Calibration saved in EEPROM Address:  ",TCS3200::_nEEPROM);
				DUMPPRINTLN();
				TCS3200::saveCal(nEEPROM);
			}

			DUMPREADSTRING();DUMPSDEBUGCAL("\nDo you want to set Color values (Y) or Load EEPROM Values (N)?");
			while(!DUMPSAVAILABLE()){
			}

			DUMPREAD(readY);
			DUMPDEBUGCAL("Char Read : ",readY);
			if (readY == 'Y'){
				DUMPSDEBUGCAL("\nSetting RGB Values");
				TCS3200::setColorCal();
				DUMPREADSTRING();DUMPSDEBUGCAL("\nDo you want to save Calibration Colors in EEPROM?\n");
				while(!DUMPSAVAILABLE()){
				}

				DUMPREAD(readY);
				DUMPDEBUGCAL("Char Read : ",readY);
				if (readY == 'Y'){
			  		DUMPDEBUGCAL("\nColour table saved in EEPROM in EEPROM Address:  ", TCS3200::_nEEPROM);
			  		TCS3200::saveCT(TCS3200::_nEEPROM);
				}else{
			  		DUMPSDEBUGCAL("\nNot saved in EEPROM");
				}
			}else if (readY == 'N'){
				DUMPSDEBUGCAL("\nEEPROM RGB Values");
				TCS3200::loadCT( nEEPROM );
			}

		}

		void TCS3200::setDarkCal(){
			sensorData darkcl;
			DUMPSDEBUGCAL(" BLACK Calibration ");
			DUMPPRINTLN();
			TCS3200::voidRAW(&darkcl);
			bool sure= false;
			while (sure == false){

				while(!DUMPSAVAILABLE()){

				}
				DUMPREADSTRING();

				darkcl = TCS3200::readRAW();
				DUMPSDEBUGCAL("RGB BLACK Values"); 

				String dataRGB = "";
				for (int i = 0; i < RGB_SIZE; ++i){
					dataRGB += darkcl.value[i];
					dataRGB += DEBUGCAL_SEPCMD;
				}
				DUMPCAL("",dataRGB ); 
				DUMPPRINTLN();

				DUMPDEBUGCAL("",dataRGB);
				DUMPPRINTLN();
				DUMPSDEBUGCAL(" Are you sure this values are correct for BLACK Calibration? (Y/N)");
				while(!DUMPSAVAILABLE()){

				}
				DUMPPRINTLN();
				char chr;
				DUMPREAD(chr);
				DUMPDEBUGCAL("Char Read : ",chr);
				if (chr == 'Y'){
					_darkraw = darkcl;
					sure = true;
				}
			}
			DUMPPRINTLN();
			DUMPSDEBUGCAL(" End BLACK Calibration");
		}


		void TCS3200::setWhiteCal(){
			sensorData whitecl;
			DUMPPRINTLN();
			DUMPSDEBUGCAL(" WHITE Calibration ");
			TCS3200::voidRAW(&whitecl);
			bool sure= false;
			while (sure == false){

				while(!DUMPSAVAILABLE()){

				}
				DUMPREADSTRING();

				whitecl = TCS3200::readRAW();
				DUMPSDEBUGCAL("RGB WHITE Values"); 

				String dataRGB = "";
				for (int i = 0; i < RGB_SIZE; ++i){
					dataRGB += whitecl.value[i];
					dataRGB += DEBUGCAL_SEPCMD;
				}
				DUMPCAL("",dataRGB ); 
				DUMPDEBUGCAL("",dataRGB);
				DUMPPRINTLN();
				DUMPSDEBUGCAL(" Are you sure this values are correct for WHITE Calibration? (Y/N)");
				while(!DUMPSAVAILABLE()){

				}
				DUMPPRINTLN();
				char chr;
				DUMPREAD(chr);
				DUMPDEBUGCAL("Char Read : ",chr);
				if (chr == 'Y'){
					_whiteraw = whitecl;
					sure = true;
				}
			}

			DUMPSDEBUGCAL(" End WHITE Calibration");
		}

		void TCS3200::setColorCal(){

			DUMPSDEBUGCAL(" Color Calibration ");

			while(!DUMPSAVAILABLE()){ }

			DUMPREADSTRING();
			for (int i = 0; i < SIZECOLORS; ++i){
				bool sure= false;
				while (sure == false){
					DUMPPRINTLN();
					DUMPDEBUGCAL("Calibration for color - ", _ct[i].name);
					DUMPPRINTLN();
					colorData colorcl = TCS3200::readRGB();

					String dataRGB = "";
					for (int i = 0; i < RGB_SIZE; ++i){
						dataRGB += colorcl.value[i];
						dataRGB += DEBUGCAL_SEPCMD;
					}
					DUMPCAL("",dataRGB );
					DUMPPRINTLN();
					DUMPSDEBUGCAL("Is Correct? ");
					while(!DUMPSAVAILABLE()){
					}

					char readY;
					DUMPREAD(readY);
					DUMPDEBUGCAL("Char Read : ",readY);
					DUMPPRINTLN();

					if (readY == 'Y'){
						_ct[i].rgb = colorcl;
						sure = true;
					}else if(readY == 'N'){
						//sure = true;
					}
					DUMPREADSTRING();
				}
			}
		}

		void  TCS3200::saveCal(uint8_t nEEPROM){
			_nEEPROM = nEEPROM;
			int calWhiteAddress = _nEEPROM;
			int calDarkAddress = calWhiteAddress + sizeof(sensorData);
			EEPROM.put(calWhiteAddress, _whiteraw);
			EEPROM.put(calDarkAddress, _darkraw);

			DUMPS("\t White Calibration: ");
			for (uint8_t i=0; i<4; i++){
				DUMP(" RGB[ ", i);
				DUMP(" ] - ", _whiteraw.value[i]);
				DUMPS("\t");
			}
			DUMPS("\n \t Black Calibration: ");
			for (uint8_t i=0; i<4; i++){
				DUMP(" RGB[ ", i);
				DUMP(" ] - ", _darkraw.value[i]);
				DUMPS("\t");
			}

			DUMPS("\nSaved Calibration");
		}

		void  TCS3200::loadCal(uint8_t nEEPROM){
			TCS3200::loadBW(nEEPROM);
			TCS3200::loadCT(nEEPROM);
		}

		void  TCS3200::loadBW(uint8_t nEEPROM){
			_nEEPROM = nEEPROM;
			int calWhiteAddress = _nEEPROM;
			int calDarkAddress = calWhiteAddress + sizeof(sensorData);

			EEPROM.get(calWhiteAddress, _whiteraw);
			EEPROM.get(calDarkAddress, _darkraw);

			DUMPS("\n \t White Calibration: ");
			for (uint8_t i=0; i<4; i++){
				DUMP("RGB [",i);
				DUMP("] - ", _whiteraw.value[i]);
				DUMPS("\t");
			}
			DUMPS("\n \tBlack Calibration: ");
			for (uint8_t i=0; i<4; i++){
				DUMP("RGB [",i);
				DUMP("] - ", _darkraw.value[i]);
				DUMPS("\t");
			}
			DUMPS("\nCalibration Loaded ");
		}

		void  TCS3200::saveCT(uint8_t nEEPROM ){
			int address = nEEPROM + 2*sizeof(sensorData);

			for (int i = 0; i < SIZECOLORS; ++i){
				EEPROM.put(address, _ct[i]);
				address += sizeof(colorTable);
			}
			TCS3200::readCT();

			DUMPS("\n Color Table Calibration Saved ");
			DUMPPRINTLN();
		}

		void  TCS3200::loadCT(uint8_t nEEPROM ){
			int address = nEEPROM + 2*sizeof(sensorData);

			for (int i = 0; i < SIZECOLORS; ++i){
				EEPROM.get(address, _ct[i]);
				address += sizeof(colorTable);
			}
			TCS3200::readCT();

			DUMPS("\n Color Table Calibration Loaded ");
		}

		void  TCS3200::readCT(){
			for (int i = 0; i < SIZECOLORS; ++i){
				DUMP("\nColor: ",_ct[i].name);
				for (uint8_t j=0; j<3; j++){
					DUMP("  -  ", _ct[i].rgb.value[j]);
				}
				DUMPS("\n");
			}
		}
#endif