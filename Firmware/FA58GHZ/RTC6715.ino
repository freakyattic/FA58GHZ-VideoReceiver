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

	uint8_t		currentChannel = 0;
	uint16_t	currentFrequency = 0;
	uint8_t		currentFrequencyIndex = 0;		

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
	}

	
	void	RF_ChannelSet		( uint8_t	channel)
	{
		uint8_t	_x;
		
		currentChannel = channel;	//Save the channel selection.
		
		//Get the index
		uint8_t	_high = ((channel & 0xF0)>>4)-1;
		uint8_t	_low = (channel & 0x0F)-1;
		uint8_t _index = _high*8 + _low;
		
		if(_index>= MAX_RF_CHANNELS)
		{
			_index = 0;
			currentChannel =0x11;	
		}
	
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
			for(uint8_t x = 16; x > 0; x--)
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
		return currentChannel;
	}
	
	uint8_t		RF_ChannelGetIndex	( void )				//Index 0-MAXCH
	{
		//Get the index
		uint8_t	_high = ((currentChannel & 0xF0)>>4)-1;
		uint8_t	_low = (currentChannel & 0x0F)-1;
		return (_high*8 + _low);
	}

	void		RF_ChannelInc		( void )
	{
		uint8_t _index = RF_ChannelGetIndex();
		
		if( ++_index == MAX_RF_CHANNELS)
			_index = 0;

		//Convert the index to channel code
		uint8_t _high = (_index/8);
		uint8_t _low = _index - (_high*8);
		RF_ChannelSet((((++_high)<<4)&0xF0)|((++_low)&0x0F));
	}

	void		RF_ChannelDec		( void )
	{
		uint8_t _index = RF_ChannelGetIndex();
		if( --_index == 0)
			_index = MAX_RF_CHANNELS-1;
		
		//Convert the index to channel code
		uint8_t _high = (_index/8);
		uint8_t _low = _index - (_high*8);
		RF_ChannelSet((((++_high)<<4)&0xF0)|((++_low)&0x0F));
	}

	uint16_t	RF_FrequencyGet		( void )	//Current frequency
	{
		return currentFrequency;	
	}
	
	void	RF_FrequencyNext	( void )	//Next in the channelFreqOrder
	{
		if(++currentFrequencyIndex >= MAX_RF_CHANNELS)
			currentFrequencyIndex = 0;
		
		uint16_t _channel = pgm_read_byte_near( channelFreqOrder + (uint8_t)currentFrequencyIndex);
		
		//Convert the index to channel code
		uint8_t _high = (_channel/8);
		uint8_t _low = _channel - (_high*8);
		RF_ChannelSet((((++_high)<<4)&0xF0)|((++_low)&0x0F));
	}

	uint16_t	RF_GetFrequencyFromChannel	( uint8_t	channel )
	{
		//Convert from channel code to index
		uint8_t	_high = ((channel & 0xF0)>>4)-1;
		uint8_t	_low = (channel & 0x0F)-1;
		uint8_t _value = _high*8 + _low;
	
		if(_value>= MAX_RF_CHANNELS)
			_value = 0;
		
		uint16_t freq = pgm_read_word_near( channelFrequency + _value);
		return freq;
	}
	
	
	uint16_t		RF_RSSIGet			( void )
	{
		uint16_t _val = RF_RSSIGet_Raw();
		
		if(_val < eep_RSSIMin)
			return 0;
			
		if(_val > eep_RSSIMax)
			return 100;
			
		return (((_val - eep_RSSIMin)*100) / (eep_RSSIMax - eep_RSSIMin));
	}
		

	uint16_t	RF_RSSIGet_Raw		( void ) //Get RSSI data
	{
		unsigned long	_tot = 0;
		
		for( int _x=0; _x < RSSI_AVERAGE; _x++)
		{
			_tot += (unsigned long)analogRead(rssiPin1);
		}
		
		_tot /= RSSI_AVERAGE;
		return (uint16_t)_tot;
	}