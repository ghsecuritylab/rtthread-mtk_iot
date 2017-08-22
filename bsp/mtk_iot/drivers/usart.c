/*
 * File      : usart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2010-03-29     Bernard      remove interrupt Tx and DMA Rx mode
 * 2013-05-13     aozima       update for kehong-lingtai.
 * 2015-01-31     armink       make sure the serial transmit complete in putc()
 */

#include "usart.h"
#include "board.h"
#include <rtdevice.h>


struct mt_uart
{
    hal_uart_port_t uart_device;
    IRQn_Type irq;
    uint32_t priority;
};

#if defined (RT_USING_UART1)
struct mt_uart uart1 = 
{
    HAL_UART_0,
	CM4_UART1_IRQ,
	CM4_UART1_PRI,
};

struct rt_serial_device serial1;

#endif



static void uart_isr(hal_nvic_irq_t irq_number)
{
#define UART_BASE_ADDR(port_no) \
            ((port_no == UART_PORT0) ? CM4_UART1_BASE : CM4_UART2_BASE)
    UART_PORT u_port;
    unsigned int base;
    uint16_t IIR;

    if (irq_number == CM4_UART1_IRQ)
    {
        u_port = UART_PORT0;
        base = UART_BASE_ADDR(u_port);
    } else {
        u_port = UART_PORT1;
        base = UART_BASE_ADDR(u_port);
    }

    IIR = HAL_REG_32(base + UART_IIR);
    switch (IIR & 0x3F)
    {
        case 0x03:
            rt_kprintf("hello\n");
            break;
            
        case 0x04:
            rt_kprintf("world\n");
            break;
            
        default:
            break;
    }

}

static void UART_IRQHandler(hal_nvic_irq_t irq_number)
{
    /* enter interrupt */
    rt_interrupt_enter();

//    uart_isr(irq_number);
        rt_kprintf("%s\n", __FUNCTION__);

    /* leave interrupt */
    rt_interrupt_leave();
}

rt_err_t mt_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct mt_uart* uart;
    hal_uart_config_t uart_config;

    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);

    
    uart = (struct mt_uart *)serial->parent.user_data;
    
    if(cfg->baud_rate == BAUD_RATE_115200)
    {
        uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    }
    else if(cfg->baud_rate == BAUD_RATE_57600)
    {
        uart_config.baudrate = HAL_UART_BAUDRATE_57600;
    }
    else if(cfg->baud_rate == BAUD_RATE_38400)
    {
        uart_config.baudrate = HAL_UART_BAUDRATE_38400;
    }
    else if(cfg->baud_rate == BAUD_RATE_19200)
    {
        uart_config.baudrate = HAL_UART_BAUDRATE_19200;
    }
    else if(cfg->baud_rate == BAUD_RATE_9600)
    {
        uart_config.baudrate = HAL_UART_BAUDRATE_9600;
    }
    
    //data_bits
    if(cfg->data_bits == DATA_BITS_5)
    {
        uart_config.word_length = HAL_UART_WORD_LENGTH_5;
    }
    else if(cfg->data_bits == DATA_BITS_6)
    {
        uart_config.word_length = HAL_UART_WORD_LENGTH_6;
    }
    else if(cfg->data_bits == DATA_BITS_7)
    {
        uart_config.word_length = HAL_UART_WORD_LENGTH_7;
    }
    else if(cfg->data_bits == DATA_BITS_8)
    {
        uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    }

    //stop_bits
    if(cfg->stop_bits == STOP_BITS_1)
    {
        uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    }
    else if(cfg->stop_bits == STOP_BITS_2)
    {
        uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    }

    //parity
    if(cfg->parity == PARITY_NONE)
    {
        uart_config.parity = HAL_UART_PARITY_NONE;
    }
    else if(cfg->parity == PARITY_ODD)
    {
        uart_config.parity = HAL_UART_PARITY_ODD;
    }
    else if(cfg->parity == PARITY_EVEN)
    {
        uart_config.parity = HAL_UART_PARITY_EVEN;
    }

    hal_uart_init(uart->uart_device, &uart_config);
    
    return RT_EOK;
}

rt_err_t mt_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    rt_kprintf("[line]:%d %s\n", __LINE__, __FUNCTION__);

    struct mt_uart* uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct mt_uart *)serial->parent.user_data;

    switch(cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        hal_nvic_disable_irq(uart->irq);
        break;

    case RT_DEVICE_CTRL_SET_INT:
        hal_nvic_enable_irq(uart->irq);
        break;

    case RT_DEVICE_CTRL_CONFIG:
        break;
    }
    
    return RT_EOK;
}

int mt_putc(struct rt_serial_device *serial, char c)
{
    struct mt_uart* uart;

    RT_ASSERT(serial != RT_NULL);

    uart = (struct mt_uart *)serial->parent.user_data;

    hal_uart_put_char(uart->uart_device, c);

    return 1;
}

int mt_getc(struct rt_serial_device *serial)
{
    int ch;
    struct mt_uart* uart;

    RT_ASSERT(serial != RT_NULL);
    
    uart = (struct mt_uart *)serial->parent.user_data;

    ch = -1;
    ch = hal_uart_get_char(uart->uart_device);

    return ch;
}


static const struct rt_uart_ops mt_uart_ops = 
{
    mt_configure,
    mt_control,
    mt_putc,
    mt_getc,
};



static void uart_gpio_config(void)
{
#if defined (RT_USING_UART1)
    /* Set Pinmux to UART */
    hal_pinmux_set_function(HAL_GPIO_0, HAL_GPIO_0_UART1_RTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_1, HAL_GPIO_1_UART1_CTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_2, HAL_GPIO_2_UART1_RX_CM4);
    hal_pinmux_set_function(HAL_GPIO_3, HAL_GPIO_3_UART1_TX_CM4);
#endif

}

static void nvic_uart_config(struct mt_uart* uart)
{
    RT_ASSERT(uart != RT_NULL);
    
    hal_nvic_register_isr_handler(uart->irq , UART_IRQHandler);
    hal_nvic_set_priority(uart->irq, uart->priority);
}


void rt_hw_usart_init(void)
{
    struct mt_uart* uart;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    
    uart_gpio_config();

#if defined (RT_USING_UART1)
    uart = &uart1;
    config.baud_rate = BAUD_RATE_115200;

    nvic_uart_config(uart);

    serial1.ops = &mt_uart_ops;
    serial1.config = config;

    //
    rt_hw_serial_register(&serial1, "uart1",
                            RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                            uart);
    
    
#endif  /* RT_USING_UART1 */

}
