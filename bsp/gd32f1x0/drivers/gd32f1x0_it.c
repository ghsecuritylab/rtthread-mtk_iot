/**
  ******************************************************************************
  * @file    gd32f1x0_it.c
  * @author  MCU SD
  * @version V1.0.1
  * @date    6-Sep-2014
  * @brief   Main interrupt service routines
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_it.h"

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void  NMI_Handler (void)
{
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void  SVC_Handler (void)
{
}


/**
  * @brief  This function handles USB FS Handler.
  * @param  None
  * @retval None
  */
void  USB_FS_LP_IRQHandler (void)
{
//    USB_Ifr();
}

#ifdef USB_DEVICE_LOW_PWR_MODE_SUPPORT

/**
  * @brief  This function handles USB WakeUp interrupt request.
  * @param  None
  * @retval None
  */
void  USBWakeUp_IRQHandler (void)
{
    EXTI_ClearINTBitState(EXTI_LINE18);
}

#endif


/************************ (C) COPYRIGHT 2014 GIGADEVICE *****END OF FILE****/
