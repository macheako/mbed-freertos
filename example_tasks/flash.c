/*
    FreeRTOS V6.1.0 - Copyright (C) 2010 Real Time Engineers Ltd.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public 
    License and the FreeRTOS license exception along with FreeRTOS; if not it 
    can be viewed here: http://www.freertos.org/a00114.html and also obtained 
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.
*/

/**
 * This version of flash .c is for use on systems that have limited stack space
 * and no display facilities.  The complete version can be found in the 
 * Demo/Common/Full directory.
 * 
 * Three tasks are created, each of which flash an LED at a different rate.  The first 
 * LED flashes every 200ms, the second every 400ms, the third every 600ms.
 *
 * The LED flash tasks provide instant visual feedback.  They show that the scheduler 
 * is still operational.
 *
 */


#include <stdlib.h>

/* Scheduler include files. */
#include <FreeRTOS.h>
#include <task.h>

/* Demo program include files. */
#include "drivers/gpio.h"
#include "example_tasks/flash.h"

#define ledSTACK_SIZE		( configMINIMAL_STACK_SIZE )
#define ledNUMBER_OF_LEDS	( 4 )
#define ledFLASH_RATE_BASE	( ( portTickType ) 500 ) // milliseconds

/* The task that is created three times. */
static portTASK_FUNCTION_PROTO( vLEDFlashTask, pvParameters );

/*-----------------------------------------------------------*/

void vStartLEDFlashTasks( unsigned portBASE_TYPE uxPriority )
{
signed portBASE_TYPE xLEDTask;
signed char xLedName[] = "LEDx";

	/* Create the four tasks. */
	for( xLEDTask = 0; xLEDTask < ledNUMBER_OF_LEDS; ++xLEDTask )
	{
		/* Spawn the task. */
		xLedName[3] = '1' + xLEDTask;
		xTaskCreate( vLEDFlashTask, xLedName, ledSTACK_SIZE, ( void * ) xLEDTask, uxPriority, ( xTaskHandle * ) NULL );
	}
}
/*-----------------------------------------------------------*/

static portTASK_FUNCTION( vLEDFlashTask, pvParameters )
{
portTickType xFlashRate, xLastFlashTime;
unsigned portBASE_TYPE uxLED, uxFlashTaskNumber = ( unsigned portBASE_TYPE ) pvParameters;
	portLONG pin_map[] = {18, 20, 21, 23};	

	/* See which of the eight LED's we should use. */
	uxLED = pin_map[uxFlashTaskNumber++];

	xFlashRate = ledFLASH_RATE_BASE + (ledFLASH_RATE_BASE * (portTickType)uxFlashTaskNumber);
	xFlashRate /= portTICK_RATE_MS;

	/* We will turn the LED on and off again in the delay period, so each
	delay is only half the total period. */
	xFlashRate /= ( portTickType ) 2;

	/* We need to initialise xLastFlashTime prior to the first call to 
	vTaskDelayUntil(). */
	xLastFlashTime = xTaskGetTickCount();

	for(;;)
	{
		/* Delay for half the flash period then toggle the LED. */
		vTaskDelayUntil( &xLastFlashTime, xFlashRate );
		GPIO_PinToggle(1, uxLED);
	}
} /*lint !e715 !e818 !e830 Function definition must be standard for task creation. */

