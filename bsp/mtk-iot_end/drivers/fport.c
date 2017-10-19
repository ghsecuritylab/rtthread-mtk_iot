#include <rtthread.h>

#include "f_port.h"

//#define     SHOW_TSK_DEBUG_INFO
//#define     SHOW_QUE_DEBUG_INFO
//#define     SHOW_TIM_DEBUG_INFO


extern rt_thread_t rt_current_thread;
static volatile unsigned short tm_index = 0;
static unsigned short mx_index = 0;
static unsigned short mq_index = 0;


/* 
	memory 
*/
void *pvPortMalloc(size_t xWantedSize)
{
	return rt_malloc(xWantedSize);
}
	
void vPortFree(void *pv)
{
	rt_free(pv);
}

void *pvPortCalloc(size_t nmemb, size_t size)
{
	return rt_calloc(nmemb, size);
}

void *pvPortRealloc(void *pv, size_t size)
{
	return rt_realloc(pv, size);
}

size_t xPortGetFreeHeapSize(void)
{
   rt_uint32_t total, used, max_used;
    
    rt_memory_info(&total, &used, &max_used);
    
	return (total - used);
}


/* 
task 
*/
void vTaskDelay(const TickType_t xTicksToDelay)
{
	rt_thread_delay(xTicksToDelay);
}


void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement )
{
   rt_kprintf("%s\n", __FUNCTION__);
}

void vTaskSuspend( TaskHandle_t xTaskToSuspend )
{
    rt_thread_suspend((rt_thread_t)xTaskToSuspend);
}

void vTaskResume( TaskHandle_t xTaskToResume )
{
   rt_thread_resume((rt_thread_t)xTaskToResume);
}

void vTaskSuspendAll(void)
{
   rt_enter_critical();
}

signed portBASE_TYPE xTaskResumeAll(void)
{
   rt_exit_critical();
	return pdTRUE;
}

void vTaskEnterCritical(void)
{
    rt_enter_critical();
}

void vTaskExitCritical(void)
{
    rt_exit_critical();
}


void vTaskDelete(TaskHandle_t xTaskToDelete)
{
	rt_thread_t thread = xTaskToDelete;
	
	if(0 == thread)
	{
	    thread = rt_current_thread;
	}

#ifdef SHOW_TSK_DEBUG_INFO
    rt_kprintf("TaskDelete cur:%s name:%s\n",
                (rt_current_thread) ? (rt_current_thread->name) : ("NULL"),\
                thread->name);
#endif

    rt_thread_delete(thread);
	rt_schedule();
}

TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
	return rt_current_thread;
}

TickType_t xTaskGetTickCount(void)
{
    return rt_tick_get();
}

void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority )
{
    rt_uint8_t priority = uxNewPriority;
    rt_thread_control((rt_thread_t)xTask, RT_THREAD_CTRL_CHANGE_PRIORITY, &priority);
}

UBaseType_t uxTaskPriorityGet( TaskHandle_t xTask )
{
    rt_thread_t thread = (rt_thread_t)xTask;

    return (UBaseType_t)thread->current_priority;
}

BaseType_t xTaskCreate( TaskFunction_t pxTaskCode, const char * const pcName, const uint16_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask)
{
	printf("pcName:%s uxPriority:%d usStackDepth:%d\n", pcName, uxPriority, usStackDepth);
    rt_thread_t thread;
    BaseType_t xReturn = errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
    
    uint16_t usStackDepthC = usStackDepth * sizeof(StackType_t);
    uxPriority = RT_THREAD_PRIORITY_MAX - 5 - uxPriority;
    thread = rt_thread_create(pcName, pxTaskCode, pvParameters, usStackDepthC, uxPriority, 10);

#ifdef SHOW_TSK_DEBUG_INFO    
        rt_kprintf("TaskCreate cur:%s name:%s pri:%d size:%d\n\n",
                    (rt_current_thread) ? (rt_current_thread->name): ("NULL"),
                    pcName, uxPriority, usStackDepthC);
#endif
    if(RT_NULL != thread)
    {
        rt_thread_startup(thread);
        xReturn = pdPASS;
    }
    else
    {
#ifdef SHOW_TSK_DEBUG_INFO    
        rt_kprintf("TaskCreate failed!\n");
#endif
        return xReturn;
    }


	if(pxCreatedTask != RT_NULL)
	{

	    *pxCreatedTask = thread;
	}

	return xReturn;
}


