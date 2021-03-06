/**
  **********************************  STM8S  ***********************************
  * @文件名     ： bsp_adc.c
  * @作者       ： strongerHuang
  * @库版本     ： V2.2.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2017年04月10日
  * @摘要       ： 电压检测头文件
  ******************************************************************************/

/* 定义防止递归包含 ----------------------------------------------------------*/
#ifndef _BSP_ADC_H
#define _BSP_ADC_H

/* 包含的头文件 --------------------------------------------------------------*/
#include "stm8s.h"

/* 宏定义 --------------------------------------------------------------------*/
#define ADC_GPIO_PORT             GPIOD
#define ADC_GPIO_PIN              GPIO_PIN_2

/* 函数申明 ------------------------------------------------------------------*/
void ADC_Initializes(void);
uint32_t ADC_Read(void);


#endif /* _BSP_ADC_H */

/**** Copyright (C)2017 strongerHuang. All Rights Reserved **** END OF FILE ****/
