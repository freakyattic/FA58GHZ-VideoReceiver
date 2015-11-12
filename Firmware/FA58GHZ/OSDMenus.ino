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
	
	uint8_t			_selMenu, _selMenuPrev, _selMenuMax, _selWidth;	//selected menu
	
	unsigned long	_millisMenu, _millisRSSI, _millisSearch;
	uint16_t		_spectrumRSSI_Max, _spectrumRSSI_Min;

/*********************************************************************************************************/
/*							PROTOTYPES																	 */
/*********************************************************************************************************/

	void	SelectedMenu_Ini	( uint8_t _selected, uint8_t _maxItems);
	void	SelectedMenu_Ini	( uint8_t _selected, uint8_t _maxItems,  uint8_t _width);
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
		
		OSD_State = OSDs_Intro;		//Start with the Intro screen.
		
		_debug(F("OSD: OSD initialized"));
	}

	void	OSD_Tasks	( void )
	{
		uint8_t				_val0;
		uint16_t			_val16_0, _val16_1;
		static uint8_t		_prevIndex;
		static uint8_t		_Update;
		static uint8_t		_autoInc;
		static uint8_t		_fanInc, _fanPower, _fanMode;
		static uint8_t		_batEnable, _batType;
		static uint16_t		_batCell, _batCalib;
		
		switch(OSD_State)
		{
//Off ------------------------------------------------------
			case OSDs_Off:
				_debug(F("OSD: OSD off"));
				Clear_ButtonStates();
				VideoSelect(eep_VideoInput+1); //Select the Video input and switch off OSD
				TV.pause();	//switch off OSD
				OSD_State = OSDs_OffUpdate;
				break;
			case OSDs_OffUpdate:
			
				//Enter Setup
				if( isbuttonMode_Hold() | isbuttonMode_HoldLong() | isbuttonNext_Hold())
					OSD_State = OSDs_Main;
				
				////Select next channel
				//if( isbuttonNext_Click())
				
				//// Automatic Scann
				//if( isbuttonNext_Hold())
					
				break;
				
//Intro - Welcome screen------------------------------------------------------
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
				TV.draw_line(0,84, TV_WIDTH-1, 84, 1);
				TV.set_cursor(10, 87);
				TV.print(RF_ChannelGet(),HEX);
				TV.printPGM(PSTR(" - "));
				TV.print(RF_FrequencyGet());
				TV.printPGM(PSTR(" -"));
				
				_millisRSSI = millis();
				_millisMenu = millis()+MENU_TIMEOUT;
				OSD_State = OSDs_IntroUpdate;
				break;
			case OSDs_IntroUpdate:
			
				if((millis() > _millisMenu)|isbutton_AnyPressed())
					OSD_State = OSDs_Main;
				
				//Update RSSI
				if(millis() > _millisRSSI)
				{
					_millisRSSI = millis() + 250;
					
					//RSSI
					TV.set_cursor(60, 87);
					TV.printPGM(PSTR("    "));	//Clean
					TV.set_cursor(60, 87);
					TV.print(RF_RSSIGet(), DEC);
					TV.printPGM(PSTR("%"));
					
					//Voltage
					TV.set_cursor(95, 87);
					TV.print("     ");
					TV.set_cursor(95, 87);
					_val16_1 = BatteryVolt();
					_val16_0 = _val16_1/1000;
					TV.print(_val16_0);
					TV.print(".");
					_val16_0 = (_val16_1 - _val16_0*1000)/10;
					if(_val16_0<10)	TV.print("0");
					TV.print(_val16_0);
					TV.print("v");
				}
				break;				
				
//Main Screen	------------------------------------------------------
			case OSDs_Main:
			
				RF_ChannelSet(eep_RFchannel);	//Select the saved channel
				
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
				TV.printPGM(5,60,PSTR("Battery Monitor"));
				TV.printPGM(5,70,PSTR("Exit"));
				
				//Place information about the channel selected at the bottom
				TV.select_font(font4x6);
				TV.draw_line(0,84, TV_WIDTH-1, 84, 1);
				TV.set_cursor(10, 87);
				TV.print(RF_ChannelGet(),HEX);
				TV.printPGM(PSTR(" - "));
				TV.print(RF_FrequencyGet());
				TV.printPGM(PSTR(" -"));
				
				SelectedMenu_Ini(5,6);
				_millisRSSI = millis();
				_millisMenu = millis()+MENU_TIMEOUT;
				OSD_State = OSDs_MainUpdate;
				break;
			case OSDs_MainUpdate:
				
			//Timeout - No user input
				if(millis() > _millisMenu)
				{
					OSD_State = OSDs_Off;
				}
				
			//Update RSSI
				if(millis() > _millisRSSI)
				{
					TV.select_font(font4x6);
					
					_millisRSSI = millis() + 250;
					
					//RSSI
					TV.set_cursor(60, 87);
					TV.printPGM(PSTR("     "));	//Clean
					TV.set_cursor(60, 87);
					TV.print(RF_RSSIGet(), DEC);
					TV.printPGM(PSTR("%"));
					
					//Voltage
					TV.set_cursor(95, 87);
					TV.print("     ");
					TV.set_cursor(95, 87);
					_val16_1 = BatteryVolt();
					_val16_0 = _val16_1/1000;
					TV.print(_val16_0);
					TV.print(".");
					_val16_0 = (_val16_1 - _val16_0*1000)/10;
					if(_val16_0<10)	TV.print("0");
					TV.print(_val16_0);
					TV.print("v");
				}
				
			//Next Menu
				if(isbuttonNext_Click())
				{
					_millisMenu = millis()+MENU_TIMEOUT;
					SelectedMenu_Next();
				}
					
			//Menu Selected
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
							OSD_State = OSDs_RFSpectrum;
							break;
						case 3:		//Fan Output
							OSD_State = OSDs_FANMenu;
							break;
						case 4:		//Battery Monitor
							OSD_State = OSDs_BatteryMon;
							break;
						case 5:		//Exit
							OSD_State = OSDs_Off;
							break;	
					}
				}
				break;
				
