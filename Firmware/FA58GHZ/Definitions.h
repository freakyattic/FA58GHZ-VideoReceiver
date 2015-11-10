/*
	Definitions.h
 
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


#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

/*********************************************************************************************************/
/*							CONFIGURATIONS																 */
/*********************************************************************************************************/

	#define		FIRMWAREVER		"1.0b"	// Firmware Version
	#define		EEPROM_VER		0x31	// EEPROM Data storage version

	#define		DEBUG		// Uncomment for serial debug.

	#define		TV_WIDTH		128		//Screen width, MUST BE multiple of 8
	#define		TV_HEIGHT		96		//Screen height

/*********************************************************************************************************/
/*							PIN DEFINITIONS																 */
/*********************************************************************************************************/

	// TV OUT				D7,D9	pins reserved

//RF Module
	#define spiDataPin		10	//	To control the RF module
	#define slaveSelectPin	11
	#define spiClockPin		12

	#define rssiPin1		A6
	#define rssiPin2		A7
	
	#define PowerRF			A4	
	
//Video Multiplexer	
	#define VideoSel0		A0
	#define VideoSel1		A1

//Buttons
	#define pinbuttonMode	2
	#define pinbuttonNext	3
	#define pinbuttonFan	4	//For the FAN
	
//Buzzer
	#define buzzer			A2
	#define buzzern			A3
	#define buzzer_toggle()	PORTc ^= 0b00001100	//toggle pins

//LED
	#define led				13	

//External Fan Controller
	#define FanON			9	//could be PWM controlled
	
		
//Voltage Input monitor	
	#define Vin				A5

/*********************************************************************************************************/
/*							OTHER DEFINITIONS															 */
/*********************************************************************************************************/

	#define		beep_Confirmation()		beep(20);delay(200);beep(20)	//Confirmation Sequence

	#define     BUTTON_GLITCHES			50      //Minimum value to validate a click
	#define     BUTTON_CLICK			600     //Minimum value for Click
	#define     BUTTON_HOLD				3000    //Minimum value for Hold
	#define     BUTTON_HOLDLONG			4000    //Minimum value for Hold Long
	
	enum stateButton
	{
		sbutton_idle,
		sbutton_click,
		sbutton_hold,
		sbutton_holdlong,
	};

#endif /* DEFINITIONS_H_ */