/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <board.h>
#include <rtthread.h>

#ifdef RT_USING_DFS
#include <dfs_fs.h>
#include <dfs_init.h>
#include <dfs_elm.h>
#endif

#ifdef RT_USING_LWIP
//#include <stm32_eth.h>
#include <netif/ethernetif.h>
extern int lwip_system_init(void);
#endif

#ifdef RT_USING_FINSH
#include <shell.h>
#include <finsh.h>
#endif


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




#include "wifi_api.h"
#include "wifi_lwip_helper.h"

void rt_platform_init(void)
{
    /* User initial the parameters for wifi initial process,  system will determin which wifi operation mode
     * will be started , and adopt which settings for the specific mode while wifi initial process is running*/
    wifi_config_t config = {0};
    config.opmode = WIFI_MODE_STA_ONLY;
    strcpy((char *)config.sta_config.ssid, (const char *)"360WiFI-FB321E");
    strcpy((char *)config.sta_config.password, (const char *)"20112012pw707");
//    strcpy((char *)config.sta_config.ssid, (const char *)"TP-LINK_487880");
//    strcpy((char *)config.sta_config.password, (const char *)"chenyisong12345");
    config.sta_config.ssid_length = strlen((const char *)config.sta_config.ssid);
    config.sta_config.password_length = strlen((const char *)config.sta_config.password);


     rt_kprintf("[line]:%d %s\n", __LINE__, __FUNCTION__);
   /* Initialize wifi stack and register wifi init complete event handler,
     * notes:  the wifi initial process will be implemented and finished while system task scheduler is running.*/
    wifi_init(&config, NULL);

    /* Tcpip stack and net interface initialization,  dhcp client, dhcp server process initialization*/
		lwip_network_init(config.opmode);
    lwip_net_start(config.opmode);
}


void rt_init_thread_entry(void* parameter)
{
		rt_uint32_t t = 0;
    rt_kprintf("[line]:%d %s\n", __LINE__, __FUNCTION__);
    {
        extern void rt_platform_init(void);
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
	eth_system_device_init();

	/* initialize lwip system */
	lwip_system_init();
	rt_kprintf("TCP/IP initialized!\n");
#endif

#ifdef RT_USING_FINSH
	/* initialize finsh */
	finsh_system_init();
	finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

        rt_kprintf("%s %d\n", __FUNCTION__, t ++);
    while(1)
    {
        rt_thread_delay(100);
    }
}
void rt_hw_console_output(const char *str)
{
	printf("%s\n", str);
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

/*@}*/
