
#include "string.h"
#include "stdlib.h"
#include "stdint.h"

#include "rtthread.h"
#include "f_port.h"
#include "mt7687.h"

#ifdef __CC_ARM
extern int Image$$DATA$$ZI$$Limit;
#define MT76XX_SRAM_BEGIN    (&Image$$DATA$$ZI$$Limit)
#elif __ICCARM__
#pragma section="HEAP"
#define MT76XX_SRAM_BEGIN    (__segment_end("HEAP"))
#else
extern int __bss_end;
#define MT76XX_SRAM_BEGIN    (&__bss_end)
#endif



#define MT76XX_SRAM_SIZE         256
#define MT76XX_SRAM_END          (0x20000000 + MT76XX_SRAM_SIZE * 1024)

/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
    rt_kprintf("\n\r Wrong parameter value detected on\r\n");
    rt_kprintf("       file  %s\r\n", file);
    rt_kprintf("       line  %d\r\n", line);

    while (1) ;
}
void __aeabi_assert(const char *expr, const char *file, int line)
{
    rt_kprintf("[line]:%d %s\n", __LINE__, __FUNCTION__);
	while(1);
}

void platform_assert(const char *expr, const char *file, int line)
{
    rt_kprintf("[line]:%d %s\n", __LINE__, __FUNCTION__);
	while(1);
}



void rt_hw_console_output(const char *str)
{
	printf(str);
}

#include "wifi_api.h"

void rt_platform_init()
{
    /* User initial the parameters for wifi initial process,  system will determin which wifi operation mode
     * will be started , and adopt which settings for the specific mode while wifi initial process is running*/
    wifi_config_t config = {0};
    config.opmode = WIFI_MODE_STA_ONLY;
//    strcpy((char *)config.sta_config.ssid, (const char *)"360WiFI-FB321E");
//    strcpy((char *)config.sta_config.password, (const char *)"20112012pw707");
    strcpy((char *)config.sta_config.ssid, (const char *)"Xiaomi_7937");
   strcpy((char *)config.sta_config.password, (const char *)"chenyisong12345");
    config.sta_config.ssid_length = strlen((const char *)config.sta_config.ssid);
    config.sta_config.password_length = strlen((const char *)config.sta_config.password);


     rt_kprintf("[line]:%d %s\n", __LINE__, __FUNCTION__);
   /* Initialize wifi stack and register wifi init complete event handler,
     * notes:  the wifi initial process will be implemented and finished while system task scheduler is running.*/
    wifi_init(&config, NULL);
 
		/* for patch and fw download */
    ethernetif_init_callback();

    /* Tcpip stack and net interface initialization,  dhcp client, dhcp server process initialization*/
		lwip_network_init(config.opmode);
extern int32_t wifi_station_port_secure_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);
extern int32_t wifi_station_disconnected_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);
            wifi_connection_register_event_handler(WIFI_EVENT_IOT_PORT_SECURE, wifi_station_port_secure_event_handler);
            wifi_connection_register_event_handler(WIFI_EVENT_IOT_DISCONNECTED, wifi_station_disconnected_event_handler);
//    lwip_net_start(config.opmode);
}

void rt_init_thread_entry(void* parameter)
{
		rt_uint32_t t = 0;
    rt_kprintf("[line]:%d %s\n", __LINE__, __FUNCTION__);
    {
//        extern void rt_platform_init(void);
			rt_platform_init();
    }

    /* Filesystem Initialization */
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
	/* initialize the device file system */
	dfs_init();

	/* initialize the elm chan FatFS file system*/
	elm_init();
    
    /* mount sd card fat partition 1 as root directory */
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("File System initialized!\n");
    }
    else
    {
        rt_kprintf("File System initialzation failed!\n");
    }
#endif /* RT_USING_DFS && RT_USING_DFS_ELMFAT */

#ifdef RT_USING_LWIP
	/* initialize lwip stack */
	/* register ethernetif device */
//	eth_system_device_init();

	/* initialize lwip system */
//	lwip_system_init();
	rt_kprintf("TCP/IP initialized!\n");
#endif
		
//		netio_init();

#ifdef RT_USING_FINSH
	/* initialize finsh */
	finsh_system_init();
	finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif


    
//    rt_uint32_t temp = 0;
    
//	xSemaphoreHandle xMutex;
//	xMutex = xSemaphoreCreateMutex();
    while(1)
    {
    #if 1
//        rt_kprintf("%s %d\n", __FUNCTION__, t ++);
        rt_thread_delay(100);
	#else
	//			xSemaphoreTake(xMutex, portMAX_DELAY);
        rt_kprintf("%s %d\n", __FUNCTION__, t ++);
        rt_thread_delay(100);
			if((t % 5 == 0)) {
//					rt_kprintf("name:%s\n",pcTaskGetTaskName(xTaskGetCurrentTaskHandle()));
					rt_kprintf("prio:%d\n", uxTaskPriorityGet(Test_Task_Handle));
					xQueueSend(xQueue, &temp, 10);
					temp ++;
//					xSemaphoreGive(xSemaphore);

//					vTaskDelete(Test_Task_Handle);
			}
	//		xSemaphoreGive(xMutex);
			#endif
    }
}

int rt_application_init(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
                            rt_init_thread_entry, RT_NULL,
                            4096, RT_THREAD_PRIORITY_MAX-3, 20);

    if (tid != RT_NULL) 
        rt_thread_startup(tid);

    return 0;
}
void SysTick_Handler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();

	rt_tick_increase();

	/* leave interrupt */
	rt_interrupt_leave();
}
void rt_hw_board_init(void)
{
    /* Do system initialization, eg: hardware, nvdm and random seed. */
    system_init();
    /* Configure the SysTick */
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
}
/**
 * This function will startup RT-Thread RTOS.
 */
void rtthread_startup(void)
{
    /* init board */
    rt_hw_board_init();

    /* show version */
    rt_show_version();

    /* init tick */
    rt_system_tick_init();

    /* init kernel object */
    rt_system_object_init();

    /* init timer system */
    rt_system_timer_init();

#ifdef RT_USING_HEAP
		rt_system_heap_init((void*)MT76XX_SRAM_BEGIN, (void*)MT76XX_SRAM_END);
#endif

    /* init scheduler system */
    rt_system_scheduler_init();

    /* init application */
    rt_application_init();

    /* init timer thread */
    rt_system_timer_thread_init();

    /* init idle thread */
    rt_thread_idle_init();

    /* start scheduler */
    rt_system_scheduler_start();

    /* never reach here */
    return ;
}
