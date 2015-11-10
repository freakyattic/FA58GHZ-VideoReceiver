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
	
	uint8_t			_selMenu, _selMenuPrev, _selMenuMax;	//selected menu
	
	unsigned long	_millisMenu, _millisRSSI, _millisSearch;

/*********************************************************************************************************/
/*							PROTOTYPES																	 */
/*********************************************************************************************************/

	void	SelectedMenu_Ini	( uint8_t _selected, uint8_t _maxItems);
	void	SelectedMenu_Next	();
	uint8_t	SelectedMenu_Get	();
	
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
		
		_debug(F("OSD: OSD initialized"));
	}

	void	OSD_Tasks	( void )
	{
		uint8_t				_val0;
		static uint8_t		_prevIndex;
		static uint8_t		_autoInc;
		
		switch(OSD_State)
		{
	//Off
			case OSDs_Off:
				_debug(F("OSD: OSD off"));
				Clear_ButtonStates();
				VideoSelect(eep_VideoInput+1); //Select the Video input and switch off OSD
				TV.pause();	//switch off OSD
				beep(80);
				OSD_State = OSDs_OffUpdate;
				break;
			case OSDs_OffUpdate:
			
				//Enter Setup
				if( isbuttonMode_Hold() | isbuttonMode_HoldLong())
					OSD_State = OSDs_Main;
				
				////Select next channel
				//if( isbuttonNext_Click())
				
				//// Automatic Scann
				//if( isbuttonNext_Hold())
					
				break;
				
	//Intro - Welcome screen
			case OSDs_Intro:
			
				Clear_ButtonStates();
				VideoSelect(0);		//Select OSD
				TV.clear_screen();
				TV.draw_rect(0,0,TV_WIDTH-1,TV_HEIGHT-1,2);
				
				//Information
				TV.select_font(font8x8ext);
				TV.printPGM(35, 18,PSTR("FA58GHZ"));
				TV.select_font(font4x6);
				TV.printPGM(10, 40,PSTR("5.8GHz Video Rx - Ver."));
				TV.print(FIRMWAREVER);
				TV.printPGM(32, 50,PSTR("FreakyAttic.com"));
				
				//Place information about the channel selected.
				TV.draw_line(0,74, TV_WIDTH-1, 74, 1);
				TV.printPGM(12, 82,PSTR("Channel:"));
				TV.set_cursor(50, 82);
				TV.print(eep_RFchannel,HEX);
				TV.printPGM(PSTR(" - "));
				TV.print(RF_GetFrequencyFromChannel(eep_RFchannel));
				TV.printPGM(PSTR(" - "));
				TV.print(Read_RSSI(), DEC);
				TV.printPGM(PSTR("%"));
				
				_millisMenu = millis()+MENU_TIMEOUT;
				OSD_State = OSDs_IntroUpdate;
				break;
			case OSDs_IntroUpdate:
				if(millis() > _millisMenu)
					OSD_State = OSDs_Main;
				break;				
				
	//Main Screen				
			case OSDs_Main:
				
				Clear_ButtonStates();
				VideoSelect(0);		//Select OSD			
					
				TV.resume();
				TV.clear_screen();
				
				TV.draw_rect(0,0,TV_WIDTH-1,TV_HEIGHT-1,2);
				TV.select_font(font4x6);
				TV.printPGM(20,3,PSTR("Configuration Menu"));
				TV.draw_line(0,11,TV_WIDTH-1,11,2);

				TV.select_font(font6x8);
				TV.printPGM(5,20,PSTR("Video Input Select."));
				TV.printPGM(5,30,PSTR("RF Channel Setup"));
				TV.printPGM(5,40,PSTR("RF Spectrum"));
				TV.printPGM(5,50,PSTR("Fan Output"));
				TV.printPGM(5,60,PSTR("RSSI Calibration"));
				TV.printPGM(5,70,PSTR("Exit"));
				
				SelectedMenu_Ini(5,6);
				
				_millisMenu = millis()+MENU_TIMEOUT;
				OSD_State = OSDs_MainUpdate;
				break;
			case OSDs_MainUpdate:
				
				//Timeout - No user input
				if(millis() > _millisMenu)
				{
					OSD_State = OSDs_Off;
				}
				
				if(isbuttonNext_Click())
				{
					_millisMenu = millis()+MENU_TIMEOUT;
					SelectedMenu_Next();
				}
					
				if(isbuttonMode_Click() | isbuttonNext_Hold())
				{
					beep_Confirmation();
					_millisMenu = millis()+MENU_TIMEOUT;
					switch(SelectedMenu_Get())
					{
						case 0:		//Video Select
							OSD_State = OSDs_VideoInput;
							break;
						case 1:		//RF Manual Setup
							OSD_State = OSDs_RFManual;
							break;						
						case 2:		//RF Spectrum
							delay(1000);
							beep(19);
							RF_ChannelSet(0x15);
							break;
						case 3:		//Fan Output
							delay(1000);
							beep(19);
							RF_ChannelSet(0x58);
							break;
						case 4:		//RSSI Calibration
							break;
						case 5:		//Exit
							OSD_State = OSDs_Off;
							break;	
					}
				}
				break;
				
	//Video Input selection		
			case OSDs_VideoInput:
			
				Clear_ButtonStates();
				TV.resume();
				TV.clear_screen();
			
				//Box
				TV.draw_rect(0,0,TV_WIDTH-1,TV_HEIGHT-1,2);
				TV.select_font(font4x6);
				TV.printPGM(20,3,PSTR("Video Input Menu"));
				TV.draw_line(0,11,TV_WIDTH-1,11,2);

				//Menu
				TV.select_font(font6x8);
				TV.printPGM(5,20,PSTR("  RF Video 1"));
				TV.printPGM(5,30,PSTR("  RF Video 2"));
				TV.printPGM(5,40,PSTR("  Ext. Video"));
				TV.printPGM(5,50,PSTR("  RF Diversity"));
				TV.printPGM(5,60,PSTR("Exit"));
				
				//Mark the selected Input
				TV.printPGM(5,(20+eep_VideoInput*10),PSTR(">"));
				SelectedMenu_Ini(4, 5);
			
				_millisMenu = millis()+MENU_TIMEOUT;
				OSD_State = OSDs_VideoInputUpdate;
				break;
			case OSDs_VideoInputUpdate:
			
				//Timeout - No user input
				if(millis() > _millisMenu)
					OSD_State = OSDs_Main;
				
				if(isbuttonNext_Click())
				{
					_millisMenu = millis()+MENU_TIMEOUT;
					SelectedMenu_Next();
				}
				
				if(isbuttonMode_Click())
				{
					_millisMenu = millis()+MENU_TIMEOUT;
					_val0 = SelectedMenu_Get();
					
					if(_val0 == 4)	//Exit
					{
						OSD_State = OSDs_Main;
						break;
					}

					if( _val0 == 3)	//Diversity not supported yet
						_val0 = 0;
						
					//Save the value
					eep_VideoInput = _val0;
					Save_EEPROM();
					beep_Confirmation();
					OSD_State = OSDs_Main;
				}
				break;	
				
//OSD RF Manual				
			case OSDs_RFManual:
				Clear_ButtonStates();
				TV.resume();
				TV.clear_screen();
			
				//Place Key Info	
				TV.select_font(font6x8);
				TV.printPGM(10,10,PSTR("Button Key Info"));
				TV.select_font(font4x6);
				TV.printPGM(12,25,PSTR("Next Click: Ch+"));
				TV.printPGM(12,35,PSTR("     Hold : Scan"));
				TV.printPGM(12,50,PSTR("Mode Click: Exit"));
				TV.printPGM(12,60,PSTR("     Hold : Save"));
				
				TV.printPGM(5,90,PSTR("Press any to continue..."));
				_millisMenu = millis()+5000;
				OSD_State = OSDs_RFManualUpdate0;
				break;
			case OSDs_RFManualUpdate0:				
				//Wait some time
				if((millis() >_millisMenu) | isbutton_AnyPressed())
				{
					OSD_State = OSDs_RFManualUpdate1;
					
				//Print the menu
					TV.clear_screen();
					
				//Box
					TV.draw_rect(0,0,TV_WIDTH-1,35,1);
					TV.select_font(font4x6);
					TV.printPGM(20,3,PSTR("RF Channel Selection"));
					TV.draw_line(0,11,TV_WIDTH-1,11,1);
					
				//Channel
					TV.select_font(font6x8);
					TV.printPGM(5,15,PSTR("Channel: "));
					TV.printPGM(5,25,PSTR("RSSI"));
					
				//Draw Spectrum Box
					TV.draw_rect(11,37,104,50,1);
					TV.select_font(font4x6);
					TV.set_cursor(5,90);
					TV.print((int)RF_GetFrequencyFromChannel(0x34));	//Low Freq
					TV.set_cursor(55,90);
					TV.print((int)RF_GetFrequencyFromChannel(0x24));	//Mid Freq
					TV.set_cursor(105,90);
					TV.print((int)RF_GetFrequencyFromChannel(0x38));	//Mid Freq
					
				//Update screen Values
					TV.select_font(font6x8);
					TV.set_cursor(60, 15);
					TV.print(RF_ChannelGet(),HEX);
					TV.print(" - ");
					TV.print((int)RF_GetFrequencyFromChannel(RF_ChannelGet()));
					
					_prevIndex = 255;
					_autoInc = 0;	
					_millisSearch = millis();
					_millisRSSI   = millis();
					_millisMenu   = millis()+60000;
				}
				break;
			case OSDs_RFManualUpdate1:
			
			//Update RSSI
				if( millis() > _millisRSSI)
				{
					_millisRSSI = millis()+250;
					uint16_t _rssi = (unsigned long)RF_RSSIGet();
					
					//Update Bar and text
					TV.draw_rect( 33, 25, 75, 6, 1, 0);
					TV.draw_rect( 33, 25, map(_rssi, 0, 100, 0, 75), 6, 1, 1);
					TV.set_cursor(112, 25);
					TV.print(_rssi);
					
					//Spectrum bar
					uint8_t _index = map(RF_FrequencyGet(), 5645, 5945, 0, 100);
					TV.draw_line(13 + _index, 85, 13 + _index, 39, 0);	//Clear
					TV.draw_line(13 + _index, 85, 13 + _index, 39 + map(_rssi,0,100,46,0), 1);	//Draw
					
					//Frequency and Cursor
					if(_prevIndex != _index)
					{
						//Update screen Values
						TV.select_font(font6x8);
						TV.set_cursor(60, 15);
						TV.print(RF_ChannelGet(),HEX);
						TV.print(" - ");
						TV.print((int)RF_GetFrequencyFromChannel(RF_ChannelGet()));
						
						//Cursor
						if(_prevIndex != 255)
						{	
							TV.set_pixel(13 + _prevIndex, 37, 2);
							TV.set_pixel(13 + _prevIndex, 87, 2);
						}
							
						TV.set_pixel(13 + _index, 37, 2);
						TV.set_pixel(13 + _index, 87, 2);
						_prevIndex = _index;
					}
				}
				
			// Tasks Search
				if(_autoInc & (millis()>_millisSearch) )
				{
					_millisSearch = millis() + 120;
					
					RF_ChannelInc();
					delay(50);	//some time to stabilize
					
					//Stop autosearch 75%
					if( RF_RSSIGet() >= 75)
					{
						_autoInc = 0;
						delay(20);
						beep(15);
						delay(20);
						beep(15);
					}
				}
				
			//Next channel
				if(isbuttonNext_Click())
				{
					_millisMenu = millis()+60000;
					
					RF_ChannelInc();
					delay(10);	//some time to stabilize
				}
				
			//Search				
				if(isbuttonNext_Hold())
					_autoInc ^= 1;	//toggle the autoInc/search
					
			//Exit									
				if(isbuttonMode_Click())
				{
					RF_ChannelSet(eep_RFchannel);	//Cancel the modifications
					OSD_State = OSDs_Main;	
				}
				
			//Save and Exit
				if(isbuttonMode_Hold())		
				{
					beep_Confirmation();
					eep_RFchannel = RF_ChannelGet();	//Save the channel
					Save_EEPROM();
					OSD_State = OSDs_Main;
				}
					
			//Timeout - No user input
				if(millis() > _millisMenu)
					OSD_State = OSDs_Main;	
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

	void	SelectedMenu_Ini ( uint8_t _selected, uint8_t _maxItems)
	{
		_selMenu = _selected;
		_selMenuPrev = _selMenu;
		_selMenuMax = _maxItems;
		
		//Select Menu
		TV.draw_rect(2,(18 + _selected*10),TV_WIDTH-5,10, 1, 2);
	}
	
	void	SelectedMenu_Next ()
	{
		if(++_selMenu == _selMenuMax)
			_selMenu = 0;
		
		//DeSelect Previous
		TV.draw_rect(2,(18 + _selMenuPrev*10),TV_WIDTH-5,10, 0, 2);
		
		//Select Menu
		TV.draw_rect(2,(18 + _selMenu*10),TV_WIDTH-5,10, 1, 2);
		
		_selMenuPrev = _selMenu;
	}
	
	uint8_t	SelectedMenu_Get ()
	{
		return _selMenu;
	}
