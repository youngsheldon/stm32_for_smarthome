#ifndef _USART2_H_
#define _USART2_H_
#include "sys.h"
void Uart2_Init(u32 br_num);
void Uart2_IRQ(void);
void Uart2_Put_String(unsigned char *Str);
void Uart2_Put_Buf(unsigned char *DataToSend , u8 data_num);
void Uart2SendRequst(u8 order);
void Uart2SendDataOut(u8 *DataToSend , int data_num);
void Uart2_SendPackt(unsigned char *Str,int len);
void Uart2_Put_Char(unsigned char DataToSend);
extern u8 SaveRxBuffer[600];
extern int RxCompeleteFlag;
extern int RXCouter;
#endif