char *pcTaskGetTaskName(TaskHandle_t xTaskToQuery)
{
    rt_thread_t thread = xTaskToQuery;

    return thread->name;
}


BaseType_t xTaskGetSchedulerState(void)
{
    BaseType_t xReturn = taskSCHEDULER_NOT_STARTED;

    if(rt_critical_level() == 0)
    {
        xReturn = taskSCHEDULER_RUNNING;
    }
    else
    {
        xReturn = taskSCHEDULER_SUSPENDED;
    }

    return xReturn;
}

void vTaskSwitchContext( void )
{
	rt_kprintf("%s\n", __FUNCTION__);
}

BaseType_t xTaskIncrementTick( void )
{
	rt_kprintf("%s\n", __FUNCTION__);
}


/* queue */
extern rt_mailbox_t rt_fmq_create(const char *name, rt_size_t item, rt_size_t size, rt_uint8_t flag);
extern rt_err_t rt_fmq_send(rt_mailbox_t mb, void *value, rt_int32_t pos, rt_int32_t timeout);
extern rt_err_t rt_fmq_delete(rt_mailbox_t mb);
extern rt_err_t rt_fmq_recv(rt_mailbox_t mb, void *value, rt_int32_t peek, rt_int32_t timeout);


QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType )
{
    char name[10] = {0};
	rt_object_t obj = 0;

	if(uxItemSize <= 0 || uxQueueLength <= 0)
	{
        if(ucQueueType == queueQUEUE_TYPE_MUTEX)
        {
            sprintf(name, "mux%02d", ((++ mx_index) % 100));
            obj = (rt_object_t)rt_sem_create(name, 1, RT_IPC_FLAG_PRIO);
        }
        else if(ucQueueType == queueQUEUE_TYPE_RECURSIVE_MUTEX)
        {
            sprintf(name, "rmx%02d", ((++ mx_index) % 100));
            obj = (rt_object_t)rt_sem_create(name, 1, RT_IPC_FLAG_PRIO);
        }
        else
        {
            sprintf(name, "sem%02d", ((++ mx_index) % 100));
            obj = (rt_object_t)rt_sem_create(name, 0, RT_IPC_FLAG_PRIO);
        }
	}
	else
	{
        sprintf(name, "fmq%02d", ((++ mq_index) % 100));
        obj = (rt_object_t)rt_fmq_create(name, uxItemSize, uxQueueLength, RT_IPC_FLAG_PRIO);
	}

#ifdef SHOW_QUE_DEBUG_INFO
    rt_kprintf("QueueCreate cur:%s name:%s count:%d size:%d type:%d\n\n",
                (rt_current_thread) ? (rt_current_thread->name) : ("NULL"),
                name, uxQueueLength, uxItemSize, ucQueueType);
#endif

	return obj;
}

void vQueueDelete(QueueHandle_t xQueue)
{
    rt_object_t obj = xQueue;
#ifdef SHOW_QUE_DEBUG_INFO
    rt_kprintf("QueueDelete cur:%s name:%s\n\n",
                (rt_current_thread) ? (rt_current_thread->name) : ("NULL"),
                obj->name);
#endif
    if(obj->type == RT_Object_Class_Semaphore)
    {
        rt_sem_delete((rt_sem_t)obj);
    }
    else if(obj->type == RT_Object_Class_Mutex)
    {
        rt_mutex_delete((rt_mutex_t)obj);
    }
    else
    {
        rt_fmq_delete((rt_mailbox_t)obj);
    }
}



QueueHandle_t xQueueCreateCountingSemaphore( const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount )
{
	rt_kprintf("%s\n", __FUNCTION__);
}

BaseType_t xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition )
{
    rt_object_t obj = (rt_object_t)xQueue;

#ifdef SHOW_QUE_DEBUG_INFO
    rt_kprintf("QueueSend cur:%s name:%s wait:%d pos:%d\n\n",
                (rt_current_thread) ? (rt_current_thread->name) :("NULL"),
                obj->name, xTicksToWait, xCopyPosition);
#endif

    rt_err_t err = RT_EOK;
    if(obj->type == RT_Object_Class_Semaphore)
    {
        err = rt_sem_release((rt_sem_t)obj);
    }
    else if(obj->type == RT_Object_Class_Mutex)
    {
        err = rt_mutex_release((rt_mutex_t)obj);
    }
    else
    {
        err = rt_fmq_send((rt_mailbox_t)obj, (void *)pvItemToQueue, xCopyPosition, xTicksToWait);
    }

    return (err == RT_EOK) ? pdPASS : errQUEUE_FULL;
}

