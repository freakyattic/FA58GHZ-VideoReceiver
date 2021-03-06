/*
	FA58GHZ.ino
 
	Created: 2015-11-08 19:38:50
	Author: FreakyAttic.com
 
 -------------------------------------------------------------------------------------
	 The MIT License (MIT)

	 Copyright (c) 2015 FreakyAttic.com

	 Permission is hereby granted, free of charge, to any person obtaining a copy
	 of this software and associated documentation files (the "Software"), to deal
	 in the Software without restriction, including without limitation the rights
	 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	 copies of the Software, and to permit persons to whom the Software is
	 furnished to do so, subject to the following conditions:

	 The above copyright notice and this permission notice shall be included in all
	 copies or substantial portions of the Software.

	 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	 SOFTWARE.
	 
*/ 

#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <stdint.h>
#include <TVout.h>
#include <fontALL.h>

#include "Definitions.h"
#include "RTC6715.h"


/*****************************************************/
/*		GLOBAL VARIABLES							 */
/*****************************************************/
	
	unsigned long	_millisLED;		//LED status
	unsigned long	_millisFAN, _millisFANMode;	

	uint8_t		eep_VideoInput;		// 0: RF1, 1:RF2, 2:EXT-Video, 3:Diversity

	uint8_t		eep_RFchannel;		//EEPROM channel saved.
	uint16_t	eep_RSSIMax;
	uint16_t	eep_RSSIMin;
	
	uint8_t		eep_FANTimeInc;		//FAN Increment
	uint8_t		eep_FANPower;		//FAN Power
	uint8_t		eep_FANMode;		//FAN Mode
	
	uint8_t		eep_BattEnable;		//Battery Monitor
	uint8_t		eep_BattType;
	uint16_t	eep_BattCell;
	uint16_t	epp_BattCalib;
	
	stateButton		_buttonMode = sbutton_idle;
	stateButton		_buttonNext = sbutton_idle;
	stateButton		_buttonFan  = sbutton_idle;
	
/*****************************************************/
/*		FUNCTIONS									 */
/*****************************************************/
void setup ( void )
{	
//Pin out Initialization
	
	//RF Module
		pinMode (slaveSelectPin, OUTPUT);
		pinMode (spiDataPin, OUTPUT);
		pinMode (spiClockPin, OUTPUT);
		
		pinMode (PowerRF, OUTPUT);
			digitalWrite(PowerRF, HIGH);	//Power OFf
			
	//Video Multiplexer
		pinMode (VideoSel0, OUTPUT);
		pinMode (VideoSel1, OUTPUT);
			digitalWrite(VideoSel0, LOW);	//IN0 selected
			digitalWrite(VideoSel1, LOW);
	
	//Buttons
		pinMode(pinbuttonMode, INPUT);
			digitalWrite(pinbuttonMode, INPUT_PULLUP);
		pinMode(pinbuttonNext, INPUT);
			digitalWrite(pinbuttonNext, INPUT_PULLUP);
		pinMode(pinbuttonFan, INPUT);
			digitalWrite(pinbuttonFan, INPUT_PULLUP);			
	
	// Buzzer
		pinMode(buzzer, OUTPUT);
		pinMode(buzzern, OUTPUT);
		digitalWrite(buzzer, LOW);
		digitalWrite(buzzern, LOW);
	
	//LED	
		pinMode(led, OUTPUT);
			digitalWrite(led, HIGH);

	
	//External Fan
		pinMode(pinFan, OUTPUT); 
			digitalWrite(pinFan, HIGH);

	//Ini Beeping
		for(char i = 0; i<6; i++)
		{
			digitalWrite(led, HIGH);
			beep(10);
			digitalWrite(led, LOW);
			delay(100);
		}
		
	//EEPROM Initialization	
		Load_EEPROM();

	//RF Module ini
		RF_Ini(eep_RFchannel);

	// OSD Initialization
		OSD_Ini();
		
	//Finish Initialization
		_debug(F("Board initialized ..."));
		
	_millisLED = millis();	//LED status
	_millisFAN = millis();				
}

void loop()
{
	
//Button Tasks
	Buttons_Tasks();
	
//OSD Tasks
	OSD_Tasks();

//FAN Tasks
	FAN_Tasks();	
	
//LED Status
	if(millis() > _millisLED)
	{
		_millisLED = millis() + 500;
		if(digitalRead(led))
		digitalWrite(led, LOW);
		else
		digitalWrite(led, HIGH);
		
	}
}


/*****************************************************/
/*		FUNCTIONS									 */
/*****************************************************/

