/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-09-22     Bernard      add board.h to this bsp
 */

// <<< Use Configuration Wizard in Context Menu >>>
#ifndef __BOARD_H__
#define __BOARD_H__

#include "gd32f1x0.h"
#include "gd32f1x0_misc.h"
/* board configuration */

// <o> Internal SRAM memory size[Kbytes] <8-64>
//	<i>Default: 64
#define GD32_SRAM_SIZE         8
#define GD32_SRAM_END          (0x20000000 + GD32_SRAM_SIZE * 1024)

// <<< Use Configuration Wizard in Context Menu >>>

/* USART driver select. */
#define RT_USING_UART1
#define RT_USING_UART2
#define RT_USING_UART3
#define USING_BXCAN1

void rt_hw_board_init(void);

#endif /* __BOARD_H__ */
