/*
	FA58GHZ.ino
 
	Created: 2015-11-08 19:38:50
	Author: FreakyAttic.com
 
	Description XXXXXXXXXXXXXXXXXXXXXX
	XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
 
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

#include <TVout.h>
#include <fontALL.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <stdint.h>

#include "Definitions.h"
#include "RTC6715.h"


/*****************************************************/
/*		GLOBAL VARIABLES							 */
/*****************************************************/
	
	unsigned long	_millisLED;		//LED status

	uint8_t		eep_RFchannel;		//EEPROM channel saved.
	
	stateButton		_pinbuttonMode = sbutton_idle;
	stateButton		_pinbuttonNext = sbutton_idle;
	stateButton		_pinbuttonFan = sbutton_idle;
	
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
		pinMode(FanON, OUTPUT); 
			digitalWrite(FanON, HIGH);

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
}

void loop()
{
	static char _val = 0;
	static char _val1 = 0;

////Button 1 test
	//if(digitalRead(pinbuttonMode) == LOW)
	//{
		//VideoMux(_val1 ^= 1);	//Togle input 0-1
		//
		//beep(10);
		////Bounce
		//delay(100);
		//while(digitalRead(pinbuttonMode) == LOW);
		//delay(100);
	//}
//
//
////Button 3 test
	//if(digitalRead(pinbuttonFan) == LOW)
	//{
		//if(_val ^= 1)
			//RF_ChannelSet(0x15);
		//else
			//RF_ChannelSet(0x58);		
			//
		//Serial.print("Channel ");
		//Serial.println(RF_ChannelGet(), HEX);
		//
		//beep(10);
		////Bounce
		//delay(100);
		//while(digitalRead(pinbuttonFan) == LOW);
		//delay(100);
	//}
	
//Button Tasks
	Buttons_Tasks();
	
//OSD Tasks
	OSD_Tasks();
	
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
		Serial.println(F("By Freakyattic.com\n"));
		ini++;
	}
	Serial.println(description);
	#endif
}

void	Load_EEPROM		( void )
{
	eep_RFchannel = 0x15;
}

void	Save_EEPROM		( void )
{
	
}

void	VideoMux		( uint8_t _input)
{
	switch(_input)
	{
		case 1:			//RF Module
			OSDScreen_Off();
			digitalWrite(VideoSel0, HIGH);
			digitalWrite(VideoSel1, LOW);
			break;
		case 2:
			OSDScreen_Off();
			digitalWrite(VideoSel0, LOW);
			digitalWrite(VideoSel1, HIGH);
			break;
		case 3:
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
	
//Button Mode
	if(!digitalRead(pinbuttonMode))	
	{
		if(_millisButtMode == 0)
		{
			beep(15);
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
					_debug(F("click"));
					_pinbuttonMode = sbutton_click;
				}
				else if(_millisButtMode < BUTTON_HOLD)
				{
					_debug(F("hold"));
					_pinbuttonMode = sbutton_hold;
				}
				else if(_millisButtMode < BUTTON_HOLDLONG)
				{
					_debug(F("hold long"));
					_pinbuttonMode = sbutton_holdlong;
				}
			}
			_millisButtMode = 0;
		}
	}
}

uint8_t	ispinbuttonMode_Click ( void )
{
	if(_pinbuttonMode== sbutton_click)
	{
		_pinbuttonMode = sbutton_idle;
		return 1;
	}
	return 0;
}

uint8_t	ispinbuttonMode_Hold ( void )
{
	if(_pinbuttonMode== sbutton_hold)
	{
		_pinbuttonMode = sbutton_idle;
		return 1;
	}
	return 0;
}

uint8_t	ispinbuttonMode_HoldLong ( void )
{
	if(_pinbuttonMode== sbutton_holdlong)
	{
		_pinbuttonMode = sbutton_idle;
		return 1;
	}
	return 0;
}