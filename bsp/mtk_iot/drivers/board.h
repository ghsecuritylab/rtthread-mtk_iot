/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2013, RT-Thread Development Team
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
//
#include "mt7687.h"
#include "system_mt7687.h"
#include "sys_init.h"

#define MT76XX_SRAM_SIZE         256
#define MT76XX_SRAM_END          (0x20000000 + MT76XX_SRAM_SIZE * 1024)

#define RT_USING_UART1
#define RT_USING_SPI1

void rt_hw_board_init(void);
void rt_hw_usart_init(void);

#endif
