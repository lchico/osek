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
#include "../inc/ejercicio_1_4.h"         /* <= own header */
#include "chip.h"
#include "os.h"               /* <= operating system header */

/*==================[macros and definitions]=================================*/
typedef enum state {UP,DOWN,FALLING_EDGE,RAISE_EDGE} current_state;
/*==================[internal data declaration]==============================*/
static uint32_t counter;
static	uint32 tpress=0;

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
	Chip_SCU_PinMux(1, 0, SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN, SCU_MODE_FUNC0);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, 4);
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

TASK(Task_led_on)
{
	static int i=0;


	switch (i){
	case 0:
			Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 1); // led off
			if ( tpress > 0){
				SetRelAlarm(Pressbutton_on,tpress,0);
				i=1;
			}else{
				SetRelAlarm(Pressbutton_on,100,0);
			}
			break;
	case 1:
			Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, 5, 1); // led on
			if ( tpress < 1000){
				SetRelAlarm(Pressbutton_on,tpress,0);
				i=2;
			}else{
				SetRelAlarm(Pressbutton_on,100,0); // if it is more than 1sec, have a refresh for each second.
			}
			break;
	case 2:
			Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 1); // led on
			if ( tpress < 1000){
				SetRelAlarm(Pressbutton_on,1000-tpress,0);
				i=1;
			}else{
				SetRelAlarm(Pressbutton_on,100,0); // if it is more than 1sec, have a refresh for each second.
				i=1;
			}
			break;
	}
   if (tpress > 4000)  i=0;

	TerminateTask();
}

TASK(Task_led_off)
{
	/*
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 1); // led off
	if (tpress > 0){
		SetRelAlarm(Pressbutton_on,1000-tpress,0);
	}else{
		SetRelAlarm(Pressbutton_off,1000,0);
	}
*/
	TerminateTask();
}

TASK(state_machine)
{
	static	uint32 lastcount;
	static current_state sbutton=UP;
	StatusType aux;
    TickRefType tick;


	switch (sbutton){
			case UP:
			if( Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, 4) == 0){
				sbutton=FALLING_EDGE;
			}
			/* apagar rojo */
			Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 0);

			break;
		case FALLING_EDGE:
			if(Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, 4) == 0){
				sbutton=DOWN;
				lastcount = counter;
			}else{
				sbutton=UP;
			}
			break;
		case DOWN:
			if(Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, 4) == 1 ){
				sbutton=RAISE_EDGE;
			};
			/* prendo led verde */

			break;
		case RAISE_EDGE:
			if(Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, 4) == 0){
				sbutton=DOWN;
			}else{
				tpress=counter-lastcount;
//				SetRelAlarm(Pressbutton_off,tpress,0);
//				Chip_GPIO_SetPinOutHight(LPC_GPIO_PORT, 5, 1); //Enciendo led
//				ActivateTask(Task_led_on);
//				ActivateTask(Task_led_off);
				sbutton=UP;
			}
			break;
	}
	TerminateTask();
}


ALARMCALLBACK(TickHook)
{
	counter++;
}


/*==================[end of file]============================================*/

