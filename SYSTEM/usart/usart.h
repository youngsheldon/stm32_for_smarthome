#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
void uart_init(u32 bound);
void Uart1Data_Receive_Prepare(u8 data);
void Uart1_Put_String(unsigned char *Str);
void Uart1SendRequst(u8 order);
void Uart1_SendPackt(unsigned char *Str,int len);
void Uart1_Put_Char(unsigned char DataToSend);
extern u8 ShowDataReady;
extern u8 NetData[10];
#endif


