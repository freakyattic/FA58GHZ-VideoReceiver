/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Pro or Pro Mini w/ ATmega328 (5V, 16 MHz), Platform=avr, Package=arduino
*/

#define __AVR_ATmega328p__
#define __AVR_ATmega328P__
#define ARDUINO 164
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define F_CPU 16000000L
#define ARDUINO 164
#define ARDUINO_AVR_PRO
#define ARDUINO_ARCH_AVR
extern "C" void __cxa_pure_virtual() {;}

void setup ( void );
//
uint8_t		Read_RSSI	( void );
void beep(uint16_t time);
void	_debug	( const __FlashStringHelper *description );
void	Load_EEPROM		( void );
void	Save_EEPROM		( void );
void	VideoSelect		( uint8_t _input);
void	Buttons_Tasks		( void );
void	Clear_ButtonStates	( void );
uint8_t	isbuttonMode_Click ( void );
uint8_t	isbuttonMode_Hold ( void );
uint8_t	isbuttonMode_HoldLong ( void );
uint8_t	isbuttonNext_Click ( void );
uint8_t	isbuttonNext_Hold ( void );
uint8_t	isbuttonNext_HoldLong ( void );
uint8_t	isbuttonFan_Click ( void );
uint8_t	isbuttonFan_Hold ( void );
uint8_t	isbuttonFan_HoldLong ( void );
uint8_t	isbutton_AnyPressed( void );
void	OSD_Ini		( void );
void	OSD_Tasks	( void );
void	OSDScreen_Main	( void );
void	OSDScreen_Off	( void );
void	RF_Ini	( uint8_t IniChannel );
void	RF_ChannelSet		( uint8_t	channelCode);
uint8_t		RF_ChannelGet		( void );
uint8_t		RF_ChannelGetIndex	( void );
void		RF_ChannelInc		( void );
void		RF_ChannelDec		( void );
uint16_t	RF_FrequencyGet		( void );
uint16_t	RF_GetFrequencyFromChannel	( uint8_t	channelCode );
uint16_t	RF_RSSIGet			( void );

#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\variants\eightanaloginputs\pins_arduino.h" 
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\arduino.h"
#include <FA58GHZ.ino>
#include <Definitions.h>
#include <OSDMenus.h>
#include <OSDMenus.ino>
#include <RTC6715.h>
#include <RTC6715.ino>
