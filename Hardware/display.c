#include "stm32f10x.h"
#include "ad.h"
#include "OLED.h"

#define wave_y_start 16
#define wave_y_end   63
#define wave_y_height (wave_y_end-wave_y_start)

static uint8_t last_y[128];

void display_init()
{
    OLED_Clear();

    for(uint8_t i=0;i<128;i++)
    {
        last_y[i]=255;
    }

    OLED_ShowString(1,1,"STM32 Oscilloscope");

    oled_refresh();
}

void display_waveform(uint16_t *data,uint8_t scale)
{
    uint8_t x,y;

    for(x=0;x<128;x++)
    {
        uint32_t base_h=(uint32_t)data[x]*wave_y_height/4095;
        base_h*=scale;
        if(base_h>wave_y_height)base_h=wave_y_height;

        y=wave_y_end-(uint8_t)base_h;

        if(last_y[x]!=255&&last_y[x]!=y)
        {
            oled_draw_point(x,last_y[x],0);

            uint8_t start_y=(last_y[x]>y)?y:last_y[x];
            uint8_t end_y=(last_y[x]>y)?last_y[x]:y;
            for(uint8_t i=start_y+1;i<end_y;i++)
            {
                oled_draw_point(x,i,1);
            }
        }

        oled_draw_point(x,y,1);
        last_y[x]=y;
    }
    oled_refresh();
}

void display_info(uint16_t *data)
{
    uint16_t max=0;
    uint16_t min=4095;                                                                                                 

    for(uint8_t i=0;i<128;i++)
    {
        if(data[i]>max)max=data[i];
        if(data[i]<min)min=data[i];
    }

    uint16_t vpp=(uint32_t)(max-min)*330/4095;
    OLED_ShowString(1,10,"VPP:");
    OLED_ShowNum(1,14,vpp/100,1);
    OLED_ShowChar(1,15,'.');
    OLED_ShowNum(1,16,vpp%100,2);
}