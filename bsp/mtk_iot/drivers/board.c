/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2013 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 */

#include <rthw.h>
#include <rtthread.h>
#include "board.h"
#include "usart.h"

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();

	rt_tick_increase();

	/* leave interrupt */
	rt_interrupt_leave();
}

/**
 * This function will initial mtk-iot board.
 */
void rt_hw_board_init(void)
{
    system_init();

    /* Configure the SysTick */
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
	
    rt_hw_usart_init();
	
#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
    
    //rt_kprintf("%s %s\n", __DATE__, __TIME__);
}

/*@}*/
