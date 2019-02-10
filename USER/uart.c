


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
  while((UART3_GetFlagStatus(UART3_FLAG_TXE)==RESET));
    UART3_SendData8((unsigned char)data);
  /* Loop until the end of transmission */
  while (UART3_GetFlagStatus(UART3_FLAG_TC) == RESET);
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