


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





/* �������� ------------------------------------------------------------------*/
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
  

  /*�����ڲ�ʱ��16MΪ��ʱ��*/ 
  //CLK_DeInit();
  
  
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE );
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  TIMER_Initializes();
  //ADC_Initializes();
  
  // ��ʼ�� 485�ӿ�
  
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7, GPIO_MODE_OUT_PP_LOW_FAST); //  485��gpio��ʼ�� ��д �Ͷ�
  
  
  
  for(int i= 0 ; i<10 ;i++){
    GPIO_Init(devices[i].typea, (GPIO_Pin_TypeDef)devices[i].pina, GPIO_MODE_OUT_PP_LOW_SLOW); // a
    GPIO_Init(devices[i].typeb, (GPIO_Pin_TypeDef)devices[i].pinb, GPIO_MODE_OUT_PP_LOW_SLOW);// b
  }
  
  /*!<Set High speed internal clock  */
  //TIMDelay_Nms(1000);
  Uart_Init();
  __enable_interrupt(); 
  
  
  // GPIO_WriteLow (GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7); // ��
  
   
   
   while (1)
   {
      if(hastask)
      { // ������
        if((RxBuffer[0]& 0xff ) == ADDR){ // ��һλ��ַ�롣 ��Ҫ�Լ�����
          func = RxBuffer[1]& 0xff; // ����
          if(func == 1){ // ������ ιʳ

            for(u8 i = 0 ; i< 10 ;i++){ // �����ݼ�¼����
               u8 ii= i+1;
               runtime[i] = (RxBuffer[ii*2+1]<<8)|RxBuffer[ii*2+2]; // ʱ������
               if(runtime[i]!=0){
                 OPEN(i);
               }
            }
            
            
            GPIO_WriteHigh (GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7) ;
            while(UART3_GetFlagStatus(UART3_FLAG_IDLE) == RESET);
            TIMDelay_Nms(1000); // �ȴ�һ���ȡ����֮��ĵ�ѹ���ݡ�
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

            // ��ַλ ����λ ����λ ����λ
            UART3_SendByte(0xff);
            UART3_SendString(sendData,23);
            UART3_SendByte(0xff);
            TIMDelay_Nms(100);
            GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7); 
          }else if(func == 2){ // ���״̬
            
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

            // ��ַλ ����λ ����λ ����λ
            UART3_SendByte(0xff);
            UART3_SendString(sendData,23);
            UART3_SendByte(0xff);
            TIMDelay_Nms(100);
            GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_7); 
            func = 0;
          }
        }
        hastask = 0; //  û�������ˡ�
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
          if(runtime[i] != 0){ // runtime��0ʱ��ɶҲ������
            if(ri == 0){ // ͣ���
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
* index 0 ��ͷ
*/
void OPEN(u8 index){
  GPIO_WriteHigh(devices[index].typea, (GPIO_Pin_TypeDef)devices[index].pina);
}
void CLOSE(u8 index){
  GPIO_WriteLow(devices[index].typea, (GPIO_Pin_TypeDef)devices[index].pina);
}


/************************************************
�������� �� ADC_Initializes
��    �� �� ADC��ʼ��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
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
  
  ADC2_Cmd(ENABLE);                              //ʹ��ADC
}

/************************************************
�������� �� ADC_Read
��    �� �� ��ؼ��
��    �� �� ��
�� �� ֵ �� ��ѹ(�Ŵ�1000��)
��    �� �� strongerHuang
*************************************************/
uint16_t ADC_Read(u8 index)
{
  uint16_t  i;
  //ADC2_DeInit();
  //ADC2_SchmittTriggerConfig(devices[index].ast, DISABLE);
    ADC2_Init(ADC2_CONVERSIONMODE_SINGLE , devices[index].act, ADC2_PRESSEL_FCPU_D18, ADC2_EXTTRIG_TIM, DISABLE, ADC2_ALIGN_RIGHT, devices[index].ast,DISABLE);
  //ADC2_Init(ADC2_CONVERSIONMODE_SINGLE,ADC2_CHANNEL_11,ADC2_PRESSEL_FCPU_D18,ADC2_EXTTRIG_TIM,DISABLE,ADC2_ALIGN_RIGHT,ADC2_SCHMITTTRIG_CHANNEL11, DISABLE);//����ת��
   ADC2_ITConfig(ENABLE);//�ж�ʹ��
   ADC2_Cmd(ENABLE);//ת����ʹ��
   ADC2_StartConversion();//��ʼת��
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

/******************* (C) COPYRIGHT ��۵���Ƕ��ʽ���������� *****END OF FILE****/