//Video Input selection	------------------------------------------------------	
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
				
				if(isbuttonMode_Click()|isbuttonNext_Hold())
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
					
					TV.set_cursor(40, 80);
					TV.printPGM(PSTR("SAVED !!!"));
					delay(1500);
					
					OSD_State = OSDs_Main;
				}
				break;	
				
//OSD RF Manual	------------------------------------------------------			
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
				
				RF_ChannelSet(eep_RFchannel);
				
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
					_millisMenu   = millis()+MENU_TIMEOUTLONG;
				}
				break;
			case OSDs_RFManualUpdate1:
			
			//Update RSSI
				if( millis() > _millisRSSI)
				{
					_millisRSSI = millis()+250;
					uint16_t _rssi = (unsigned long)RF_RSSIGet();
					
					//Update Bar and text
					TV.draw_rect( 30, 25, 75, 6, 1, 0);
					TV.draw_rect( 30, 25, map(_rssi, 0, 100, 0, 75), 6, 1, 1);
					
					TV.printPGM(108, 25,PSTR("   "));	//Clean
					TV.set_cursor(108, 25);
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
					_millisMenu = millis()+MENU_TIMEOUTLONG;
					
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
					
					TV.set_cursor(40, 45);
					TV.printPGM(PSTR("SAVED !!!"));
					delay(1500);
					
					OSD_State = OSDs_Main;
				}
					
			//Timeout - No user input
				if(millis() > _millisMenu)
					OSD_State = OSDs_Main;	
				break;
				
