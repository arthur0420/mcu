
/******************** (C) COPYRIGHT  风驰电子嵌入式开发工作室 ********************
 * 文件名  ：main.c
 * 描述    ：串口3通信     
 * 实验平台：风驰电子STM8开发板
 * 库版本  ：V2.0.0
 * 作者    ：ling_guansheng  QQ：779814207
 * 博客    ：
 *修改时间 ：2011-12-20
**********************************************************************************/

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_clk.h"
#include "intrinsics.h"
#include "stm8s_uart3.h"
#include "stm8s_gpio.h"
#include "stm8s_tim4.h"
#include "bsp_timer.h"
#include "uart.h"
void Delay(u32 nCount);
extern u8 RxBuffer[RxBufferSize];
extern u8 UART_RX_NUM;
extern u8 isend;
/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
// re de 高发  低收
#define ADDR 1
int main(void)
{
  
  //GPIO_Init(LED_GPIO_PORT, (GPIO_Pin_TypeDef)LED_GPIO_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7, GPIO_MODE_OUT_OD_HIZ_FAST ); //  485的gpio初始化 高写 低读
  
  
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST); // a
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);// b
  
  
  //GPIO_WriteLow(LED_GPIO_PORT, GPIO_PIN_3);
  //GPIO_WriteHigh(GPIOD, GPIO_PIN_7);
  
  // u8 len ;
  /* Infinite loop */
  
  /*设置内部时钟16M为主时钟*/ 
 
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  /*!<Set High speed internal clock  */
  TIMDelay_Nms(1000);
  Uart_Init();
   __enable_interrupt(); 
 //  UART3_SendString("1Serial Communication ---STM8 Development Board of FengChi Electron ",\
             sizeof("1Serial Communication ---STM8 Development Board of FengChi Electron"));
   
  // UART3_SendByte('\n');
   
   
   
   //  GPIO_WriteHigh ( GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7) 
  
   GPIO_WriteLow (GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7); // 读
    
   
   int runtime = 0;
   u8 runflag = 0;;
   while (1)
   {
     
      if(isend)
      {
        
        if((RxBuffer[0]& 0xff ) == ADDR){ // 第一位地址码。
          runtime = (RxBuffer[2]<<8)|RxBuffer[3];
          runflag = 1;
        }else{
          isend = 0;
          UART_RX_NUM = 0;
          continue;
        }
        
        /*GPIO_WriteHigh ( GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7) ;
        len=UART_RX_NUM&0x3f;
        while(UART3_GetITStatus(UART3_IT_IDLE));
        TIMDelay_Nms(3000);
        UART3_SendString("You sent the messages is:",sizeof("You sent the messages is"));
        UART3_SendString(RxBuffer,len);
        UART3_SendByte('\n');
        GPIO_WriteLow (GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7);
        */
        UART_RX_NUM=0;
        isend = 0;
        
      }
      
      if(runflag){
        GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3);
        runflag = 0;
      }else if(runtime > 0 ){
        runtime --;
        if(runtime == 0 ){
          GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3);
        }
        TIMDelay_Nms(1000);
      }
      
   }
}

void Delay(u32 nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}



#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 风驰电子嵌入式开发工作室 *****END OF FILE****/