void beep(uint16_t time)
{
	unsigned long  targetms;
	
	targetms = millis() + time;
	while( millis() < targetms )
	{
		//Toggle the pins
		digitalWrite(buzzer, HIGH);
		digitalWrite(buzzern, LOW);
		delayMicroseconds(100);
		digitalWrite(buzzer, LOW);
		digitalWrite(buzzern, HIGH);
		delayMicroseconds(100);
	}
	
	digitalWrite(buzzer, LOW); //Disable Buzzer
	digitalWrite(buzzern, LOW);
}

void	_debug	( const __FlashStringHelper *description )
{
#ifdef DEBUG
	static char ini = 0;
	if(!ini)
	{
		Serial.begin(115200);
		Serial.print(F("\n\n\n\rFA58GHZ RF Video Module - Ver. "));
		Serial.println(FIRMWAREVER);
		Serial.print(F("   By Freakyattic.com - "));
		Serial.println(F(__DATE__));
		Serial.println(F(" "));
		
		ini++;
	}
	Serial.println(description);
#endif
}

void	Load_EEPROM		( void )
{
	uint16_t	_address;
	uint8_t		_value0, _value1;
	
	//IF EEPROM structure changed, modify EEPROM_VER in Definitions.h
	
	_address = 0;
	_value0 = EEPROM.read(_address++);
	
	//Check if different version, then Factory Default.
	if( (_value0 != EEPROM_VER) || ( !digitalRead(pinbuttonMode)&!digitalRead(pinbuttonFan)))
	{
		_debug(F("EEPROM: Loading default values"));
		_address = 0;
		EEPROM.write(_address++, EEPROM_VER);
		
		//Default values
		eep_VideoInput	= 0;
		eep_RFchannel	= 0x15;		//RF
		eep_RSSIMax		= 340;
		eep_RSSIMin		= 145;
		eep_FANTimeInc	= 3;		//FAN
		eep_FANPower	= 0;
		eep_FANMode		= 0;
		eep_BattEnable	= 0;		//Battery
		eep_BattType	= 2;
		eep_BattCell	= 3000;
		epp_BattCalib	= 3167;
		
		Save_EEPROM();
	}
	
	//Load Data
	eep_VideoInput	= EEPROM.read(_address++);
	eep_RFchannel	= EEPROM.read(_address++);
	eep_RSSIMax		= ((EEPROM.read(_address++)<<8) | (EEPROM.read(_address++)));
	eep_RSSIMin		= ((EEPROM.read(_address++)<<8) | (EEPROM.read(_address++)));
	eep_FANTimeInc	= EEPROM.read(_address++);
	eep_FANPower	= EEPROM.read(_address++);
	eep_FANMode		= EEPROM.read(_address++);
	eep_BattEnable	= EEPROM.read(_address++);
	eep_BattType	= EEPROM.read(_address++);
	eep_BattCell	= ((EEPROM.read(_address++)<<8) | (EEPROM.read(_address++)));
	epp_BattCalib	= ((EEPROM.read(_address++)<<8) | (EEPROM.read(_address++)));

	_debug(F("EEPROM: Configuration loaded"));
}

void	Save_EEPROM		( void )
{
	uint8_t		_address = 1;
	
//Save Data
	EEPROM.write(_address++,eep_VideoInput);
	EEPROM.write(_address++,eep_RFchannel);				//RF
	EEPROM.write(_address++,highByte(eep_RSSIMax));	
	EEPROM.write(_address++,lowByte(eep_RSSIMax));
	EEPROM.write(_address++,highByte(eep_RSSIMin));
	EEPROM.write(_address++,lowByte(eep_RSSIMin));
	EEPROM.write(_address++,eep_FANTimeInc);			//FAN
	EEPROM.write(_address++,eep_FANPower);
	EEPROM.write(_address++,eep_FANMode);
	EEPROM.write(_address++,eep_BattEnable);			//Battery
	EEPROM.write(_address++,eep_BattType);
	EEPROM.write(_address++,highByte(eep_BattCell));
	EEPROM.write(_address++,lowByte(eep_BattCell));
	EEPROM.write(_address++,highByte(epp_BattCalib));
	EEPROM.write(_address++,lowByte(epp_BattCalib));

	_debug(F("EEPROM: Configuration saved"));
}

