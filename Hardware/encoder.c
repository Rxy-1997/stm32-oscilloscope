#include "stm32f10x.h"

__IO uint8_t keyflag=0;

void encoder_init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

    GPIO_InitTypeDef gpio_initstruct;
    gpio_initstruct.GPIO_Mode=GPIO_Mode_IPU;
    gpio_initstruct.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_6|GPIO_Pin_7;
    gpio_initstruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&gpio_initstruct);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource2);

    EXTI_InitTypeDef exti_initstruct;
    exti_initstruct.EXTI_Line=EXTI_Line2;
    exti_initstruct.EXTI_LineCmd=ENABLE;
    exti_initstruct.EXTI_Mode=EXTI_Mode_Interrupt;
    exti_initstruct.EXTI_Trigger=EXTI_Trigger_Falling;
    EXTI_Init(&exti_initstruct);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef nvic_initstruct;
    nvic_initstruct.NVIC_IRQChannel=EXTI2_IRQn;
    nvic_initstruct.NVIC_IRQChannelCmd=ENABLE;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority=2;
    nvic_initstruct.NVIC_IRQChannelSubPriority=2;
    NVIC_Init(&nvic_initstruct);

    TIM_TimeBaseInitTypeDef tim_timebase_initstruct;
    tim_timebase_initstruct.TIM_ClockDivision=TIM_CKD_DIV1;
    tim_timebase_initstruct.TIM_CounterMode=TIM_CounterMode_Up;
    tim_timebase_initstruct.TIM_Period=65535;
    tim_timebase_initstruct.TIM_Prescaler=0;
    tim_timebase_initstruct.TIM_RepetitionCounter=0;
    TIM_TimeBaseInit(TIM4,&tim_timebase_initstruct);

    TIM_ICInitTypeDef tim_ic_initstruct;
    TIM_ICStructInit(&tim_ic_initstruct);
    tim_ic_initstruct.TIM_Channel=TIM_Channel_1;
    tim_ic_initstruct.TIM_ICFilter=10;
    TIM_ICInit(TIM4,&tim_ic_initstruct);

    tim_ic_initstruct.TIM_Channel=TIM_Channel_2;
    tim_ic_initstruct.TIM_ICFilter=10;
    TIM_ICInit(TIM4,&tim_ic_initstruct);

    TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);

    TIM_Cmd(TIM4,ENABLE);
}

int16_t encoder_getcnt()
{
    int16_t temp;

    temp=TIM_GetCounter(TIM4);
    TIM_SetCounter(TIM4,0);
    return temp;
}

uint8_t encoder_get_keyflag()
{
    if(keyflag==1)
    {
        keyflag=0;
        return 1;
    }
    return 0;
}

void EXTI2_IRQHandler()
{
    if(EXTI_GetITStatus(EXTI_Line2)==SET)
    {
        keyflag=1;
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

