/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board.h"
#include "gpio.h"
#include "SysTick.h"

#define BALIZA_FREQ_HZ 15U


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

_Bool buttonLastState = false;
_Bool isBalizaOn = false;
//_Bool rebote = false;

void sysTickCallback(void);
void switchCallback(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    gpioMode(PIN_LED_RED, OUTPUT);
    //gpioMode(PIN_LED_BLUE, OUTPUT);
    gpioMode(PIN_SW3 , SW_INPUT_TYPE);
    //gpioMode(PIN_SW2 , SW_INPUT_TYPE);
    gpioMode(PIN_LED_EXTERNAL , EXTERNAL_LED_TYPE);
    //gpioMode(PIN_SWITCH_EXTERNAL , EXTERNAL_SW_TYPE);
    gpioWrite(PIN_LED_RED, HIGH);

    gpioIRQ (PIN_SW3, GPIO_IRQ_MODE_FALLING_EDGE, switchCallback);
    SysTick_Init(&sysTickCallback);

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{

	//espero interrupciones y realizo ISRs.
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



void sysTickCallback(void)
{
	static uint32_t counter = 0;
	if(isBalizaOn)
	{
		if(counter == (SYSTICK_ISR_FREQ_HZ/BALIZA_FREQ_HZ))
		{
			gpioToggle(PIN_LED_EXTERNAL);
			counter = 0;
		}
		else
		{
			counter++;
		}

	}
	else
	{
		counter = 0;
	}
}

void switchCallback(void)
{
	//if(!rebote)
	//{
		isBalizaOn = !isBalizaOn;
		gpioToggle(PIN_LED_RED);
		if (isBalizaOn)
		{
			gpioWrite(PIN_LED_EXTERNAL, HIGH); //que la baliza arranque prendida
		}
		else
		{
			gpioWrite(PIN_LED_EXTERNAL, LOW); //que la baliza se apague inmediatamente
		}
	//}

}

/*******************************************************************************
 ******************************************************************************/