void	VideoSelect		( uint8_t _input)
{
	switch(_input)
	{
		case 1:			//RF Module 1
			RF_ChannelSet(eep_RFchannel);	//Set saved channel
			OSDScreen_Off();
			digitalWrite(VideoSel0, HIGH);
			digitalWrite(VideoSel1, LOW);
			break;
		case 2:			//RF Module 2
			RF_ChannelSet(eep_RFchannel);	//Set saved channel
			OSDScreen_Off();
			digitalWrite(VideoSel0, LOW);
			digitalWrite(VideoSel1, HIGH);
			break;
		case 3:			//Ext Video
			OSDScreen_Off();
			digitalWrite(VideoSel0, HIGH);
			digitalWrite(VideoSel1, HIGH);
			break;
		default:
		case 0:			//OSD
			OSDScreen_Main();
			digitalWrite(VideoSel0, LOW);
			digitalWrite(VideoSel1, LOW);
			break;
	}
}

/*****************************************************/
/*		Buttons  									 */
/*****************************************************/

void	Buttons_Tasks		( void )
{
	static unsigned long	_millisButtMode = 0;
	static unsigned long	_millisButtNext = 0;
	static unsigned long	_millisButtFan  = 0;
	
//Button Mode
	if(!digitalRead(pinbuttonMode))	
	{
		if(_millisButtMode == 0)
		{
			_millisButtMode = millis();
		}
	}
	else
	{
		if(_millisButtMode != 0)
		{
			_millisButtMode = millis() - _millisButtMode;
			
			if(_millisButtMode > BUTTON_GLITCHES)
			{
				if(_millisButtMode < BUTTON_CLICK)
				{
					_buttonMode = sbutton_click;
				}
				else if(_millisButtMode < BUTTON_HOLD)
				{
					_buttonMode = sbutton_hold;
				}
				else if(_millisButtMode < BUTTON_HOLDLONG)
				{
					_buttonMode = sbutton_holdlong;
				}
			}
			_millisButtMode = 0;
		}
	}
	
//Button Next
	if(!digitalRead(pinbuttonNext))
	{
		if(_millisButtNext == 0)
		{
			_millisButtNext = millis();
		}
	}
	else
	{
		if(_millisButtNext != 0)
		{
			_millisButtNext = millis() - _millisButtNext;
		
			if(_millisButtNext > BUTTON_GLITCHES)
			{
				if(_millisButtNext < BUTTON_CLICK)
				{
					_buttonNext = sbutton_click;
				}
				else if(_millisButtNext < BUTTON_HOLD)
				{
					_buttonNext = sbutton_hold;
				}
				else if(_millisButtNext < BUTTON_HOLDLONG)
				{
					_buttonNext = sbutton_holdlong;
				}
			}
			_millisButtNext = 0;
		}
	}	

//Button FAN
	if(!digitalRead(pinbuttonFan))
	{
		if(_millisButtFan == 0)
		{
			_millisButtFan = millis();
		}
	}
	else
	{
		if(_millisButtFan != 0)
		{
			_millisButtFan = millis() - _millisButtFan;
			
			if(_millisButtFan > BUTTON_GLITCHES)
			{
				if(_millisButtFan < BUTTON_CLICK)
				{
					_buttonFan = sbutton_click;
				}
				else if(_millisButtFan < BUTTON_HOLD)
				{
					_buttonFan = sbutton_hold;
				}
				else if(_millisButtFan < BUTTON_HOLDLONG)
				{
					_buttonFan = sbutton_holdlong;
				}
			}
			_millisButtFan = 0;
		}
	}
}

// BUT ------------------------------------------------------

	void	Clear_ButtonStates	( void )
	{
		_buttonMode = sbutton_idle;
		_buttonNext = sbutton_idle;
		_buttonFan = sbutton_idle;
	}

// MODE BUT ------------------------------------------------------
	uint8_t	isbuttonMode_Click ( void )
	{
		if(_buttonMode== sbutton_click)
		{
			Clear_ButtonStates();	// Only one button allowed at a time
			beep(15);
			return 1;
		}
		return 0;
	}

	uint8_t	isbuttonMode_Hold ( void )
	{
		if(_buttonMode== sbutton_hold)
		{
			Clear_ButtonStates();	// Only one button allowed at a time
			beep(15);
			return 1;
		}
		return 0;
	}

	uint8_t	isbuttonMode_HoldLong ( void )
	{
		if(_buttonMode== sbutton_holdlong)
		{
			Clear_ButtonStates();	// Only one button allowed at a time
			beep(15);
			return 1;
		}
		return 0;
	}

