#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f10x.h"

void encoder_init();
int16_t encoder_getcnt();
uint8_t encoder_get_keyflag();
void EXTI2_IRQHandler();

#endif