//RF Spectrum	------------------------------------------------------			
			case OSDs_RFSpectrum:
			
				Clear_ButtonStates();
				TV.resume();
				TV.clear_screen();
				
				TV.draw_rect(0,0,TV_WIDTH-1, 11,2);
				TV.select_font(font4x6);
				TV.printPGM(20,3,PSTR("RF spectrum analyzer"));
				
				//Draw Spectrum Box
				TV.draw_rect(11,15,104,70,1);
				TV.select_font(font4x6);
				TV.set_cursor(5,89);
				TV.print((int)RF_GetFrequencyFromChannel(0x34));	//Low Freq
				TV.set_cursor(55,89);
				TV.print((int)RF_GetFrequencyFromChannel(0x24));	//Mid Freq
				TV.set_cursor(105,89);
				TV.print((int)RF_GetFrequencyFromChannel(0x38));	//Mid Freq

				_spectrumRSSI_Min = eep_RSSIMax;
				_spectrumRSSI_Max = eep_RSSIMin;
				
				_prevIndex = 255;
				_millisSearch = millis();
				_millisMenu = millis() + MENU_TIMEOUTLONG;
				OSD_State = OSDs_RFSpectrumUpdate;
				break;
				
			case OSDs_RFSpectrumUpdate:
			
				if(millis() > _millisSearch)
				{
					_millisSearch = millis() + 60;
					
					RF_FrequencyNext();
					delay(40);	//some time to stabilize
					
					uint16_t _rssi = (unsigned long)RF_RSSIGet();
					
					//Spectrum bar
					uint8_t _index = map(RF_FrequencyGet(), 5645, 5945, 0, 100);
					TV.draw_line(13 + _index, 85, 13 + _index, 17, 0);	//Clear
					TV.draw_line(13 + _index, 85, 13 + _index, 17 + map(_rssi,0,100,66,0), 1);	//Draw
					
					//Frequency and Cursor
					if(_prevIndex != _index)
					{
						//Cursor
						if(_prevIndex != 255)
						{
							TV.set_pixel(13 + _prevIndex, 15, 2);
							TV.set_pixel(13 + _prevIndex, 85, 2);
						}
						
						TV.set_pixel(13 + _index, 15, 2);
						TV.set_pixel(13 + _index, 85, 2);
						_prevIndex = _index;
					}
					
					//Detect Max-Min RSSI Values
					_rssi = (unsigned long)RF_RSSIGet_Raw();
					
					if(_rssi < _spectrumRSSI_Min)
						_spectrumRSSI_Min = _rssi;
					if(_rssi > _spectrumRSSI_Max)
						_spectrumRSSI_Max = _rssi;
				}
				
				if(isbuttonMode_Hold())
				{
					Clear_ButtonStates();
					TV.resume();
					TV.clear_screen();
					
					TV.draw_rect(0,0,TV_WIDTH-1, 11,2);
					TV.select_font(font4x6);
					TV.printPGM(20,3,PSTR("RSSI Calibration"));

					TV.select_font(font6x8);					
					TV.set_cursor(5, 20);
					TV.printPGM(PSTR("Saved   : "));
					TV.print((int)eep_RSSIMin);
					TV.printPGM(PSTR(" - "));
					TV.println((int)eep_RSSIMax);
					
					TV.set_cursor(5, 30);
					TV.printPGM(PSTR("Detected: "));
					TV.print((int)_spectrumRSSI_Min);
					TV.printPGM(PSTR(" - "));
					TV.println((int)_spectrumRSSI_Max);
					
					TV.set_cursor(10, 50);
					TV.printPGM(PSTR("Hold Mode to Save"));
					TV.set_cursor(10, 60);
					TV.printPGM(PSTR("any to cancel"));
					
					_millisMenu = millis() + MENU_TIMEOUTLONG;	
					OSD_State = OSDs_RFSpectrumRSSI;
				}
				
				//Timeout - No user input
				if((millis() > _millisMenu)|isbutton_AnyPressed())
					OSD_State = OSDs_Main;
				
				break;
			case OSDs_RFSpectrumRSSI:

				//Save parameters
				if(isbuttonMode_Hold())
				{
					eep_RSSIMin = _spectrumRSSI_Min;
					eep_RSSIMax = _spectrumRSSI_Max;
					
					Save_EEPROM();
					beep_Confirmation();
					
					TV.set_cursor(40, 80);
					TV.printPGM(PSTR("SAVED !!!"));
					delay(1500);
					
					OSD_State = OSDs_Main;
				}
				
				//Timeout - No user input
				if((millis() > _millisMenu)|isbutton_AnyPressed())
					OSD_State = OSDs_Main;
					
				break;

