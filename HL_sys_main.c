/** @file HL_sys_main.c 
*   @brief Application main file
*   @date 02-Mar-2016
*   @version 04.05.02
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2016 Texas Instruments Incorporated - www.ti.com  
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "HL_sys_common.h"

/* USER CODE BEGIN (1) */
#include "HL_gio.h"
#include "HL_sci.h"
#include "HL_reg_sci.h"
#include "HL_mibspi.h"
#include "HL_reg_mibspi.h"
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

#define DELAY_VALUE 8
#define ADC_LATENCY 3
#define numbers_XPOLADC_read  1000



void delay(int j){
	int i=0;
	for (i=0 ; i<j; i++){
	}
}


int read_ADC(int length, uint32 * pnt_data)
/* This function reads the output of the XPOL ADC.
 * It receives the pointer of the memory area dedicated to the ADC samples,
 * and receives the numbers of the read access.
 * The function fills the array and returns an integer,
 * if it is negative it will represents the numbers of OUT of range events on the ADC conversion.
  */
{

	int i=0;
	int j=0;
	int out_of_range=0;
	uint32 Adc_LOW;
	uint32 Adc_HIGH;

	for( j=0; j<(ADC_LATENCY); j++)							//wait the 3 clock cycles latency of the XPOL ADC
	{
		mibspiPORT4->DCLR = (uint32)1U << PIN_CS5;			//clock falling edge
		delay(DELAY_VALUE);

		mibspiPORT4->DSET = (uint32)1U << PIN_CS5;			//clock rising edge
		delay(DELAY_VALUE);
	}

	Adc_LOW = gioGetPort(gioPORTA)& 0x000000FF;				//read the low byte of the first Sampled Value
	Adc_HIGH = gioGetPort(mibspiPORT4) & 0x0000020F;		//read the high byte of the first Sampled Value
	*pnt_data = Adc_HIGH << 8 | Adc_LOW;					//store the value in the array

	for (i=1 ; i<length; i++){								//loop on the n read access
		mibspiPORT4->DCLR = (uint32)1U << PIN_CS5;			//clock falling edge
		delay(DELAY_VALUE);
		mibspiPORT4->DSET = (uint32)1U << PIN_CS5;			//clock rising edge
															//here there isn't the delay because it is
															//intrinsic with the following instructions
		Adc_LOW = (gioPORTA->DIN) & 0x000000FF;				//read the low byte of the Sampled Value
		Adc_HIGH = (mibspiPORT4->DIN) & 0x0000020F;			//read the high byte of the Sampled Value
		*(pnt_data + i) = Adc_HIGH << 8 | Adc_LOW;			//store the value in the array
	}

	for (i=0; i<length; i++){								//loop on all the array
		if (((*(pnt_data + i))&(0x20000))>>17){				//looking for an out of range error that is codify in the bit 17
			out_of_range--;
		}
	}
return out_of_range;
}





void main(void)
{

	uint32 pixel_values[numbers_XPOLADC_read];

	mibspiInit();
	gioInit();
    sciInit();

	for (;;) {

		read_ADC(numbers_XPOLADC_read,&pixel_values[0]);

		sciSend(sciREG1,5,(uint8*)"ciao_"); //debug
		gioToggleBit(gioPORTB, 7);			//debug
		gioToggleBit(gioPORTB, 0);			//debug
	}



}


