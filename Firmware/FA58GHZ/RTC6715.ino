/*
	RTC6715.c
 
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

	#include <stdint.h>
	#include <avr/pgmspace.h>
	#include "RTC6715.h"

/*********************************************************************************************************/
/*							VARIABLES																	 */
/*********************************************************************************************************/

	uint8_t		currentChannelIndex = 0;
	uint16_t	currentFrequency = 0;		

/*********************************************************************************************************/
/*						SPI DEFINITIONS																	 */
/*********************************************************************************************************/

	//The SPI pins must be defined outside this module: spiDataPin, slaveSelectPin, spiClockPin
	
	//The SPI functions are implemented by software.
	#define		RFSPI_LE_High()	delayMicroseconds(1);digitalWrite(slaveSelectPin, HIGH);delayMicroseconds(10)
	#define		RFSPI_LE_Low()	delayMicroseconds(1);digitalWrite(slaveSelectPin, LOW);delayMicroseconds(10)
	
	#define		RFSPI_Write1()	digitalWrite(spiClockPin, LOW);delayMicroseconds(1);digitalWrite(spiDataPin, HIGH);delayMicroseconds(1);digitalWrite(spiClockPin, HIGH);delayMicroseconds(1);digitalWrite(spiClockPin, LOW);delayMicroseconds(1)
	#define		RFSPI_Write0()	digitalWrite(spiClockPin, LOW);delayMicroseconds(1);digitalWrite(spiDataPin, LOW);delayMicroseconds(1);digitalWrite(spiClockPin, HIGH);delayMicroseconds(1);digitalWrite(spiClockPin, LOW);delayMicroseconds(1)

/*********************************************************************************************************/
/*							FUNCTIONS																	 */
/*********************************************************************************************************/

	void	RF_Ini	( uint8_t IniChannel )
	{
		RF_ChannelSet(IniChannel);
		
		RFSPI_LE_High();
		
		_debug(F("RF: Video RX initialized"));
	}

	
	void	RF_ChannelSet		( uint8_t	channelCode)
	{	
		uint8_t _x;
		
		//Get the index
		uint8_t	_high = ((channelCode & 0xF0)>>4)-1;
		uint8_t	_low = (channelCode & 0x0F)-1;
		uint8_t _index = _high*8 + _low;
		
		if(_index >= MAX_RF_CHANNELS)
			_index = 0;
	
		//Save the index
		currentChannelIndex = _index;
		
		Serial.println((int)_index);
		
		//Save current frequency
		currentFrequency = pgm_read_word_near( channelFrequency + _index);
		
		//Frequency Code
		uint16_t freqcode = pgm_read_word_near( channelCode + _index);
				
	//Send Frequency to the SPI
		//Send frequency code to Address 0x01
			RFSPI_LE_Low();		//Activate the RX

			RFSPI_Write1();
			RFSPI_Write0();
			RFSPI_Write0();
			RFSPI_Write0();

			RFSPI_Write1();	//R/W

			// 16b code
			for(_x = 16; _x > 0; _x--)
			{
				if(freqcode & 0x01)
				{
					RFSPI_Write1();
				}
				else
				{
					RFSPI_Write0();
				}
				freqcode >>= 1;
			}
			
			//4b of Zeros
			for(_x = 4; _x > 0; _x--)
			{
				RFSPI_Write0();
			}

			RFSPI_LE_High();		//Save the data into the register.		

		//Disable SPI
		  digitalWrite(slaveSelectPin, HIGH);
		  digitalWrite(spiClockPin, LOW);
		  digitalWrite(spiDataPin, LOW);			
	}

	//channel code: BYTE - MSB band - LSB channel , 0xFF unknown
	uint8_t		RF_ChannelGet		( void )
	{
		uint8_t	_high, _low;
		
		_high = (currentChannelIndex/8);
		_low  = currentChannelIndex - (_high*8);
		
		return ((++_high<<4)&0xF0)|(++_low & 0x0F);
	}
	
	uint8_t		RF_ChannelGetIndex	( void )
	{
		return currentChannelIndex;
	}
	
	void		RF_ChannelInc		( void )
	{
		if( ++currentChannelIndex == MAX_RF_CHANNELS)
			currentChannelIndex = 0;
		RF_ChannelSet(RF_ChannelGet());
	}

	void		RF_ChannelDec		( void )
	{
		if( --currentChannelIndex == 0)
			currentChannelIndex = MAX_RF_CHANNELS-1;
		RF_ChannelSet(RF_ChannelGet());
	}

	uint16_t	RF_FrequencyGet		( void )	//Current frequency
	{
		return currentFrequency;	
	}

	uint16_t	RF_GetFrequencyFromChannel	( uint8_t	channelCode )
	{
		//Convert from channel code to index
		uint8_t	_high = ((channelCode & 0xF0)>>4)-1;
		uint8_t	_low = (channelCode & 0x0F)-1;
		uint8_t _value = _high*8 + _low;
	
		if(_value>= MAX_RF_CHANNELS)
			_value = 0;
		
		uint16_t freq = pgm_read_word_near( channelFrequency + _value);
		return freq;
	}
	
	uint16_t	RF_RSSIGet			( void )	//Get RSSI data
	{
		unsigned long	_tot = 0;
		
		for( int _x=0; _x < RSSI_AVERAGE; _x++)
		{
			_tot += analogRead(rssiPin1);
		}
		
		_tot = _tot / RSSI_AVERAGE;
		return (uint16_t)_tot;
	}

	