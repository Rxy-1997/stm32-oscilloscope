#include "stm32f10x.h"

uint16_t ad_data[128];

void ad_init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    GPIO_InitTypeDef gpio_initstruct;
    gpio_initstruct.GPIO_Mode=GPIO_Mode_AIN;
    gpio_initstruct.GPIO_Pin=GPIO_Pin_0;
    gpio_initstruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&gpio_initstruct);

    TIM_TimeBaseInitTypeDef tim_timebase_initstruct;
    TIM_TimeBaseStructInit(&tim_timebase_initstruct);
    tim_timebase_initstruct.TIM_ClockDivision=TIM_CKD_DIV1;
    tim_timebase_initstruct.TIM_CounterMode=TIM_CounterMode_Up;
    tim_timebase_initstruct.TIM_Period=10-1;
    tim_timebase_initstruct.TIM_Prescaler=72-1;
    TIM_TimeBaseInit(TIM2,&tim_timebase_initstruct);

    TIM_OCInitTypeDef tim_oc_initstruct;
    TIM_OCStructInit(&tim_oc_initstruct);
    tim_oc_initstruct.TIM_OCMode = TIM_OCMode_PWM1; 
    tim_oc_initstruct.TIM_OutputState = TIM_OutputState_Enable; 
    tim_oc_initstruct.TIM_Pulse = 5;
    TIM_OC2Init(TIM2, &tim_oc_initstruct);

    DMA_InitTypeDef dma_initstruct;
    DMA_StructInit(&dma_initstruct);
    dma_initstruct.DMA_BufferSize=128;
    dma_initstruct.DMA_DIR=DMA_DIR_PeripheralSRC;
    dma_initstruct.DMA_M2M=DMA_M2M_Disable;
    dma_initstruct.DMA_MemoryBaseAddr=(uint32_t)ad_data;
    dma_initstruct.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
    dma_initstruct.DMA_MemoryInc=DMA_MemoryInc_Enable;
    dma_initstruct.DMA_Mode=DMA_Mode_Circular;
    dma_initstruct.DMA_PeripheralBaseAddr=(uint32_t)&ADC1->DR;
    dma_initstruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
    dma_initstruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
    dma_initstruct.DMA_Priority=DMA_Priority_Medium;
    DMA_Init(DMA1_Channel1,&dma_initstruct);

    DMA_Cmd(DMA1_Channel1,ENABLE);

    ADC_InitTypeDef adc_initstruct;
    ADC_StructInit(&adc_initstruct);
    adc_initstruct.ADC_ContinuousConvMode=DISABLE;
    adc_initstruct.ADC_DataAlign=ADC_DataAlign_Right;
    adc_initstruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_T2_CC2;
    adc_initstruct.ADC_Mode=ADC_Mode_Independent;
    adc_initstruct.ADC_NbrOfChannel=1;
    adc_initstruct.ADC_ScanConvMode=DISABLE;
    ADC_Init(ADC1,&adc_initstruct);

    ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_13Cycles5);

    ADC_DMACmd(ADC1,ENABLE);
    ADC_Cmd(ADC1,ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1)==SET);
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1)==SET);
}

void ad_start()
{
    DMA_SetCurrDataCounter(DMA1_Channel1,128);

    ADC_ExternalTrigConvCmd(ADC1,ENABLE);

    TIM_Cmd(TIM2,ENABLE);
}

void ad_stop()
{
    TIM_Cmd(TIM2,DISABLE);

    ADC_ExternalTrigConvCmd(ADC1,DISABLE);
}

void ad_set_rate(uint16_t arr)
{
    if(arr<2)arr=2;

    TIM_SetAutoreload(TIM2,arr-1);
}