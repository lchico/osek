/* Copyright 2014, Mariano Cerdeiro
 * Copyright 2014, Pablo Ridolfi
 * Copyright 2014, Juan Cecconi
 * Copyright 2014, Gustavo Muro
 *
 * This file is part of CIAA Firmware.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*==================[inclusions]=============================================*/
#include "../../ejercicio_1_2/inc/ejercicio_1_2.h"         /* <= own header */

#include "chip.h"
#include "os.h"               /* <= operating system header */

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
static void boardButtonsInit(void) {

   /* Config EDU-CIAA-NXP Button Pins as GPIOs */
   Chip_SCU_PinMux(1,0,MD_PUP|MD_EZI|MD_ZI,FUNC0); /* GPIO0[4], TEC1 */
   Chip_SCU_PinMux(1,1,MD_PUP|MD_EZI|MD_ZI,FUNC0); /* GPIO0[8], TEC2 */
   Chip_SCU_PinMux(1,2,MD_PUP|MD_EZI|MD_ZI,FUNC0); /* GPIO0[9], TEC3 */
   Chip_SCU_PinMux(1,6,MD_PUP|MD_EZI|MD_ZI,FUNC0); /* GPIO1[9], TEC4 */

   /* Config EDU-CIAA-NXP Button Pins as Inputs */
   Chip_GPIO_SetDir(LPC_GPIO_PORT, 0,(1<<4)|(1<<8)|(1<<9),0);
   Chip_GPIO_SetDir(LPC_GPIO_PORT, 1,(1<<9),0);

}

static void boardLedsInit(void) {

   /* Config EDU-CIAA-NXP Led Pins as GPIOs */
   Chip_SCU_PinMux(2,0,MD_PUP,FUNC4);  /* GPIO5[0],  LEDR */
   Chip_SCU_PinMux(2,1,MD_PUP,FUNC4);  /* GPIO5[1],  LEDG */
   Chip_SCU_PinMux(2,2,MD_PUP,FUNC4);  /* GPIO5[2],  LEDB */
   Chip_SCU_PinMux(2,10,MD_PUP,FUNC0); /* GPIO0[14], LED1 */
   Chip_SCU_PinMux(2,11,MD_PUP,FUNC0); /* GPIO1[11], LED2 */
   Chip_SCU_PinMux(2,12,MD_PUP,FUNC0); /* GPIO1[12], LED3 */

   /* Config EDU-CIAA-NXP Led Pins as Outputs */
   Chip_GPIO_SetDir(LPC_GPIO_PORT, 5,(1<<0)|(1<<1)|(1<<2),1);
   Chip_GPIO_SetDir(LPC_GPIO_PORT, 0,(1<<14),1);
   Chip_GPIO_SetDir(LPC_GPIO_PORT, 1,(1<<11)|(1<<12),1);

   /* Init EDU-CIAA-NXP Led Pins OFF */
   Chip_GPIO_ClearValue(LPC_GPIO_PORT, 5,(1<<0)|(1<<1)|(1<<2));
   Chip_GPIO_ClearValue(LPC_GPIO_PORT, 0,(1<<14));
   Chip_GPIO_ClearValue(LPC_GPIO_PORT, 1,(1<<11)|(1<<12));

}



static void initHardware(void)
{
	Chip_GPIO_Init(LPC_GPIO_PORT);
	boardLedsInit();
/*	Chip_SCU_PinMux(2,10,SCU_MODE_INACT,SCU_MODE_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,0,14);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,0,14);
*/
}

int main(void)
{
   initHardware();
   StartOS(AppMode1);

   return 0;
}


void ErrorHook(void)
{
   ShutdownOS(0);
}


TASK(BlinkTask_on)
{
	static int i=0,j=1;
	static int state=0;
	//Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,0,14);
	i++;
	if (i==1){
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,0,14);
	}else if (i==10*j){
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,0,14);
	}else if (i>=100){
		j++;
		i=0;
	}
	if (j>9) j=1;
	TerminateTask();
}

TASK(BlinkTask_off)
{
	//Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,0,14);

	TerminateTask();
}

/*==================[end of file]============================================*/

