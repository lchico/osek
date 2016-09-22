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
#include "queue.h"         /* <= own header */

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
static void boardButtonsInit(void);
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
	boardButtonsInit();

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


/** Create queue **/
void create_queue(spriority *squeue)
{
	int i=0;
    squeue->top=&(squeue->queue[0]);
    squeue->button=&(squeue->queue[0]);
    squeue->state=0;
    for (i=0; i< )
}

/** Add element to a queue **/
void add_queue( spriority *squeue, int element)
{
    *(squeue->top)=element;
    squeue->state++;

    if( squeue->top == &(squeue->queue[MAX_SIZE_QUEUE-1])) {
        squeue->top = &(squeue->queue[0]);
    } else{
        squeue->top++ ;
    }
}

/** Remove element for the queue and return **/
uint32_t remove_queue(spriority *squeue )
{
	uint32_t osk_next_task;
    if( squeue->state == 0) {
        //printf("Queue is empty, I can't remove task");
    } else {
    	osk_next_task= *(squeue->button); // save next task.
        *(squeue->button)=NULL_TASK;
        squeue->state--;
        if(squeue->button == &(squeue->queue[MAX_SIZE_QUEUE-1])) {
            squeue->button=&(squeue->queue[0]);
        } else
            squeue->button++ ;
    }
    return osk_next_task;
}

void control_queue ( spriority *squeue){
	/* mientras no tengo elemento, espero */
	if ( squeue->state > 1 && squeue->state < MAX_SIZE_QUEUE-1 ){
	 	 /* If someone is wating data for de queue */
	 	 if(squeue-> nro_waiting_task > 0){
	 	 	/* I will send the event from task ID */
	 	 	 setevent(squeue->request[squeue->nro_waiting_task],rx_ev_queue);
	 	 	 squeue->nro_waiting_task--;
	 	 }
	}else{
		if(squeue->full_queue > 0){

		}
	}
	ChainTask(control_queue);
}

int push_queue(spriority *queue,TaskType my_task_id){


	if( queue->state > MAX_SIZE_QUEUE -1 ){
		queue->task_push_block[queue->count_push_block]=my_task_id;
		squeue->full_queue++;
		WaitEvent(tx_ev_queue);
		ClearEvent(tx_ev_queue);
	}
	debo cruzar los get

	if ( queue->task_pop_block > 1){
	 	setevent(queue->task_pop_block[queue->count_pop_block],tx_ev_queue);
		 queue->count_pop_block--;
	}




	queue->request[ queue->nro_waiting_task]= my_task_id;
	queue->nro_waiting_task++;
	waitevent(rx_event);
	ClearEvent(rx_event);
	return remove_queue(queue);
}

int pop_queue(spriority *squeue, int element,TaskType my_task_id){

	if ( squeue-> state < MAX_SIZE_QUEUE -1){
		add_queue(squeue,element);
	}else{
		squeue->task_full_id[squeue->full_queue]=my_task_id;
		squeue->full_queue++;
		WaitEvent(tx_ev_queue);
		ClearEvent(tx_ev_queue);
	}
}


TASK(Task_led_1)
{
	EventMaskType events;

	SetRelAlarm(Alarm_timeout, 1000, 0);


	WaitEvent(ev_pressbutton | ev_time_out );

	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 1); // led off
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 1, 11); // led off


	GetEvent(Task_led_1, &events);

	if (events & ev_pressbutton){
		ClearEvent(events);
		CancelAlarm(Alarm_timeout);
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, 5, 1); // led on
	}

	if (events & ev_time_out){
		ClearEvent(events);
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, 1, 11); // led on
	}

	ChainTask(Task_led_1);
}


TASK(state_machine)
{
	static	uint32 lastcount;
	static current_state sbutton=UP;

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
				SetEvent(Task_led_1,ev_pressbutton);
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

