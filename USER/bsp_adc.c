/**
  **********************************  STM8S  ***********************************
  * @�ļ���     �� bsp_adc.c
  * @����       �� strongerHuang
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2017��04��10��
  * @ժҪ       �� ��ѹ���Դ�ļ�
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ������־:
  2017-04-10 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "bsp_adc.h"


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

  ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, ADC1_CHANNEL_3, ADC1_PRESSEL_FCPU_D2, \
            ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL3,\
            DISABLE);

  ADC1_Cmd(ENABLE);                              //ʹ��ADC
}

/************************************************
�������� �� ADC_Read
��    �� �� ��ؼ��
��    �� �� ��
�� �� ֵ �� ��ѹ(�Ŵ�1000��)
��    �� �� strongerHuang
*************************************************/
uint32_t ADC_Read(void)
{
  uint8_t  i;
  uint16_t adc_value = 0;
  uint32_t adc_voltage = 0;

  for(i=0; i<4; i++)
  {
    while(RESET == ADC1_GetFlagStatus(ADC1_FLAG_EOC));
    ADC1_ClearFlag(ADC1_FLAG_EOC);               //�ȴ�ת����ɣ��������־

    adc_value += ADC1_GetConversionValue();      //��ȡת�����
  }
  
  adc_voltage = adc_value >> 2;                  //��ƽ��
  adc_voltage = (adc_voltage*3300) >> 10;        //1000����ѹֵ
  
  return adc_voltage;
}


/**** Copyright (C)2017 strongerHuang. All Rights Reserved **** END OF FILE ****/
