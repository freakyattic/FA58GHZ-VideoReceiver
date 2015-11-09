/*
 * OSDMenus.ino
 *
 * Created: 2015-11-09 19:25:08
 *  Author: FreakyAttic.com
 
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

	#include "OSDMenus.h"
	
/*********************************************************************************************************/
/*							VARIABLES																	 */
/*********************************************************************************************************/

	TVout			TV;
	OSDSTATES		OSD_State;		//Start with main OSD
	
	uint8_t			_selMenu = 0;	//selected menu
	
	unsigned long	delayMenuTimeout;

/*********************************************************************************************************/
/*							FUNCTIONS																	 */
/*********************************************************************************************************/

	void	OSD_Ini		( void )
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

		TV.clear_screen();
		
		OSD_State = OSDs_Main;		//Start with the Intro screen.
	}

	void	OSD_Tasks	( void )
	{
		switch(OSD_State)
		{
	//Off
			case OSDs_Off:
				TV.pause();	//switch off OSD
				_debug(F("OSD menu off"));
				OSD_State = OSDs_OffUpdate;
				break;
			case OSDs_OffUpdate:
			
				//Automatic button functions.
				
				break;
				
	//Intro - Welcome screen
			case OSDs_Intro:
			
				//Place information about the channel selected.
				
				break;
			case OSDs_IntroUpdate:
				break;				
				
	//Main Screen				
			case OSDs_Main:
				TV.resume();
				TV.clear_screen();
				
				TV.draw_rect(0,0,TV_WIDTH-1,TV_HEIGHT-1,2);
				TV.select_font(font4x6);
				TV.printPGM(20,3,PSTR("Configuration Menu"));
				TV.draw_line(0,11,TV_WIDTH-1,11,2);

				TV.select_font(font6x8);
				TV.printPGM(5,20,PSTR("Video Input Select."));
				TV.printPGM(5,30,PSTR("RF Manual Setup"));
				TV.printPGM(5,40,PSTR("RF Scanner"));
				TV.printPGM(5,50,PSTR("Fan Output"));
				TV.printPGM(5,60,PSTR("RSSI Calibration"));
				TV.printPGM(5,70,PSTR("Exit"));
				
				OSD_State = OSDs_MainUpdate;

				break;
			case OSDs_MainUpdate:
				
				//delay(1000);
				//TV.draw_rect(2,18,TV_WIDTH-5,10, 1, 2);
				//TV.draw_rect(2,58,TV_WIDTH-5,10, 1, 2);
				//delay(1000);
				//TV.draw_rect(2,18,TV_WIDTH-5,10, 0, 2);
				//TV.draw_rect(2,58,TV_WIDTH-5,10, 0, 2);
					//
				break;
				
	//Video Input selection		
			case OSDs_VideoInput:
				break;
			case OSDs_VideoInputUpdate:
				break;	
		}
	
	
	}
	
	void	OSDScreen_Main	( void )
	{
		if(OSD_State != OSDs_MainUpdate)
			OSD_State = OSDs_Main;		
	}
	
	void	OSDScreen_Off	( void )
	{
		if(OSD_State != OSDs_OffUpdate)
			OSD_State = OSDs_Off;
	}