//FAN Options	------------------------------------------------------				
			case OSDs_FANMenu:
			
				Clear_ButtonStates();
				TV.resume();
				TV.clear_screen();
				
				TV.draw_rect(0,0,TV_WIDTH-1, TV_HEIGHT-1, 1);
				TV.select_font(font4x6);
				TV.printPGM(20,3,PSTR("FAN Options"));
				TV.draw_line(0,11,TV_WIDTH-1,11,2);
				
				//Menu
				TV.select_font(font6x8);
				TV.printPGM(5,20,PSTR("Timer Inc. :"));
				TV.printPGM(5,30,PSTR("Fan Speed  :"));
				TV.printPGM(5,40,PSTR("Fan Mode   :"));
				TV.printPGM(5,50,PSTR("Exit"));

				//Mark the selected Input
				SelectedMenu_Ini(3, 4, 75);
				
				_fanInc = eep_FANTimeInc;	//Load saved parameters
				_fanPower = eep_FANPower;
				_fanMode = eep_FANMode;
				
				_Update = 1;		//Update screen parametes
					
				_millisMenu = millis() + MENU_TIMEOUT;
				OSD_State = OSDs_FANMenuUpdate;
				break;
				
			case OSDs_FANMenuUpdate:
			
			//Next menu 
				if(isbuttonNext_Click())
				{
					_millisMenu = millis()+MENU_TIMEOUT;
					SelectedMenu_Next();
				}
			
			//Save Parameters if exit with mode hold
				if(SelectedMenu_Get()==3)
				{
					if(isbuttonMode_Hold()|isbuttonNext_Hold())
					{
						//Save eeprom parameters
						eep_FANTimeInc = _fanInc;	//Load saved parameters
						eep_FANPower = _fanPower;
						eep_FANMode = _fanMode;
						
						Save_EEPROM();
						beep_Confirmation();
						
						TV.set_cursor(40, 80);
						TV.printPGM(PSTR("SAVED !!!"));
						delay(1500);
						
						_millisFAN = millis();		//Stop FAN, so new parameters can be used
						_millisFANMode = millis();
						
						OSD_State = OSDs_Main;
					}
				}
			
			//Menu pressed
				if(isbuttonMode_Click()|isbuttonNext_Hold())
				{

					switch(SelectedMenu_Get())
					{
						case 0:			//Fan Inc
							if(++_fanInc >= 5)
								_fanInc=0;
							break;
						case 1:			//Fan Power
							if(++_fanPower >= 5)
								_fanPower=0;
							break;
						case 2:			//Fan Mode
							if(++_fanMode >= 5)
								_fanMode=0;
							break;						
						default:		//Exit
						case 3:
							OSD_State = OSDs_Main;
							break;
					}
					
					_Update = 1;		//Update screen parametes
					_millisMenu = millis()+MENU_TIMEOUT;					
				}						
				
			//Update Screen values.
				if(_Update)
				{
					_Update = 0;
			
					//FAN INC
					TV.printPGM(80,20,PSTR("      "));//Clean
					switch(_fanInc)
					{
						case 0:
							TV.printPGM(80,20,PSTR("1 min"));
							break;
						case 1:
							TV.printPGM(80,20,PSTR("2 min"));
							break;
						case 2:
							TV.printPGM(80,20,PSTR("5 min"));
							break;
						case 3:
							TV.printPGM(80,20,PSTR("8 min"));
							break;
						case 4:
							TV.printPGM(80,20,PSTR("10 min"));
							break;
					}
					
					//FAN POWER
					TV.printPGM(80,30,PSTR("     "));//Clean
					switch(_fanPower)
					{
						case 0:
							TV.printPGM(80,30,PSTR("100 %"));
							break;
						case 1:
							TV.printPGM(80,30,PSTR("80 %"));
							break;
						case 2:
							TV.printPGM(80,30,PSTR("60 %"));
							break;
						case 3:
							TV.printPGM(80,30,PSTR("40 %"));
							break;
						case 4:
							TV.printPGM(80,30,PSTR("20 %"));
							break;
					}
					
					//FAN Mode
					TV.printPGM(80,40,PSTR("      "));//Clean
					switch(_fanMode)
					{
						case 0:
						TV.printPGM(80,40,PSTR("Cont."));
						break;
						case 1:
						TV.printPGM(80,40,PSTR("5 sec"));
						break;
						case 2:
						TV.printPGM(80,40,PSTR("10 sec"));
						break;
						case 3:
						TV.printPGM(80,40,PSTR("20 sec"));
						break;
						case 4:
						TV.printPGM(80,40,PSTR("30 sec"));
						break;
					}
				}
			
				//Timeout - No user input
				if(millis() > _millisMenu)
					OSD_State = OSDs_Main;
				break;
				