BaseType_t xQueueGenericSendFromISR( QueueHandle_t xQueue, const void * const pvItemToQueue, BaseType_t * const pxHigherPriorityTaskWoken, const BaseType_t xCopyPosition )
{
    rt_object_t obj = (rt_object_t)xQueue;
    
    rt_interrupt_enter();

    rt_err_t err = RT_EOK;
    if(obj->type == RT_Object_Class_Semaphore)
    {
        err = rt_sem_release((rt_sem_t)obj);
    }
    else
    {
        err = rt_fmq_send((rt_mailbox_t)obj, (void *)pvItemToQueue, xCopyPosition, 0);
    }
    
    if(pxHigherPriorityTaskWoken)
    {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    
    rt_interrupt_leave();
    return (err == RT_EOK) ? pdTRUE : errQUEUE_FULL;
}

BaseType_t xQueueGenericReceive(QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait, const BaseType_t xJustPeeking)
{
    rt_object_t obj = (rt_object_t)xQueue;
	
#ifdef SHOW_QUE_DEBUG_INFO
    rt_kprintf("QueueReceive cur:%s name:%s wait:%x\n\n",
                (rt_current_thread) ? (rt_current_thread->name) :("NULL"),
                obj->name, xTicksToWait);
#endif

    rt_err_t err = RT_EOK;
    if(obj->type == RT_Object_Class_Semaphore)
    {
        err = rt_sem_take((rt_sem_t)obj, xTicksToWait);
    }
    else if(obj->type == RT_Object_Class_Mutex)
    {
        err = rt_mutex_take((rt_mutex_t)obj, xTicksToWait);
    }
    else
    {
        err = rt_fmq_recv((rt_mailbox_t)obj, (void *)pvBuffer, xJustPeeking, xTicksToWait);
    }

    return (err == RT_EOK) ? pdPASS : errQUEUE_EMPTY;
}


BaseType_t xQueueReceiveFromISR( QueueHandle_t xQueue, void * const pvBuffer, BaseType_t * const pxHigherPriorityTaskWoken )
{
    rt_object_t obj = (rt_object_t)xQueue;

    rt_interrupt_enter();
    
    rt_err_t err = RT_EOK;
    if(obj->type == RT_Object_Class_Semaphore)
    {
        err = rt_sem_take((rt_sem_t)obj, 0);
    }
    else
    {
        err = rt_fmq_recv((rt_mailbox_t)obj, (void *)pvBuffer, pdFALSE, 0);
    }

    if(pxHigherPriorityTaskWoken)
    {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }

    rt_interrupt_leave();
    return (err == RT_EOK) ? pdTRUE : errQUEUE_EMPTY;
}

BaseType_t xQueueGiveFromISR( QueueHandle_t xQueue, BaseType_t * const pxHigherPriorityTaskWoken )
{
    return xQueueGenericSendFromISR(xQueue, 0, pxHigherPriorityTaskWoken, queueSEND_TO_BACK);
}

QueueHandle_t xQueueCreateMutex( const uint8_t ucQueueType )
{
    return xQueueGenericCreate(1, 0, ucQueueType);
}

BaseType_t xQueueTakeMutexRecursive( QueueHandle_t xMutex, TickType_t xTicksToWait )
{
    return xQueueGenericReceive(xMutex, 0, xTicksToWait, pdFALSE);
}

BaseType_t xQueueGiveMutexRecursive( QueueHandle_t xMutex )
{
    return xQueueGenericSend(xMutex, 0,0, queueSEND_TO_BACK);
}

UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue )
{
    rt_object_t obj = xQueue;
    unsigned portBASE_TYPE count = 0;

    if(obj->type == RT_Object_Class_Mutex)
    {
        count = ((rt_sem_t)obj)->value;
    }
    else if(obj->type == RT_Object_Class_Mutex)
    {
        count = ((rt_mutex_t)obj)->value;
    }
    else
    {
        count = ((rt_mailbox_t)obj)->entry;
    }

    return count;
}

