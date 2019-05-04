


/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_adc2.h"
#include "stm8s_clk.h"
#include "stm8s_uart3.h"
#include "stm8s_gpio.h"
#include "stm8s_tim4.h"
#include "intrinsics.h"
#include "bsp_timer.h"
#include "uart.h"

void Delay(u32 nCount);
void OPEN(u8 index);
void CLOSE(u8 index);
void TIMER_Initializes(void);


extern u8 RxBuffer[RxBufferSize];
extern u8 UART_RX_NUM;
extern u8 hastask;
extern  u8 adcit;
/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define ADDR 0x1





/* 函数申明 ------------------------------------------------------------------*/
void ADC_Initializes(void);
uint16_t ADC_Read(u8 index);



struct Device
{
   GPIO_TypeDef *typea;
   GPIO_TypeDef *typeb;
   GPIO_Pin_TypeDef pina;
   GPIO_Pin_TypeDef pinb;
   ADC2_SchmittTrigg_TypeDef ast;
   ADC2_Channel_TypeDef act;
   
};

struct Device devices[10] ={
  {GPIOD,GPIOD,GPIO_PIN_3,GPIO_PIN_4,ADC2_SCHMITTTRIG_CHANNEL7,ADC2_CHANNEL_7},
  {GPIOD,GPIOD,GPIO_PIN_0,GPIO_PIN_2,ADC2_SCHMITTTRIG_CHANNEL5,ADC2_CHANNEL_5},
  {GPIOA,GPIOA,GPIO_PIN_1,GPIO_PIN_2,ADC2_SCHMITTTRIG_CHANNEL3,ADC2_CHANNEL_3},
  {GPIOA,GPIOA,GPIO_PIN_3,GPIO_PIN_4,ADC2_SCHMITTTRIG_CHANNEL1,ADC2_CHANNEL_1},
  {GPIOA,GPIOA,GPIO_PIN_5,GPIO_PIN_6,ADC2_SCHMITTTRIG_CHANNEL8,ADC2_CHANNEL_8},
  {GPIOC,GPIOE,GPIO_PIN_1,GPIO_PIN_5,ADC2_SCHMITTTRIG_CHANNEL6,ADC2_CHANNEL_6},
  {GPIOC,GPIOC,GPIO_PIN_2,GPIO_PIN_3,ADC2_SCHMITTTRIG_CHANNEL4,ADC2_CHANNEL_4},
  {GPIOC,GPIOC,GPIO_PIN_4,GPIO_PIN_5,ADC2_SCHMITTTRIG_CHANNEL2,ADC2_CHANNEL_2},
  {GPIOC,GPIOC,GPIO_PIN_6,GPIO_PIN_7,ADC2_SCHMITTTRIG_CHANNEL0,ADC2_CHANNEL_0},
  {GPIOG,GPIOG,GPIO_PIN_0,GPIO_PIN_1,ADC2_SCHMITTTRIG_CHANNEL9,ADC2_CHANNEL_9}
};

u16 runtime[10];
u16 theStatus[10];
u16 measureTime[10];
u8 func = 0;
int main(void)
{
  

  /*设置内部时钟16M为主时钟*/ 
  //CLK_DeInit();
  
  
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE );
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  TIMER_Initializes();
  //ADC_Initializes();
  
  // 初始化 485接口
  
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7, GPIO_MODE_OUT_PP_LOW_FAST); //  485的gpio初始化 高写 低读
  
  
  
  for(int i= 0 ; i<10 ;i++){
    GPIO_Init(devices[i].typea, (GPIO_Pin_TypeDef)devices[i].pina, GPIO_MODE_OUT_PP_LOW_SLOW); // a
    GPIO_Init(devices[i].typeb, (GPIO_Pin_TypeDef)devices[i].pinb, GPIO_MODE_OUT_PP_LOW_SLOW);// b
  }
  
  /*!<Set High speed internal clock  */
  //TIMDelay_Nms(1000);
  Uart_Init();
  __enable_interrupt(); 
  
  
  // GPIO_WriteLow (GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7); // 读
  
   
   
   while (1)
   {
      if(hastask)
      { // 有任务
        if((RxBuffer[0]& 0xff ) == ADDR){ // 第一位地址码。 需要自己处理
          func = RxBuffer[1]& 0xff; // 方法
          if(func == 1){ // 方法是 喂食

            for(u8 i = 0 ; i< 10 ;i++){ // 把数据记录下来
               u8 ii= i+1;
               runtime[i] = (RxBuffer[ii*2+1]<<8)|RxBuffer[ii*2+2]; // 时间数组
               if(runtime[i]!=0){
                 OPEN(i);
               }
            }
            
            
            GPIO_WriteHigh (GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7) ;
            while(UART3_GetFlagStatus(UART3_FLAG_IDLE) == RESET);
            TIMDelay_Nms(1000); // 等待一秒获取启动之后的电压数据。
            u8 sendData[23] ;  
            
            sendData[0] = 0x00;
            sendData[1] = 0x01;
            sendData[2] = 0x17;
            
            for(u8 i = 0 ; i< 10 ; i++){
                uint16_t x = ADC_Read(i); 
                u8 a,b;
                a=b=0xFF;
                a&=x;
                b&=x>>8;
                u8 ii = (i+1)*2 +1;
                sendData[ii] = b;
                sendData[ii+1] = a;
            }

            // 地址位 方法位 长度位 数据位
            UART3_SendByte(0xff);
            UART3_SendString(sendData,23);
            UART3_SendByte(0xff);
            TIMDelay_Nms(100);
            GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7); 
          }else if(func == 2){ // 检查状态
            
            GPIO_WriteHigh (GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7) ;
            while(UART3_GetFlagStatus(UART3_FLAG_IDLE) == RESET);
            TIMDelay_Nms(100);
            u8 sendData[23] ;  //{0x00,0x01,0x04,a,b};
            
            sendData[0] = 0x00;
            sendData[1] = 0x02;
            sendData[2] = 0x23;
            
            for(u8 i = 0 ; i< 10 ; i++){
                uint16_t x = theStatus[i];
                u8 a,b;
                a=b=0xFF;
                a&=x;
                b&=x>>8;
                u8 ii = (i+1)*2 +1;
                sendData[ii] = b;
                sendData[ii+1] = a;
            }

            // 地址位 方法位 长度位 数据位
            UART3_SendByte(0xff);
            UART3_SendString(sendData,23);
            UART3_SendByte(0xff);
            TIMDelay_Nms(100);
            GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7); 
            func = 0;
          }
        }
        hastask = 0; //  没有任务了。
        continue;
      }
      
      if(func == 1){
        //GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3);
        for(u8 i=0 ;i<10 ; i++){
          theStatus[i] = 0x0;
          measureTime[i] = 0x0;
        } 
       u8 whilei = 0;
       
       
       while(1){
        
        whilei ++;
          
        u8 stopsum = 0;
        
        for(u8 i = 0 ;i < 10 ;i++){
          u16 ri = runtime[i] - 1;
          if(runtime[i] != 0){ // runtime是0时，啥也不做。
            if(ri == 0){ // 停电机
              stopsum =stopsum+1;
              CLOSE(i);
            }
            runtime[i] = ri;
            if(whilei%10==0){
              uint16_t x = ADC_Read(i);  
              theStatus[i]= theStatus[i]+x;
              measureTime[i]++; 
            }
          }else{
            stopsum =stopsum+1;
          }
        }
        if(stopsum == 10){
          break;
        }
        TIMDelay_Nms(1000);
      }
      
      for(u8 i = 0 ; i< 10 ; i++){
        if(measureTime[i] !=0){
          theStatus[i]= theStatus[i] / measureTime[i];
        }
      }
      func = 0;
      continue;
      }
   }
}

