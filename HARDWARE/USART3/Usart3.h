#ifndef _USART3_H_
#define _USART3_H_

#include "sys.h"
void Uart3_Init(u32 br_num);
void Uart3_IRQ(void);
void Uart3_Put_String(unsigned char *Str);
void Uart3SendDataOut(u8 *DataToSend , u8 data_num);
void Uart3_SendPackt(unsigned char *Str,int len);

#endif