//Batter Monitor	------------------------------------------------------				
			case OSDs_BatteryMon:
				Clear_ButtonStates();
				TV.resume();
				TV.clear_screen();
				
				TV.draw_rect(0,0,TV_WIDTH-1, TV_HEIGHT-1, 1);
				TV.select_font(font4x6);
				TV.printPGM(20,3,PSTR("Battery Monitor"));
				TV.draw_line(0,11,TV_WIDTH-1,11,2);
				
				//Menu
				TV.select_font(font6x8);
				TV.printPGM(5,20,PSTR("Bat. Alarm :"));
				TV.printPGM(5,30,PSTR("Bat. Cells :"));
				TV.printPGM(5,40,PSTR("Cell Alarm :"));
				TV.printPGM(5,50,PSTR("Calibrate  :"));
				TV.printPGM(5,60,PSTR("Exit"));

				//Mark the selected Input
				SelectedMenu_Ini(4, 5, 75);
				
				//Load Values
				_batEnable = eep_BattEnable;
				_batType = eep_BattType;
				_batCell = eep_BattCell;
				_batCalib = epp_BattCalib;
				
				_Update = 1;		//Update screen parameters
				
				_millisMenu = millis() + MENU_TIMEOUT;
				OSD_State = OSDs_BatteryMonUpdate;
				break;		
			case OSDs_BatteryMonUpdate:
				
			//Next menu
				if(isbuttonNext_Click())
				{
					_millisMenu = millis()+MENU_TIMEOUT;
					SelectedMenu_Next();
				}
			
			//Timeout - No user input
				if(millis() > _millisMenu)
					OSD_State = OSDs_Main;
					
			//Mode Hold
				if(isbuttonMode_Click()|isbuttonNext_Hold())
				{

					switch(SelectedMenu_Get())
					{
						case 0:				// Bat alarm On/Off
							_batEnable ^= 1;
							break;	
						case 1:				//Cells
							if(_batType==2)
								_batType = 3;
							else
								_batType = 2;
							break;
						case 2:				//Warning
							_Update = 1;
							_val0 = 1;
							TV.printPGM(80,40,PSTR(">    <"));
							while(_val0)
							{
								Buttons_Tasks();
								
								if(_Update)
								{
									_Update = 0;
									TV.printPGM(86,40,PSTR("    "));
									TV.set_cursor(86,40);
									_val16_0 = _batCell/1000;
									TV.print(_val16_0);
									TV.print(".");
									_val16_0 = (_batCell - _val16_0*1000)/10;
									if(_val16_0<10)	TV.print("0");
									TV.print(_val16_0);
								}
								
								if(isbuttonMode_Hold()) //Exit
									_val0 = 0;
								if(isbuttonMode_Click()&(_batCell < 4400))
								{
									_batCell += 50;
									_Update = 1;
								}
								if(isbuttonNext_Click()&(_batCell > 2500))
								{
									_Update = 1;
									_batCell -= 50;
								}
							}
							TV.printPGM(80,40,PSTR("       "));
							break;
						case 3:				//Calibrate Voltage
							TV.printPGM(80,50,PSTR(">     <"));
							_Update = 1;
							_val0 = 1;
							_val16_1 = BatteryVolt();
							while(_val0)
							{
								Buttons_Tasks();
								
								if(_Update)
								{
									_Update = 0;
									TV.printPGM(86,50,PSTR("     "));
									TV.set_cursor(86,50);
									_val16_0 = _val16_1/1000;
									TV.print(_val16_0);
									TV.print(".");
									_val16_0 = (_val16_1 - _val16_0*1000)/10;
									if(_val16_0<10)	TV.print("0");
									TV.print(_val16_0);
								}
								
								if(isbuttonMode_Hold()) //Exit
									_val0 = 0;
								if(isbuttonMode_Click())
								{
									_val16_1 += 10;
									_Update = 1;
								}
								if(isbuttonNext_Click()&(_val16_1>100))
								{
									_val16_1 -= 10;	
									_Update = 1;
								}
							}
							TV.printPGM(80,50,PSTR("       "));		//Clean
							
							//Save calib factor
							_Update = 1;
							break;
						default:
						case 4:
							OSD_State = OSDs_Main;
							break;
					}
					
					_Update = 1;		//Update screen parameters
					_millisMenu = millis()+MENU_TIMEOUT;
				}
				
			//Save Parameters if exit with mode hold
				if(SelectedMenu_Get()==4)
				{
					if(isbuttonMode_Hold()|isbuttonNext_Hold())
					{
						//Save eeprom parameters
						eep_BattEnable = _batEnable;
						eep_BattType = _batType;
						eep_BattCell = _batCell;
						epp_BattCalib = _batCalib;
					
						Save_EEPROM();
						beep_Confirmation();
					
						TV.set_cursor(40, 80);
						TV.printPGM(PSTR("SAVED !!!"));
						delay(1500);
					
						OSD_State = OSDs_Main;
					}
				}
				
			//Update Screen Values
				if(_Update)
				{
					_Update = 0;
				
				//On/Off
					TV.printPGM(80,20,PSTR("   "));
					if(_batEnable)
						TV.printPGM(80,20,PSTR("On"));
					else
						TV.printPGM(80,20,PSTR("Off"));
					
				//Type					
					TV.printPGM(80,30,PSTR("  "));
					if(_batType == 2)
					TV.printPGM(80,30,PSTR("2s"));
					else
					TV.printPGM(80,30,PSTR("3s"));
				
				//Cell Voltage
					TV.printPGM(80,40,PSTR("  "));
					TV.set_cursor(80,40);
					_val16_0 = _batCell/1000;
					TV.print(_val16_0);
					TV.print(".");
					_val16_0 = _batCell - _val16_0*1000;
					TV.print(_val16_0);
					
				//Batt Calib
					TV.printPGM(80,50,PSTR("  "));
					TV.set_cursor(80,50);
					_val16_1 = BatteryVolt();
					_val16_0 = _val16_1/1000;
					TV.print(_val16_0);
					TV.print(".");
					_val16_0 = _val16_1 - _val16_0*1000;
					TV.print(_val16_0);
				}
								
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
		SelectedMenu_Ini( _selected, _maxItems, 123);
	}

	void	SelectedMenu_Ini ( uint8_t _selected, uint8_t _maxItems, uint8_t _width)
	{
		_selMenu = _selected;
		_selMenuPrev = _selMenu;
		_selMenuMax = _maxItems;
		_selWidth = _width;
		
		//Select Menu
		TV.draw_rect(2,(18 + _selected*10), _selWidth,10, 1, 2);
	}
	
	void	SelectedMenu_Next ()
	{
		if(++_selMenu == _selMenuMax)
			_selMenu = 0;
		
		//DeSelect Previous
		TV.draw_rect(2,(18 + _selMenuPrev*10), _selWidth,10, 0, 2);
		
		//Select Menu
		TV.draw_rect(2,(18 + _selMenu*10), _selWidth,10, 1, 2);
		
		_selMenuPrev = _selMenu;
	}
	
	uint8_t	SelectedMenu_Get ()
	{
		return _selMenu;
	}
