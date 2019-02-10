
/*----------------------------------------------------------------------------
  ������־:
  2017-04-10 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "bsp_timer.h"


/************************************************
�������� �� TIMER_Initializes
��    �� �� ��ʱ����ʼ��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void TIMER_Initializes(void)
{
                                                 //��ʱ10us (2��Ƶ=8M  ����80������)
  TIM4_TimeBaseInit(TIM4_PRESCALER_2, 79);
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
}

/************************************************
�������� �� TIMDelay_N10us
��    �� �� ��ʱ����ʱN��10us
��    �� �� Times --- Nֵ
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void TIMDelay_N10us(uint16_t Times)
{
  
  while(Times--)
  {
    while(RESET == TIM4_GetFlagStatus(TIM4_FLAG_UPDATE));
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  }
}

/************************************************
�������� �� TIMDelay_Nms
��    �� �� ��ʱ����ʱNms
��    �� �� Times --- Nֵ
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void TIMDelay_Nms(uint16_t Times)
{
  TIM4_SetCounter(0);                            //����ֵ����
  TIM4_Cmd(ENABLE);                              //������ʱ��
  while(Times--)
  {
    TIMDelay_N10us(100);
  }
  TIM4_Cmd(DISABLE);                              //�رն�ʱ��
}

/************************************************
�������� �� TIMDelay_Ns
��    �� �� ��ʱ����ʱNs
��    �� �� Times --- Nֵ
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void TIMDelay_Ns(uint16_t Times)
{
  while(Times--)
  {
    TIMDelay_Nms(1000);
  }
}


/**** Copyright (C)2017 strongerHuang. All Rights Reserved **** END OF FILE ****/