// NEXT BUT ------------------------------------------------------
	uint8_t	isbuttonNext_Click ( void )
	{
		if(_buttonNext == sbutton_click)
		{
			Clear_ButtonStates();	// Only one button allowed at a time
			beep(15);
			return 1;
		}
		return 0;
	}

	uint8_t	isbuttonNext_Hold ( void )
	{
		if(_buttonNext== sbutton_hold)
		{
			Clear_ButtonStates();	// Only one button allowed at a time
			beep(15);
			return 1;
		}
		return 0;
	}

	uint8_t	isbuttonNext_HoldLong ( void )
	{
		if(_buttonNext== sbutton_holdlong)
		{
			Clear_ButtonStates();	// Only one button allowed at a time
			beep(15);
			return 1;
		}
		return 0;
	}

// FAN BUT ------------------------------------------------------
	uint8_t	isbuttonFan_Click ( void )
	{
		if(_buttonFan == sbutton_click)
		{
			Clear_ButtonStates();	// Only one button allowed at a time
			beep(15);
			return 1;
		}
		return 0;
	}

	uint8_t	isbuttonFan_Hold ( void )
	{
		if(_buttonFan== sbutton_hold)
		{
			Clear_ButtonStates();	// Only one button allowed at a time
			beep(15);
			return 1;
		}
		return 0;
	}

	uint8_t	isbuttonFan_HoldLong ( void )
	{
		if(_buttonFan== sbutton_holdlong)
		{
			Clear_ButtonStates();	// Only one button allowed at a time
			beep(15);
			return 1;
		}
		return 0;
	}

	uint8_t	isbutton_AnyPressed( void )
	{
		if( (_buttonFan != sbutton_idle) | (_buttonMode != sbutton_idle) | (_buttonNext != sbutton_idle))
		{
			Clear_ButtonStates();	// Only one button allowed at a time
			beep(15);
			return 1;
		}
		return 0;
	}


/****************************************************/
/*		FAN Tasks									*/
/****************************************************/

void	FAN_Tasks		( void )
{
	static uint8_t  currentPower = FANPOWER_0+1;
	static uint8_t  selectedPower = FANPOWER_0;
	
	if( millis() > _millisFAN)
	{
		if( currentPower != FANPOWER_0)
		{
			currentPower = FANPOWER_0;
			analogWrite(pinFan, currentPower); //off
		}
	}
	else
	{
		if( eep_FANMode == 0)	//Cont.
		{
			if( currentPower != selectedPower)
			{
				currentPower = selectedPower;
				analogWrite(pinFan, currentPower); //Full On	
			}
		}
		else
		{
			if( millis() > _millisFANMode)		//Fan mode
			{
				if( currentPower != selectedPower)
					currentPower = selectedPower;
				else
					currentPower = FANPOWER_0;
					
				analogWrite(pinFan, currentPower);
				
				//Add time
				_millisFANMode = millis();
				switch(eep_FANMode)
				{
					case 1:	_millisFANMode +=  5000; break;
					case 2:	_millisFANMode += 10000; break;
					case 3:	_millisFANMode += 20000; break;
					case 4:	_millisFANMode += 30000; break;
				}
			}
		}
	}

//Click	- Add time
	if(isbuttonFan_Click())
	{
		
	//Increment Timer value		
		if( millis() > _millisFAN) 
			_millisFAN = millis();
		switch(eep_FANTimeInc)
		{
			case 0:	_millisFAN +=  60000; break;
			case 1:	_millisFAN += 120000; break;
			case 2:	_millisFAN += 300000; break;
			case 3:	_millisFAN += 480000; break;
			case 4:	_millisFAN += 600000; break;
		}
		
	//Obtain the selected power
		switch(eep_FANPower)
		{
			case 0:	selectedPower = FANPOWER_100; break; //100%
			case 1:	selectedPower = FANPOWER_80 ; break; //80%
			case 2:	selectedPower = FANPOWER_60 ; break; //60%
			case 3:	selectedPower = FANPOWER_40 ; break; //40%
			case 4:	selectedPower = FANPOWER_20 ; break; //20%
		}
	}
	
//Hold - Stop
	if(isbuttonFan_Hold())
	{
		_millisFAN = millis();
		_millisFANMode = millis();
	}
}

/****************************************************/
/*		Battery Voltage (mV)						*/
/****************************************************/
	uint16_t	BatteryVolt		( void ) 
	{
		static uint8_t test=0;
		unsigned long	_tot = 0;
		
		for( int _x=0; _x < BATT_AVERAGE; _x++)
		{
			_tot += (unsigned long)analogRead(Vin);
		}
		
		_tot /= BATT_AVERAGE;
		//return (uint16_t)_tot;
		return 10120+(test++);
	}
	
	
