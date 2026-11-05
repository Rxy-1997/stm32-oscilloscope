#ifndef __PWM_OUT_H
#define __PWM_OUT_H

#include "stm32f10x.h"

void pwm_out_init();
void pwm_out_set_compar1(uint16_t compare1);

#endif