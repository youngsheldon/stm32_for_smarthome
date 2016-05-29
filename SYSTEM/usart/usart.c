#include "sys.h"
#include "usart.h"	
#include "led.h"
#include "ui.h"
#include <stdlib.h>
#include <string.h>
//////////////////////////////////////////////////////////////////
#if 1
#pragma import(__use_no_semihosting)             
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       

_sys_exit(int x) 
{ 
	x = x; 
} 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)		) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )

static u8 data_to_send[50];
u8 ShowDataReady=0;
u8 NetData[10];
u8 DecToHex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
u8 addr[4] = {0};

void uart_init(u32 bound)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

	USART_Init(USART1, &USART_InitStructure); 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);                    
}


static u8 TxBuffer[256];
static u8 TxCounter=0;
static u8 count=0;

void USART1_IRQHandler(void)
{
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
    {
        USART_ReceiveData(USART1);
    }
		
	if((USART1->SR & (1<<7))&&(USART1->CR1 & USART_CR1_TXEIE))
	{
		USART1->DR = TxBuffer[TxCounter++];           
		if(TxCounter == count)
		{
			USART1->CR1 &= ~USART_CR1_TXEIE;		
			USART_ITConfig(USART1,USART_IT_TXE,DISABLE);
		}
	}
	if(USART1->SR & (1<<5))
	{
		u8 com_data = USART1->DR;
		Uart1Data_Receive_Prepare(com_data);                              
		LED0 = ! LED0;
	}
}

void Uart1_Put_Char(unsigned char DataToSend)
{
	TxBuffer[count++] = DataToSend;  
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
}

void Uart1_SendPackt(unsigned char *Str,int len)
{
	int i = 0;

	for(;i<len;i++)
		Uart1_Put_Char(*(Str+i));
}

void Uart1_Put_String(unsigned char *Str)
{
	while(*Str)
	{
	if(*Str=='\r')Uart1_Put_Char(0x0d);
		else if(*Str=='\n')Uart1_Put_Char(0x0a);
			else Uart1_Put_Char(*Str);
	Str++;
	}
}

void Uart1SendDataOut(u8 *DataToSend , u8 data_num)
{
	u8 i;
	for(i=0;i<data_num;i++)
		TxBuffer[count++] = *(DataToSend+i);
	if(!(USART1->CR1 & USART_CR1_TXEIE))
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
}

void HexToString(u8 HighBit,u8 LowBit,u8 *out)
{
	memset(out,0,4);
	*out = (int)DecToHex[HighBit/16];
	*(out + 1) = (int)DecToHex[HighBit%16];
	*(out + 2) = (int)DecToHex[LowBit/16];
	*(out + 3) = (int)DecToHex[LowBit%16];
}

// #define DHT11 1
// #define LIGHTED 2
// #define DS18B20 3
// #define MOTOR 4
// #define GAS_TYPE 5
// #define RAIN_TYPE 6
// #define FIRE_TYPE 7
// #define MAN_TYPE 8
// #define BH1750_TYPE 9
// #define DirHumity_TYPE 10
// #define LED_TYPE 11

// typedef struct 
// {
//     //data
//     char ShorAddr[4];
//     int Tempeture;
//     int Humity;
//     int SoilHumidity;
//     int LightIllumination;
//     int GasConcentration;
//     int FireFlag;
//     int HumanFlag;
//     int RainFlag;
//     //command
//     int AddWaterFlag;
//     int AddLightFlag;
//     int OpenWindowFlag;
//     int OpenFanFlag;
// }DEV;

void SensorDataFliter(int tar,DEV*dev,u8 *data,int dt)
{
	switch(tar)
	{
		case 1:
		dev->Tempeture = *(data+6+dt);
		dev->Humity = *(data+7+dt);
		break;

		case 2:
		dev->LightIllumination = 100 - (*(data+6+dt));
		break;

		case 3:
		dev->Tempeture = *(data+6+dt);
		break;

		case 5:
		dev->GasConcentration = *(data+6+dt);
		break;

		case 6:
		dev->RainFlag = *(data+6+dt);
		break;

		case 7:
		dev->FireFlag = *(data+6+dt);
		break;

		case 8:
		dev->HumanFlag = *(data+6+dt);
		break;

		case 9:
		dev->LightIllumination = *(data+6+dt) * 256 + *(data+7+dt);
		break;

		case 10:
		dev->SoilHumidity = *(data+6+dt);
		break;
	}
}

