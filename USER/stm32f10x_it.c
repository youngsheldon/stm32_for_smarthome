/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "led.h"
#include "key.h"
#include "usart2.h"
#include "usart3.h"
#include "ui.h"
#include "net.h"

int StartToUploadFlag = 0;

//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	static u8 couter = 0;
  static u8 flag = 0;

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
		couter++;
    if(couter>5)
      couter=0;
    if(flag > 4)
      flag = 0;
    switch(couter)
    {
      case 1:
      case 3:
      case 5:
      LED1=!LED1;
      if(SendCmdFlag)
      {
				SendCommand();//发送指令给协调器
      }
      break;

      case 2:  
      if(StartToUploadFlag)
      {
        flag ++;
        if(flag == 5)
        {
          LED3 = ! LED3;
          UpLoadZigbeeEndDeviceSensorDataToCloud();
        }
        if(flag == 2)
        {
          LED3 = ! LED3;
          UpLoadZigbeeEndDeviceOtherSensorDataToCloud();
        }
      }
      break;
    }
  }
}

void USART2_IRQHandler(void)  //串口中断函数
{
	Uart2_IRQ();
}

//void USART3_IRQHandler(void)  //串口中断函数
//{
//	Uart3_IRQ();
//}

void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/


