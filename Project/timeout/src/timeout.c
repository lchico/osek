/*==================[inclusions]=============================================*/

#include "timeout.h" /* <= own header */
#include "chip.h"
#include "os.h"         /* <= operating system header */

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

static uint32_t counter;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
	Chip_GPIO_Init(LPC_GPIO_PORT);

	Chip_SCU_PinMux(2, 0, SCU_MODE_INACT, SCU_MODE_FUNC4);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 5, 0);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 0);

	Chip_SCU_PinMux(2, 1, SCU_MODE_INACT, SCU_MODE_FUNC4);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 5, 1);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 1);

	Chip_SCU_PinMux(2, 2, SCU_MODE_INACT, SCU_MODE_FUNC4);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 5, 2);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 2);

	Chip_SCU_PinMux(1, 0, SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN, SCU_MODE_FUNC0);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 0, 4);
}

/*==================[external functions definition]==========================*/

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

TASK(LEDTask)
{
	EventMaskType eventos;

	/* seteo alarma timeout */
	SetRelAlarm(AlarmEvTimeout, 2000, 0);

	/* espero ambos eventos */
	WaitEvent(evBoton | evTimeout);

	/* lleg�� un evento, me fijo cu��l es */
	GetEvent(LEDTask, &eventos);

	if (eventos & evBoton) {
		/* limpio evento */
		ClearEvent(evBoton);
		/* cancelo alarma */
		CancelAlarm(AlarmEvTimeout);

		/* apagar rojo */
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 0);
		/* prendo led verde */
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, 5, 1);

	}
	if (eventos & evTimeout) {
		ClearEvent(evTimeout);
		/* apagar verde */
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 1);
		/* prendo rojo */
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, 5, 0);
	}

	/* vuelvo a empezar la tarea */
	ChainTask(LEDTask);
}

TASK(ButtonTask)
{
//	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, 0, 4) == 0) {
//		SetEvent(LEDTask, evBoton);
//	}
	ChainTask(ButtonTask);
}

ALARMCALLBACK(TickHook)
{
	counter++;
}

/*==================[end of file]============================================*/
