#ifndef __F_PORT_H__
#define __F_PORT_H__

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

#include "rtthread.h"
#include "rthw.h"


/*These definitions are IAR, Keil and GCC compiler option, the sections in this scope will not be compiled by Assembler. This module can not be disabled.*/
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)

#include "assert.h"

extern void abort(void);
extern void platform_assert(const char *, const char *, int);

#define configASSERT( x ) if( (x) == 0 ) { platform_assert(#x, __FILE__, __LINE__); }

#include "syslog.h"

#endif /*#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)*/



#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE	long


#define pdFALSE			( ( BaseType_t ) 0 )
#define pdTRUE			( ( BaseType_t ) 1 )

#define pdPASS			( pdTRUE )
#define pdFAIL			( pdFALSE )

/* Error definitions. */
#define errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY	( -1 )
#define errQUEUE_BLOCKED						( -4 )
#define errQUEUE_YIELD							( -5 )


typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

	
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / RT_TICK_PER_SECOND)//configTICK_RATE_HZ )


typedef void * QueueHandle_t;
#define xSemaphoreHandle SemaphoreHandle_t

	#define xQueueHandle QueueHandle_t

//
#define pdTASK_CODE TaskFunction_t







/* Definitions returned by xTaskGetSchedulerState().  taskSCHEDULER_SUSPENDED is
0 to generate more optimal code when configASSERT() is defined as the constant
is used in assert() statements. */
#define taskSCHEDULER_SUSPENDED		( ( BaseType_t ) 0 )
#define taskSCHEDULER_NOT_STARTED	( ( BaseType_t ) 1 )
#define taskSCHEDULER_RUNNING		( ( BaseType_t ) 2 )



void *pvPortMalloc(size_t xWantedSize);

void vPortFree(void *pv);

void *pvPortCalloc(size_t nmemb, size_t size);

void *pvPortRealloc(void *pv, size_t size);

size_t xPortGetFreeHeapSize(void);


typedef void (*TaskFunction_t)( void * );

typedef void * TaskHandle_t;

#define xTaskHandle TaskHandle_t


#define	taskDISABLE_INTERRUPTS()			rt_hw_interrupt_disable()
#define	taskENABLE_INTERRUPTS()				rt_hw_interrupt_enable(0)
#define taskENTER_CRITICAL()				rt_enter_critical()
#define taskEXIT_CRITICAL()					rt_exit_critical()
#define	taskYIELD()                         rt_thread_yield();


void vTaskDelay(const TickType_t xTicksToDelay);


void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement );

void vTaskSuspend( TaskHandle_t xTaskToSuspend );

void vTaskResume( TaskHandle_t xTaskToResume );

void vTaskSuspendAll(void);

signed portBASE_TYPE xTaskResumeAll(void);

void vTaskEnterCritical(void);

void vTaskExitCritical(void);

void vTaskDelete(TaskHandle_t xTaskToDelete);

TaskHandle_t xTaskGetCurrentTaskHandle(void);

TickType_t xTaskGetTickCount(void);

void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority );

UBaseType_t uxTaskPriorityGet( TaskHandle_t xTask );

BaseType_t xTaskCreate( TaskFunction_t pxTaskCode, const char * const pcName, const uint16_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask);

char *pcTaskGetTaskName(TaskHandle_t xTaskToQuery);


BaseType_t xTaskGetSchedulerState(void);

void vTaskSwitchContext( void );

BaseType_t xTaskIncrementTick( void );






/* For internal use only. */
#define	queueSEND_TO_BACK		( ( BaseType_t ) 0 )
#define	queueSEND_TO_FRONT		( ( BaseType_t ) 1 )
#define queueOVERWRITE			( ( BaseType_t ) 2 )


#define errQUEUE_EMPTY	( ( BaseType_t ) 0 )
#define errQUEUE_FULL	( ( BaseType_t ) 0 )


/* For internal use only.  These definitions *must* match those in queue.c. */
#define queueQUEUE_TYPE_BASE				( ( uint8_t ) 0U )
#define queueQUEUE_TYPE_SET					( ( uint8_t ) 0U )
#define queueQUEUE_TYPE_MUTEX 				( ( uint8_t ) 1U )
#define queueQUEUE_TYPE_COUNTING_SEMAPHORE	( ( uint8_t ) 2U )
#define queueQUEUE_TYPE_BINARY_SEMAPHORE	( ( uint8_t ) 3U )
#define queueQUEUE_TYPE_RECURSIVE_MUTEX		( ( uint8_t ) 4U )


BaseType_t xQueueGiveMutexRecursive( QueueHandle_t xMutex );

QueueHandle_t xQueueCreateMutex( const uint8_t ucQueueType );

QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType );

void vQueueDelete(QueueHandle_t xQueue);

BaseType_t xQueueGenericReceive(QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait, const BaseType_t xJustPeeking);

