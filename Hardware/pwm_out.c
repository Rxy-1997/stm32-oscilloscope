#include "stm32f10x.h"

void pwm_out_init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

    GPIO_InitTypeDef gpio_initstruct;
    gpio_initstruct.GPIO_Mode=GPIO_Mode_AF_PP;
    gpio_initstruct.GPIO_Pin=GPIO_Pin_6;
    gpio_initstruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&gpio_initstruct);

    TIM_InternalClockConfig(TIM3);

    TIM_TimeBaseInitTypeDef tim_timebase_initstruct;
    tim_timebase_initstruct.TIM_ClockDivision=TIM_CKD_DIV1;
    tim_timebase_initstruct.TIM_CounterMode=TIM_CounterMode_Up;
    tim_timebase_initstruct.TIM_Period=100-1;
    tim_timebase_initstruct.TIM_Prescaler=720-1;
    tim_timebase_initstruct.TIM_RepetitionCounter=0;
    TIM_TimeBaseInit(TIM3,&tim_timebase_initstruct);

    TIM_OCInitTypeDef tim_oc_initstruct;
    TIM_OCStructInit(&tim_oc_initstruct);

    tim_oc_initstruct.TIM_OCMode = TIM_OCMode_PWM1;				
	tim_oc_initstruct.TIM_OCPolarity = TIM_OCPolarity_High;
	tim_oc_initstruct.TIM_OutputState = TIM_OutputState_Enable;	
	tim_oc_initstruct.TIM_Pulse = 0;						
    TIM_OC1Init(TIM3,&tim_oc_initstruct);

    TIM_Cmd(TIM3,ENABLE);
}

void pwm_out_set_compar1(uint16_t compare1)
{
    TIM_SetCompare1(TIM3,compare1);
}