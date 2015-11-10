/*
	RTC6715.h
 
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

#ifndef RTC6715_H_
#define RTC6715_H_

	#include <stdint.h>

/*********************************************************************************************************/
/*						CONSTANTS   																	 */
/*********************************************************************************************************/

	//The SPI pins must be defined outside this module: spiDataPin, slaveSelectPin, spiClockPin

	#define		MAX_RF_CHANNELS		40		//Number of frequency channels

	#define		RSSI_AVERAGE		15		//Number of ADC average samples

	// List of frequency channels in order
	const unsigned char channelFreqOrder[] PROGMEM = {
		// 19, 18, 17, 16, 7, 8, 24, 6, 9, 25, 5, 10, 26, 4, 11, 27, 3, 12, 28, 2, 13, 29, 1, 14, 30, 0, 15, 31, 20, 21, 22, 23		// 32 channel index
		19, 32, 18, 17, 33, 16, 7, 34, 8, 24, 6, 9, 25, 5, 35, 10, 26, 4, 11, 27, 3, 36, 12, 28, 2, 13, 29, 37, 1, 14, 30, 0, 15, 31, 38, 20, 21, 39, 22, 23		// 40 channel index
	};

	// Freq values converted for the RTC6715
	const unsigned int channelCode[] PROGMEM = {
		0x2A05, 0x299B, 0x2991, 0x2987, 0x291D, 0x2913, 0x2909, 0x289F,  // Band 1
		0x2903, 0x290C, 0x2916, 0x291F, 0x2989, 0x2992, 0x299C, 0x2A05,  // Band 2
		0x2895, 0x288B, 0x2881, 0x2817, 0x2A0F, 0x2A19, 0x2A83, 0x2A8D,  // Band 3
		0x2906, 0x2910, 0x291A, 0x2984, 0x298E, 0x2998, 0x2A02, 0x2A0C,  // Band 4
		0x281D,	0x28B0, 0x2902, 0x2915, 0x2987, 0x29DA, 0x2A0C, 0x2A13	 // Band 5 -Race band
	};

	// Channel values in MHz
	const unsigned int channelFrequency[] PROGMEM = {
		5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // Band 1
		5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // Band 2
		5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // Band 3
		5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // Band 4
		5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917  // Band 5 - Race Band
	};

	
/*********************************************************************************************************/
/*							FUNCTIONS																	 */
/*		NOTE: "channel" value is the channel code as; MSB band (1-5) - LSB channel (1-8) 	 			 */
/*																										 */
/*********************************************************************************************************/
	
	void		RF_Ini				( uint8_t IniChannel );

	void		RF_ChannelSet		( uint8_t	channel);	//channel code: BYTE - MSB band - LSB channel
	uint8_t		RF_ChannelGet		( void );				//channel code: BYTE - MSB band - LSB channel , 0xFF unkown
	uint8_t		RF_ChannelGetIndex	( void );				//Index 0-MAXCH
	
	void		RF_ChannelInc		( void );
	void		RF_ChannelDec		( void );
		
	uint16_t	RF_FrequencyGet		( void );	//Current frequency
	void		RF_FrequencyNext	( void );	//change to next in the channelFreqOrder
	
	uint16_t	RF_GetFrequencyFromChannel	( uint8_t	channel );	//Return the freq. from a channel code
	
	uint16_t	RF_RSSIGet			( void );
	uint16_t	RF_RSSIGet_Raw		( void );


#endif