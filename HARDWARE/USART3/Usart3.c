#include "stm32f10x.h"
#include "usart3.h"

void Uart3_Init(u32 br_num)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART1，GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART1，GPIOA时钟
    //USART3_TX   GPIOB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //USART3_RX	  GPIOB11初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure); 

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART 初始化设置
    USART_InitStructure.USART_BaudRate = br_num;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART3, &USART_InitStructure); //初始化串口1
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART3, ENABLE);                    //使能串口1 
}

static u8 TxBuffer[256];
static u8 TxCounter=0;
static u8 count=0;

void Uart3_IRQ(void)
{
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
    {
        USART_ReceiveData(USART3);
    }
		
	//发送中断
	if((USART3->SR & (1<<7))&&(USART3->CR1 & USART_CR1_TXEIE))
	{
		USART3->DR = TxBuffer[TxCounter++]; //写DR清除中断标志          
		if(TxCounter == count)
		{
			USART3->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE中断
			USART_ITConfig(USART3,USART_IT_TXE,DISABLE);
		}
	}
	//接收中断 (接收寄存器非空) 
	if(USART3->SR & (1<<5))
	{
		u8 com_data = USART3->DR;

	}
}

static void Uart3_Put_Char(unsigned char DataToSend)
{
	TxBuffer[count++] = DataToSend;  
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
}

void Uart3_Put_String(unsigned char *Str)
{
	//判断Str指向的数据是否有效.
	while(*Str)
	{
	//是否是回车字符 如果是,则发送相应的回车 0x0d 0x0a
	if(*Str=='\r')Uart3_Put_Char(0x0d);
		else if(*Str=='\n')Uart3_Put_Char(0x0a);
			else Uart3_Put_Char(*Str);
	//指针++ 指向下一个字节.
	Str++;
	}
}

void Uart3SendDataOut(u8 *DataToSend , u8 data_num)
{
	u8 i;
	for(i=0;i<data_num;i++)
		TxBuffer[count++] = *(DataToSend+i);
	if(!(USART3->CR1 & USART_CR1_TXEIE))
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
}

void Uart3_SendPackt(unsigned char *Str,int len)
{
    int i = 0;

    for(;i<len;i++)
        Uart1_Put_Char(*(Str+i));
}