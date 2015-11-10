/*
 * OSDMenus.h
 *
 * Created: 2015-11-09 19:25:24
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


#ifndef OSDMENUS_H_
#define OSDMENUS_H_


/*********************************************************************************************************/
/*						CONSTANTS   																	 */
/*********************************************************************************************************/

	void	OSD_Ini			( void );
	void	OSD_Tasks		( void );
	
	void	OSDScreen_Main	( void );
	void	OSDScreen_Off	( void );

/*********************************************************************************************************/
/*				DEFINITIONS	        																	 */
/*********************************************************************************************************/	

	#define		MENU_TIMEOUT		5000		//Timeout time

	enum OSDSTATES
	{
		OSDs_Off,
		OSDs_OffUpdate,
		
		OSDs_Intro,
		OSDs_IntroUpdate,
		
		OSDs_Main,
		OSDs_MainUpdate,
		
		OSDs_VideoInput,
		OSDs_VideoInputUpdate,
		
		OSDs_RFManual,
		OSDs_RFManualUpdate0,
		OSDs_RFManualUpdate1,
	};




#endif /* OSDMENUS_H_ */