BaseType_t xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition );


BaseType_t xQueueGiveFromISR( QueueHandle_t xQueue, BaseType_t * const pxHigherPriorityTaskWoken );

BaseType_t xQueueGenericSendFromISR( QueueHandle_t xQueue, const void * const pvItemToQueue, BaseType_t * const pxHigherPriorityTaskWoken, const BaseType_t xCopyPosition );

BaseType_t xQueueGenericReceive(QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait, const BaseType_t xJustPeeking);

BaseType_t xQueueReceiveFromISR( QueueHandle_t xQueue, void * const pvBuffer, BaseType_t * const pxHigherPriorityTaskWoken );

BaseType_t xQueueTakeMutexRecursive( QueueHandle_t xMutex, TickType_t xTicksToWait );

UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue );


#define xQueueCreate( uxQueueLength, uxItemSize ) xQueueGenericCreate( uxQueueLength, uxItemSize, queueQUEUE_TYPE_BASE )

#define xQueueSendFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) xQueueGenericSendFromISR( ( xQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueSEND_TO_BACK )

#define xQueueSend( xQueue, pvItemToQueue, xTicksToWait ) xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), queueSEND_TO_BACK )

#define xQueueSendToFront( xQueue, pvItemToQueue, xTicksToWait ) xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), queueSEND_TO_FRONT )

#define	xQueueReceive( xQueue, pvBuffer, xTickToWait )	xQueueGenericReceive( ( xQueue ), ( pvBuffer ), ( xTickToWait ), pdFALSE )

#define xQueueSendToBack( xQueue, pvItemToQueue, xTicksToWait ) xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), queueSEND_TO_BACK )

#define xQueueSendToFrontFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) xQueueGenericSendFromISR( ( xQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueSEND_TO_FRONT )

#define xQueueSendToBackFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) xQueueGenericSendFromISR( ( xQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueSEND_TO_BACK )



UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue );





//

typedef QueueHandle_t SemaphoreHandle_t;


#define semBINARY_SEMAPHORE_QUEUE_LENGTH	( ( uint8_t ) 1U )
#define semSEMAPHORE_QUEUE_ITEM_LENGTH		( ( uint8_t ) 0U )
#define semGIVE_BLOCK_TIME					( ( TickType_t ) 0U )


#define vSemaphoreCreateBinary( xSemaphore )																							\
	{																																	\
		( xSemaphore ) = xQueueGenericCreate( ( UBaseType_t ) 1, semSEMAPHORE_QUEUE_ITEM_LENGTH, queueQUEUE_TYPE_BINARY_SEMAPHORE );	\
		if( ( xSemaphore ) != NULL )																									\
		{																																\
			( void ) xSemaphoreGive( ( xSemaphore ) );																					\
		}																																\
	}

#define xSemaphoreCreateBinary() xQueueGenericCreate( ( UBaseType_t ) 1, semSEMAPHORE_QUEUE_ITEM_LENGTH, queueQUEUE_TYPE_BINARY_SEMAPHORE )

#define xSemaphoreTake( xSemaphore, xBlockTime )		xQueueGenericReceive( ( QueueHandle_t ) ( xSemaphore ), NULL, ( xBlockTime ), pdFALSE )

#define xSemaphoreTakeRecursive( xMutex, xBlockTime )	xQueueTakeMutexRecursive( ( xMutex ), ( xBlockTime ) )

#define xSemaphoreGive( xSemaphore )		xQueueGenericSend( ( QueueHandle_t ) ( xSemaphore ), NULL, semGIVE_BLOCK_TIME, queueSEND_TO_BACK )

#define xSemaphoreGiveRecursive( xMutex )	xQueueGiveMutexRecursive( ( xMutex ) )

#define xSemaphoreGiveFromISR( xSemaphore, pxHigherPriorityTaskWoken )	xQueueGiveFromISR( ( QueueHandle_t ) ( xSemaphore ), ( pxHigherPriorityTaskWoken ) )

#define xSemaphoreTakeFromISR( xSemaphore, pxHigherPriorityTaskWoken )	xQueueReceiveFromISR( ( QueueHandle_t ) ( xSemaphore ), NULL, ( pxHigherPriorityTaskWoken ) )

#define xSemaphoreCreateMutex() xQueueCreateMutex( queueQUEUE_TYPE_MUTEX )

#define xSemaphoreCreateRecursiveMutex() xQueueCreateMutex( queueQUEUE_TYPE_RECURSIVE_MUTEX )

#define xSemaphoreCreateCounting( uxMaxCount, uxInitialCount ) xQueueCreateCountingSemaphore( ( uxMaxCount ), ( uxInitialCount ) )

#define vSemaphoreDelete( xSemaphore ) vQueueDelete( ( xQueueHandle ) ( xSemaphore ) )