void DevcieAddrFliter(u8 addr1,u8 addr2,DEV*dev)
{
	HexToString(addr1,addr2,addr);
	memset(dev->ShorAddr,0,4);
	strcat(dev->ShorAddr,addr);
}

void OnlineStateUpdate()
{
	static int OnlineStateUpdateCouter = 1;
	int i =0 ;
	OnlineStateUpdateCouter ++;
	if(OnlineStateUpdateCouter > 5)
		OnlineStateUpdateCouter = 1;
	if(OnlineStateUpdateCouter == 5)
	{
		for(i = 0;i<5;i++)
			DeviceOnlineState[i] = 0;
	}
}

void Uart1Data_Receive_Anl(u8 *data_buf,u8 num)
{
	u8 i=0,sum = 0;

	for(i=0;i<(num-1);i++)
		sum += *(data_buf+i);

	//if(!(sum==*(data_buf+num-1)))		return;		
	if(!(*(data_buf)==0xAA && *(data_buf+1)==0xAA))		return;		
	
	Uart1SendDataOut(data_buf , 20);
	for(i=0;i<8;i++)
	 *(NetData+i)=*(data_buf+4+i);

	if(*(data_buf+2)==0X03)
	{
		switch(*(data_buf+4))
		{
			case 1:
			DevcieAddrFliter(*(data_buf+17),*(data_buf+18),&dev1);
			SensorDataFliter(*(data_buf+5),&dev1,data_buf,0);
			SensorDataFliter(*(data_buf+8),&dev1,data_buf,3);
			SensorDataFliter(*(data_buf+11),&dev1,data_buf,6);
			DeviceOnlineState[0] = 1;
			break;

			case 2:
			DevcieAddrFliter(*(data_buf+17),*(data_buf+18),&dev2);
			SensorDataFliter(*(data_buf+5),&dev2,data_buf,0);
			SensorDataFliter(*(data_buf+8),&dev2,data_buf,3);
			SensorDataFliter(*(data_buf+11),&dev2,data_buf,6);
			DeviceOnlineState[1] = 1;
			break;

			case 3:
			DevcieAddrFliter(*(data_buf+17),*(data_buf+18),&dev3);
			SensorDataFliter(*(data_buf+5),&dev3,data_buf,0);
			SensorDataFliter(*(data_buf+8),&dev3,data_buf,3);
			SensorDataFliter(*(data_buf+11),&dev3,data_buf,6);
			DeviceOnlineState[2] = 1;
			break;

			case 4:
			DevcieAddrFliter(*(data_buf+17),*(data_buf+18),&dev4);
			SensorDataFliter(*(data_buf+5),&dev4,data_buf,0);
			SensorDataFliter(*(data_buf+8),&dev4,data_buf,3);
			SensorDataFliter(*(data_buf+11),&dev4,data_buf,6);
			DeviceOnlineState[3] = 1;
			break;

			case 5:
			DevcieAddrFliter(*(data_buf+17),*(data_buf+18),&dev5);
			SensorDataFliter(*(data_buf+5),&dev5,data_buf,0);
			SensorDataFliter(*(data_buf+8),&dev5,data_buf,3);
			SensorDataFliter(*(data_buf+11),&dev5,data_buf,6);
			DeviceOnlineState[4] = 1;
			break;
		}
		
		OnlineStateUpdate();

		ShowDataReady=1;
	}
}

void Uart1Data_Receive_Prepare(u8 data)
{
	static u8 RxBuffer[50];
	 static u8 _data_len = 0,_data_cnt = 0;
	static u8 state = 0;
	
	if(state==0&&data==0xAA)
	{
		state=1;
		RxBuffer[0]=data;
	}
	else if(state==1&&data==0xAA)
	{
		state=2;
		RxBuffer[1]=data;
	}
	else if(state==2&&data<0XF1)
	{
		state=3;
		RxBuffer[2]=data;
	}
	else if(state==3&&data<50)
	{
		state = 4;
		RxBuffer[3]=data;
		_data_len = data;
		_data_cnt = 0;
	}
	else if(state==4&&_data_len>0)
	{
		_data_len--;
		RxBuffer[4+_data_cnt++]=data;
		if(_data_len==0)
			state = 5;
	}
	else if(state==5)
	{
		state = 0;
		RxBuffer[4+_data_cnt]=data;
		Uart1Data_Receive_Anl(RxBuffer,_data_cnt+5);
	}
	else
		state = 0;
}
