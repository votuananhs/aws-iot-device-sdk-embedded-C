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

#include "threads_platform.h"
#ifdef _ENABLE_THREAD_SUPPORT_

#ifdef __cplusplus
extern "C" {
#endif

/* Get cmsis_compiler.h from this link https://github.com/ARM-software/CMSIS_5/tree/develop/CMSIS/Core/Include */
#include "cmsis_compiler.h"

static StaticSemaphore_t xMutexBuffer;

static SemaphoreHandle_t FreeRTOS_CreateMutex(void)
{
	if (0 != __get_IPSR()) /* inside interrupt */
	{
		return xSemaphoreCreateMutexStatic(&xMutexBuffer);
	}
	else
	{
		return xSemaphoreCreateMutex();
	}
}

static BaseType_t FreeRTOS_LockMutex(SemaphoreHandle_t MuxtexHandle)
{
	BaseType_t pxHigherPriorityTaskWoken;

	if (0 != __get_IPSR()) /* inside interrupt */
	{
		return xSemaphoreTakeFromISR(MuxtexHandle, &pxHigherPriorityTaskWoken);
	}
	else
	{
		return xSemaphoreTake(MuxtexHandle, xTicksToWait);
	}
}

static BaseType_t FreeRTOS_UnlockMutex(SemaphoreHandle_t MuxtexHandle)
{
	BaseType_t pxHigherPriorityTaskWoken;

	if (0 != __get_IPSR()) /* inside interrupt */
	{
		return xSemaphoreTakeFromISR(MuxtexHandle, &pxHigherPriorityTaskWoken);
	}
		else
	{
		return xSemaphoreGive(MuxtexHandle);
	}
}

/**
 * @brief Initialize the provided mutex
 *
 * Call this function to initialize the mutex
 *
 * @param IoT_Mutex_t - pointer to the mutex to be initialized
 * @return IoT_Error_t - error code indicating result of operation
 */
IoT_Error_t aws_iot_thread_mutex_init(IoT_Mutex_t *pMutex)
{
	if (NULL == pMutex)
	{
		return MUTEX_INIT_ERROR;
	}
	pMutex->lock = FreeRTOS_CreateMutex();
	if(NULL == pMutex->lock)
	{
		return MUTEX_INIT_ERROR;
	}

	return SUCCESS;
}

/**
 * @brief Lock the provided mutex
 *
 * Call this function to lock the mutex before performing a state change
 * Blocking, thread will block until lock request fails
 *
 * @param IoT_Mutex_t - pointer to the mutex to be locked
 * @return IoT_Error_t - error code indicating result of operation
 */
IoT_Error_t aws_iot_thread_mutex_lock(IoT_Mutex_t *pMutex)
{
	if (NULL == pMutex)
	{
		return MUTEX_LOCK_ERROR;
	}
	if(pdTRUE != FreeRTOS_LockMutex(pMutex->lock))
	{
		return MUTEX_LOCK_ERROR;
	}
	return SUCCESS;
}

/**
 * @brief Try to lock the provided mutex
 *
 * Call this function to attempt to lock the mutex before performing a state change
 * Non-Blocking, immediately returns with failure if lock attempt fails
 *
 * @param IoT_Mutex_t - pointer to the mutex to be locked
 * @return IoT_Error_t - error code indicating result of operation
 */
IoT_Error_t aws_iot_thread_mutex_trylock(IoT_Mutex_t *pMutex)
{
	if (NULL == pMutex)
	{
		return MUTEX_LOCK_ERROR;
	}
	if(pdTRUE != FreeRTOS_LockMutex(pMutex->lock))
	{
		return MUTEX_LOCK_ERROR;
	}
	return SUCCESS;
}

/**
 * @brief Unlock the provided mutex
 *
 * Call this function to unlock the mutex before performing a state change
 *
 * @param IoT_Mutex_t - pointer to the mutex to be unlocked
 * @return IoT_Error_t - error code indicating result of operation
 */
IoT_Error_t aws_iot_thread_mutex_unlock(IoT_Mutex_t *pMutex)
{
	if (NULL == pMutex)
	{
		return MUTEX_UNLOCK_ERROR;
	}
	if(pdTRUE != FreeRTOS_UnlockMutex(pMutex->lock))
	{
		return MUTEX_UNLOCK_ERROR;
	}
	return SUCCESS;
}

/**
 * @brief Destroy the provided mutex
 *
 * Call this function to destroy the mutex
 *
 * @param IoT_Mutex_t - pointer to the mutex to be destroyed
 * @return IoT_Error_t - error code indicating result of operation
 */
IoT_Error_t aws_iot_thread_mutex_destroy(IoT_Mutex_t *pMutex)
{
	if (NULL == pMutex)
	{
		return MUTEX_DESTROY_ERROR;
	}
	vSemaphoreDelete(pMutex->lock);
	return SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif /* _ENABLE_THREAD_SUPPORT_ */

