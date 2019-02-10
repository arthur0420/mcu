/**
  **********************************  STM8S  ***********************************
  * @文件名     ： bsp_adc.c
  * @作者       ： strongerHuang
  * @库版本     ： V2.2.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2017年04月10日
  * @摘要       ： 电压检测源文件
  ******************************************************************************/
/*----------------------------------------------------------------------------
  更新日志:
  2017-04-10 V1.0.0:初始版本
  ----------------------------------------------------------------------------*/
/* 包含的头文件 --------------------------------------------------------------*/
#include "bsp_adc.h"


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

  ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, ADC1_CHANNEL_3, ADC1_PRESSEL_FCPU_D2, \
            ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL3,\
            DISABLE);

  ADC1_Cmd(ENABLE);                              //使能ADC
}

/************************************************
函数名称 ： ADC_Read
功    能 ： 电池检测
参    数 ： 无
返 回 值 ： 电压(放大1000倍)
作    者 ： strongerHuang
*************************************************/
uint32_t ADC_Read(void)
{
  uint8_t  i;
  uint16_t adc_value = 0;
  uint32_t adc_voltage = 0;

  for(i=0; i<4; i++)
  {
    while(RESET == ADC1_GetFlagStatus(ADC1_FLAG_EOC));
    ADC1_ClearFlag(ADC1_FLAG_EOC);               //等待转换完成，并清除标志

    adc_value += ADC1_GetConversionValue();      //读取转换结果
  }
  
  adc_voltage = adc_value >> 2;                  //求平均
  adc_voltage = (adc_voltage*3300) >> 10;        //1000倍电压值
  
  return adc_voltage;
}


/**** Copyright (C)2017 strongerHuang. All Rights Reserved **** END OF FILE ****/
