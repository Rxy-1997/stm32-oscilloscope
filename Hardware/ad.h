#ifndef __AD_H
#define __AD_H

extern uint16_t ad_data[];

void ad_init();
void ad_start();
void ad_stop();
void ad_set_rate(uint16_t arr);

#endif