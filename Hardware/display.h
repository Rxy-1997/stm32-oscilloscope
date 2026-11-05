#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "stm32f10x.h"

void display_init();
void display_waveform(uint16_t *data,uint8_t scale);
void display_info(uint16_t *data);

#endif