/*
* index 0 开头
*/
void OPEN(u8 index){
  GPIO_WriteHigh(devices[index].typea, (GPIO_Pin_TypeDef)devices[index].pina);
}
void CLOSE(u8 index){
  GPIO_WriteLow(devices[index].typea, (GPIO_Pin_TypeDef)devices[index].pina);
}


/************************************************
函数名称 ： ADC_Initializes
功    能 ： ADC初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void ADC_Initializes(void)
{
  
   GPIO_Init(GPIOB, GPIO_PIN_7, GPIO_MODE_IN_FL_NO_IT);   
   GPIO_Init(GPIOB, GPIO_PIN_6, GPIO_MODE_IN_FL_NO_IT);
   GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);
   GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);
   GPIO_Init(GPIOB, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);
   GPIO_Init(GPIOB, GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT);
   GPIO_Init(GPIOB, GPIO_PIN_1, GPIO_MODE_IN_FL_NO_IT);
   GPIO_Init(GPIOB, GPIO_PIN_0, GPIO_MODE_IN_FL_NO_IT);
   GPIO_Init(GPIOE, GPIO_PIN_7, GPIO_MODE_IN_FL_NO_IT);
   GPIO_Init(GPIOE, GPIO_PIN_6, GPIO_MODE_IN_FL_NO_IT);

  
  
  //ADC2_Init(ADC2_CONVERSIONMODE_SINGLE , ADC2_CHANNEL_7, ADC2_PRESSEL_FCPU_D2, ADC2_EXTTRIG_TIM, DISABLE, ADC2_ALIGN_RIGHT, ADC2_SCHMITTTRIG_CHANNEL7,DISABLE);
  
  ADC2_Cmd(ENABLE);                              //使能ADC
}

/************************************************
函数名称 ： ADC_Read
功    能 ： 电池检测
参    数 ： 无
返 回 值 ： 电压(放大1000倍)
作    者 ： strongerHuang
*************************************************/
uint16_t ADC_Read(u8 index)
{
  uint16_t  i;
  //ADC2_DeInit();
  //ADC2_SchmittTriggerConfig(devices[index].ast, DISABLE);
    ADC2_Init(ADC2_CONVERSIONMODE_SINGLE , devices[index].act, ADC2_PRESSEL_FCPU_D18, ADC2_EXTTRIG_TIM, DISABLE, ADC2_ALIGN_RIGHT, devices[index].ast,DISABLE);
  //ADC2_Init(ADC2_CONVERSIONMODE_SINGLE,ADC2_CHANNEL_11,ADC2_PRESSEL_FCPU_D18,ADC2_EXTTRIG_TIM,DISABLE,ADC2_ALIGN_RIGHT,ADC2_SCHMITTTRIG_CHANNEL11, DISABLE);//单次转换
   ADC2_ITConfig(ENABLE);//中断使能
   ADC2_Cmd(ENABLE);//转换器使能
   ADC2_StartConversion();//开始转换
  //ADC2_ConversionConfig(ADC2_CONVERSIONMODE_SINGLE, devices[index].act, ADC2_ALIGN_RIGHT);
  
  //ADC2_StartConversion();
  
  while(!adcit);
  adcit = 0;
 // ADC2_ClearFlag();
  i= ADC2_GetConversionValue();
  
  
  return i;
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