UBaseType_t uxQueueMessagesWaitingFromISR( const QueueHandle_t xQueue )
{
    return uxQueueMessagesWaiting(xQueue);
}

UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue )
{
    UBaseType_t uxReturn = 0;
    rt_object_t obj = xQueue;

    if(obj->type == RT_Object_Class_MailBox)
    {
        rt_base_t level;
        struct rt_mailbox *mb = (struct rt_mailbox *)obj;

        level = rt_hw_interrupt_disable();
        uxReturn = mb->size - mb->entry;
        rt_hw_interrupt_enable(level);
    }

    return uxReturn;
}

/* time */
TimerHandle_t xTimerCreate( const char * pcTimerName, const TickType_t xTimerPeriodInTicks, const UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction )
{
   rt_timer_t timer;
    char name[10] = {0};    
    const char *tname = pcTimerName;
    
    if(pcTimerName == RT_NULL)
    {
        sprintf(name, "tim%02d", (++ tm_index) % 100);
        tname = name;
    }

    if(xTimerPeriodInTicks > 0)
    {
        rt_uint8_t flag = (uxAutoReload) ? RT_TIMER_FLAG_PERIODIC : RT_TIMER_FLAG_PERIODIC;
        timer = rt_timer_create(tname, pxCallbackFunction, pvTimerID, xTimerPeriodInTicks, flag);
    }

#ifdef SHOW_TIM_DEBUG_INFO
    rt_kprintf("xTimerCreate cur:%s name:%s tick:%d auto:%d id:%x func:%x\n",
                (rt_current_thread) ? (rt_current_thread->name) : ("NULL"),
                tname, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction);
#endif

    return timer;
}

BaseType_t xTimerGenericCommand( TimerHandle_t xTimer, const BaseType_t xCommandID, const TickType_t xOptionalValue, BaseType_t *const pxHigherPriorityTaskWoken, const TickType_t xTickToWait)
{
	rt_timer_t obj = (rt_timer_t)xTimer;

    if(pxHigherPriorityTaskWoken)
        rt_interrupt_enter();

#ifdef SHOW_TIM_DEBUG_INFO
    rt_kprintf("xTimerCommand cur:%s name:%s cmd:%d val:%d\n",
                (rt_current_thread) ? (rt_current_thread->name) : ("NULL"),
                obj->parent.name, xCommandID, xOptionalValue);
#endif
        
    rt_err_t err = RT_EOK;
		rt_tick_t tick = (rt_tick_t)xOptionalValue;
    switch(xCommandID)
    {
    case tmrCOMMAND_START:
        err = rt_timer_start(obj);
        break;

    case tmrCOMMAND_STOP:
        err = rt_timer_stop(obj);
        break;

    case tmrCOMMAND_CHANGE_PERIOD:
        err = rt_timer_control(obj, RT_TIMER_CTRL_SET_TIME, &tick);
        break;
        
    case tmrCOMMAND_DELETE:
        err = rt_timer_delete(obj);
        break;

    case tmrCOMMAND_RESET:
//        err = ;
        break;
    }
#ifdef SHOW_TIM_DEBUG_INFO
    rt_kprintf("xTimerCommandOver cur:%s name:%s ret:%d\n",
                (rt_current_thread) ? (rt_current_thread->name) : ("NULL"),
                obj->parent.name, err);
#endif

    if(pxHigherPriorityTaskWoken)
    {
        *pxHigherPriorityTaskWoken = pdFALSE;
        
        rt_interrupt_leave();
    }
	return (err == RT_EOK) ? pdPASS : pdFAIL;
}

BaseType_t xTimerIsTimerActive( TimerHandle_t xTimer)
{
    rt_timer_t obj = (rt_timer_t)xTimer;    

    return (obj->parent.flag & RT_TIMER_FLAG_ACTIVATED) ? pdTRUE : pdFALSE;
}

void *pvTimerGetTimerID( TimerHandle_t xTimer )
{
    rt_timer_t obj = (rt_timer_t)xTimer;

    return obj->parameter;
}

const char * pcTimerGetTimerName( TimerHandle_t xTimer )
{
    rt_timer_t obj = (rt_timer_t)xTimer;

    return obj->parent.name;
}



