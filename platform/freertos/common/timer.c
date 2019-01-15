/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file timer.c
 * @brief Linux implementation of the timer interface.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "timer_platform.h"
/* Get cmsis_compiler.h from this link https://github.com/ARM-software/CMSIS_5/tree/develop/CMSIS/Core/Include */
#include "cmsis_compiler.h"

static TickType_t FreeRTOS_getCurrentTick(void)
{
	if (0 != __get_IPSR()) /* inside interrupt */
	{
		return xTaskGetTickCountFromISR();
	}
	else
	{
		return xTaskGetTickCount();
	}
}

bool has_timer_expired(Timer *timer)
{
	uint32_t currentTick;

	currentTick = FreeRTOS_getCurrentTick();
	return (timer->endTick <= currentTick);
}

void countdown_ms(Timer *timer, uint32_t timeout)
{
	uint32_t currentTick;

	currentTick = FreeRTOS_getCurrentTick();
	timer->endTick = currentTick + (timeout / portTICK_PERIOD_MS);
	if (timer->endTick < currentTick) /* if it is over number of endTick */
	{
		timer->endTick = timeout / portTICK_PERIOD_MS;
	}
}

void countdown_sec(Timer *timer, uint32_t timeout)
{
	uint32_t currentTick;

	currentTick = FreeRTOS_getCurrentTick();
	timer->endTick = currentTick + (timeout * 1000 / portTICK_PERIOD_MS);
	if (timer->endTick < currentTick) /* if it is over number of endTick */
	{
		timer->endTick = timeout * 1000 / portTICK_PERIOD_MS;
	}
}

uint32_t left_ms(Timer *timer)
{
	uint32_t result_ms, currentTick;

	currentTick = FreeRTOS_getCurrentTick();
	if (timer->endTick > currentTick) {
		result_ms = (timer->endTick - currentTick) * portTICK_PERIOD_MS;
	} else {
		result_ms = 0;
	}
	return result_ms;
}

void init_timer(Timer *timer)
{
	timer->endTick = 0;
}

void delay(unsigned milliseconds)
{
	TickType_t sleepTick = milliseconds / portTICK_PERIOD_MS;
	vTaskDelay(sleepTick);
}

#ifdef __cplusplus
}
#endif
