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

#include "Definitions.h"


/*****************************************************/
/*		GLOBAL VARIABLES							 */
/*****************************************************/
	TVout TV;
	
	unsigned long	_millisLED;		//LED status

void setup()
{

//Pinout Initialization
	
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
		pinMode(buttonMode, INPUT);
			digitalWrite(buttonMode, INPUT_PULLUP);
		pinMode(buttonAction, INPUT);
			digitalWrite(buttonAction, INPUT_PULLUP);
		pinMode(buttonFAN, INPUT);
			digitalWrite(buttonFAN, INPUT_PULLUP);			
	
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
					
// OSD Initialization

	OSD_Ini();
	
	TV.clear_screen();
	TV.select_font(font4x6);
	TV.printPGM(0,0,PSTR("TEST SCREEN"));
	TV.select_font(font6x8);
	TV.printPGM(0,10,PSTR("TEST SCREEN"));
	TV.select_font(font8x8);
	TV.printPGM(0,20,PSTR("TEST SCREEN"));
	TV.select_font(font8x8ext);
	TV.printPGM(0,30,PSTR("TEST SCREEN"));
	
//Finish Initialization

	_millisLED = millis();	//LED status				
}

void loop()
{
	static char _val = 0;
	static char _val1 = 0;

//Button 1 test
	if(digitalRead(buttonMode) == LOW)
	{
		if(!_val1)
		digitalWrite(VideoSel0, HIGH);
		else
		digitalWrite(VideoSel0, LOW);
		
		_val1 ^= 1;
		
		beep(10);
		//Bounce
		delay(100);
		while(digitalRead(buttonMode) == LOW);
		delay(100);
	}


//Button 3 test
	if(digitalRead(buttonFAN) == LOW)
	{
		if(!_val)
			TV.pause();
		else
		{
			TV.resume();
		}
		
		_val ^= 1;
		
		beep(10);
		//Bounce
		delay(100);
		while(digitalRead(buttonFAN) == LOW);
		delay(100);
	}

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

void OSD_Ini ( void)
{
	char _value = 0;
	_value = TV.begin(PAL);

	if (_value > 0) {	//ERROR with TV
		while (true) {
			digitalWrite(led, HIGH);
			delay(300);
			digitalWrite(led, LOW);
			delay(300);
		}
	}
		
	TV.force_outstart(-6);
	TV.force_linestart(41);
}