/* IDs for commands that can be sent/received on the timer queue.  These are to
be used solely through the macros that make up the public software timer API,
as defined below.  The commands that are sent from interrupts must use the
highest numbers as tmrFIRST_FROM_ISR_COMMAND is used to determine if the task
or interrupt version of the queue send function should be used. */
#define tmrCOMMAND_EXECUTE_CALLBACK_FROM_ISR 	( ( BaseType_t ) -2 )
#define tmrCOMMAND_EXECUTE_CALLBACK				( ( BaseType_t ) -1 )
#define tmrCOMMAND_START_DONT_TRACE				( ( BaseType_t ) 0 )
#define tmrCOMMAND_START					    ( ( BaseType_t ) 1 )
#define tmrCOMMAND_RESET						( ( BaseType_t ) 2 )
#define tmrCOMMAND_STOP							( ( BaseType_t ) 3 )
#define tmrCOMMAND_CHANGE_PERIOD				( ( BaseType_t ) 4 )
#define tmrCOMMAND_DELETE						( ( BaseType_t ) 5 )

#define tmrFIRST_FROM_ISR_COMMAND				( ( BaseType_t ) 6 )
#define tmrCOMMAND_START_FROM_ISR				( ( BaseType_t ) 6 )
#define tmrCOMMAND_RESET_FROM_ISR				( ( BaseType_t ) 7 )
#define tmrCOMMAND_STOP_FROM_ISR				( ( BaseType_t ) 8 )
#define tmrCOMMAND_CHANGE_PERIOD_FROM_ISR		( ( BaseType_t ) 9 )


/**
 * Type by which software timers are referenced.  For example, a call to
 * xTimerCreate() returns an TimerHandle_t variable that can then be used to
 * reference the subject timer in calls to other software timer API functions
 * (for example, xTimerStart(), xTimerReset(), etc.).
 */
typedef void * TimerHandle_t;

/*
 * Defines the prototype to which timer callback functions must conform.
 */
typedef void (*TimerCallbackFunction_t)( TimerHandle_t xTimer );

//
TimerHandle_t xTimerCreate( const char * const pcTimerName, const TickType_t xTimerPeriodInTicks, const UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction );

BaseType_t xTimerIsTimerActive( TimerHandle_t xTimer);

BaseType_t xTimerGenericCommand( TimerHandle_t xTimer, const BaseType_t xCommandID, const TickType_t xOptionalValue, BaseType_t *const pxHigherPriorityTaskWoken, const TickType_t xTickToWait);

void *pvTimerGetTimerID( TimerHandle_t xTimer );


const char * pcTimerGetTimerName( TimerHandle_t xTimer );

//
#define xTimerStart( xTimer, xTicksToWait )	xTimerGenericCommand( ( xTimer ), tmrCOMMAND_START, ( xTaskGetTickCount() ), NULL, ( xTicksToWait ) )

#define xTimerStop( xTimer, xTicksToWait )	xTimerGenericCommand( ( xTimer ), tmrCOMMAND_STOP, 0U, NULL, ( xTicksToWait ) )
 
#define xTimerChangePeriod( xTimer, xNewPeriod, xTicksToWait ) xTimerGenericCommand( ( xTimer ), tmrCOMMAND_CHANGE_PERIOD, ( xNewPeriod ), NULL, ( xTicksToWait ) )

#define	xTimerDelete(xTimer, xTicksToWait)	xTimerGenericCommand( ( xTimer ), tmrCOMMAND_DELETE, 0U, NULL, ( xTicksToWait ) )

#define xTimerReset( xTimer, xTicksToWait )	xTimerGenericCommand( ( xTimer ), tmrCOMMAND_RESET, ( xTaskGetTickCount() ), NULL, ( xTicksToWait ) )

#define xTimerStartFromISR( xTimer, pxHigherPriorityTaskWoken ) xTimerGenericCommand( ( xTimer ), tmrCOMMAND_START_FROM_ISR, ( xTaskGetTickCountFromISR() ), ( pxHigherPriorityTaskWoken ), 0U )

#define xTimerStopFromISR( xTimer, pxHigherPriorityTaskWoken ) xTimerGenericCommand( ( xTimer ), tmrCOMMAND_STOP_FROM_ISR, 0, ( pxHigherPriorityTaskWoken ), 0U )

#define xTimerChangePeriodFromISR( xTimer, xNewPeriod, pxHigherPriorityTaskWoken ) 	xTimerGenericCommand( ( xTimer ), tmrCOMMAND_CHANGE_PERIOD_FROM_ISR, ( xNewPeriod ), ( pxHigherPriorityTaskWoken ), 0U )

#define xTimerResetFromISR( xTimer, pxHigherPriorityTaskWoken )	xTimerGenericCommand( ( xTimer ), tmrCOMMAND_RESET_FROM_ISR, ( xTaskGetTickCountFormISR() ), ( pxHigherPriorityTaskWoken ), 0U )

#endif
