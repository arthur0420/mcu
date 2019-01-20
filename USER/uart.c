

/******************** (C) COPYRIGHT  风驰电子嵌入式开发工作室 ***************************

 * 文件名  ：uart3.c
 * 描述    ：串口3通信配置函数库     
 * 实验平台：风驰电子STM8开发板
 * 库版本  ：V2.0.0
 * 作者    ：ling_guansheng  QQ：779814207
 * 博客    ：
 *修改时间 ：2011-12-20


****************************************************************************************/

#include "uart.h"
#include "stm8s.h"
#include "stm8s.h"

/* ********************************************
UART3  configured as follow:
  - BaudRate = 115200 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Receive and transmit enabled
 -  Receive interrupt
*********************************************/
void Uart_Init(void)
{
    UART3_DeInit();
    UART3_Init((u32)9600, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, UART3_PARITY_NO , UART3_MODE_TXRX_ENABLE);
    UART3_ITConfig(UART3_IT_RXNE_OR,ENABLE  );
    //UART3_Cmd(ENABLE );
  
}

void UART3_SendByte(u8 data)
{
    UART3_SendData8((unsigned char)data);
  /* Loop until the end of transmission */
  while (UART3_GetFlagStatus(UART3_FLAG_TXE) == RESET);
}

void UART3_SendString(u8* Data,u16 len)
{
  u16 i=0;
  for(;i<len;i++)
    UART3_SendByte(Data[i]);
  
}

u8 UART3_ReceiveByte(void)
{
     u8 USART3_RX_BUF; 
     while (UART3_GetFlagStatus(UART3_FLAG_RXNE) == RESET);
     USART3_RX_BUF=UART3_ReceiveData8();
     return  USART3_RX_BUF;
    
}


/******************* (C) COPYRIGHT 风驰电子嵌入式开发工作室 *****END OF FILE****/