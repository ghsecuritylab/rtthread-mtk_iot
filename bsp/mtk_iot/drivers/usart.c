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

#include "hal.h"
#include "uart.h"
#include "uart_hw.h"

void dump_uart1_reg(void);


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
    
    static uint32_t rx_timeout = 0;

    if (irq_number == CM4_UART1_IRQ)
    {
        u_port = UART_PORT0;
        base = UART_BASE_ADDR(u_port);
    } else {
        u_port = UART_PORT1;
        base = UART_BASE_ADDR(u_port);
    }
    
    //IIR[5:0] Priority Level Interrupt Source
    //0x01 000001 - No interrupt pending
    //0x06 000110 1 Line Status Interrupt BI, FE, PE or OE set in LSR
    //0x04 000100 2 RX Data Received RX Data received or RX Trigger Level reached.
    //0x0C 001100 2 RX Data Timeout Timeout on character in RX FIFO.
    //0x02 000010 3 TX Holding Register Empty TX Holding Register empty or TX FIFO Trigger Level reached.
    //0x00 000000 4 Modem Status change DDCD, TERI, DDSR or DCTS set in MSR
    //0x10 010000 5 Software Flow Control XOFF Character received
    //0x20 100000 6 Hardware Flow Control CTS or RTS

    IIR = HAL_REG_32(base + UART_IIR);
    switch (IIR & 0x3F)
    {
        case 0x03:
            rt_kprintf("hello\n");
            break;
            
        case 0x04:
            rt_kprintf("LSR: %08X\n", HAL_REG_32(base + UART_IIR));
            rt_kprintf("IIR: %08X, IIR & 0x3F: %08X\n", IIR, IIR & 0x3F);
            rt_hw_serial_isr(&serial1, RT_SERIAL_EVENT_RX_IND);
            break;
        
        case 0x0C:
            //rx data timeout
            rt_hw_serial_isr(&serial1, RT_SERIAL_EVENT_RX_IND);
//            if (rx_timeout % 5000 == 0)
//            {
//                rt_kprintf("rx timeout counter: %d\n", rx_timeout);
//            }
//            rx_timeout++;
            return ;
            
        default:
            break;
    }
    
    rt_kprintf("IIR: %08X, IIR & 0x3F: %08X\n", IIR, IIR & 0x3F);

}

void UART1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(CM4_UART1_IRQ);
    //rt_kprintf("%s\n", __FUNCTION__);

    /* leave interrupt */
    rt_interrupt_leave();
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
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    }
    else if(cfg->baud_rate == BAUD_RATE_57600)
    {
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.baudrate = HAL_UART_BAUDRATE_57600;
    }
    else if(cfg->baud_rate == BAUD_RATE_38400)
    {
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.baudrate = HAL_UART_BAUDRATE_38400;
    }
    else if(cfg->baud_rate == BAUD_RATE_19200)
    {
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.baudrate = HAL_UART_BAUDRATE_19200;
    }
    else if(cfg->baud_rate == BAUD_RATE_9600)
    {
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.baudrate = HAL_UART_BAUDRATE_9600;
    }
    
    //data_bits
    if(cfg->data_bits == DATA_BITS_5)
    {
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.word_length = HAL_UART_WORD_LENGTH_5;
    }
    else if(cfg->data_bits == DATA_BITS_6)
    {
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.word_length = HAL_UART_WORD_LENGTH_6;
    }
    else if(cfg->data_bits == DATA_BITS_7)
    {
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.word_length = HAL_UART_WORD_LENGTH_7;
    }
    else if(cfg->data_bits == DATA_BITS_8)
    {
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    }

    //stop_bits
    if(cfg->stop_bits == STOP_BITS_1)
    {
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    }
    else if(cfg->stop_bits == STOP_BITS_2)
    {
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    }

    //parity
    if(cfg->parity == PARITY_NONE)
    {
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.parity = HAL_UART_PARITY_NONE;
    }
    else if(cfg->parity == PARITY_ODD)
    {
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.parity = HAL_UART_PARITY_ODD;
    }
    else if(cfg->parity == PARITY_EVEN)
    {
        rt_kprintf("%s, %d\n", __FUNCTION__, __LINE__);
        uart_config.parity = HAL_UART_PARITY_EVEN;
    }

    hal_uart_deinit(uart->uart_device);
    hal_uart_init(uart->uart_device, &uart_config);
    
    {
        uint16_t IER = HAL_REG_32(CM4_UART1_BASE + UART_IER);
                
        HAL_REG_32(CM4_UART1_BASE + UART_IER) = IER | 0x01; 

        HAL_REG_32(CM4_UART1_BASE + UART_FCR) = 0x07; 
    }
    

    
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

#define PRINT_UART1_REG(__NAME__) \
    rt_kprintf("%20s %08X: %08X\n", #__NAME__, CM4_UART1_BASE + __NAME__, HAL_REG_32(CM4_UART1_BASE + __NAME__))

void dump_uart1_reg(void)
{    
    //PRINT_UART1_REG(UART_RBR);        // receive data
    //PRINT_UART1_REG(UART_THR);        // 
    PRINT_UART1_REG(UART_IER);
    //PRINT_UART1_REG(UART_IIR);        // cleared by reading
    //PRINT_UART1_REG(UART_FCR);
    PRINT_UART1_REG(UART_LCR);
    PRINT_UART1_REG(UART_MCR);
    //PRINT_UART1_REG(UART_LSR);        // RU
    //PRINT_UART1_REG(UART_MSR);        // RU
    PRINT_UART1_REG(UART_SCR);
    PRINT_UART1_REG(UART_DLL);
    PRINT_UART1_REG(UART_DLH);
    //PRINT_UART1_REG(UART_EFR);
    PRINT_UART1_REG(UART_XON1);
    //PRINT_UART1_REG(UART_XON2);
    //PRINT_UART1_REG(UART_XOFF1);
    PRINT_UART1_REG(UART_XOFF2);
    PRINT_UART1_REG(UART_AUTOBAUD_EN);
    PRINT_UART1_REG(UART_RATE_STEP);
    PRINT_UART1_REG(UART_STEP_COUNT);
    PRINT_UART1_REG(UART_SAMPLE_COUNT);
    PRINT_UART1_REG(UART_AUTOBAUD_REG);
    PRINT_UART1_REG(UART_RATE_FIX_REG);
    PRINT_UART1_REG(UART_GUARD);
    PRINT_UART1_REG(UART_ESCAPE_DATA);
    PRINT_UART1_REG(UART_ESCAPE_EN);
    PRINT_UART1_REG(UART_SLEEP_EN);
    PRINT_UART1_REG(UART_VFIFO_EN_REG);
    PRINT_UART1_REG(UART_RX_TRIGGER_ADDR);
    PRINT_UART1_REG(UART_FRACDIV_L);
    PRINT_UART1_REG(UART_FRACDIV_M);
    PRINT_UART1_REG(UART_TX_ACTIVE_EN);
    
#define UART_RX_OFFSET       (0x68)
#define UART_TX_OFFSET       (0x6C)

    //PRINT_UART1_REG(UART_RX_OFFSET);        // RU
    //PRINT_UART1_REG(UART_TX_OFFSET);        // RU
    
    rt_kprintf("\n\n");
}
