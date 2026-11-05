#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "pwm_out.h"
#include "ad.h"
#include "display.h"
#include "encoder.h"

extern uint16_t ad_data[128];

int main()
{
	int16_t encode_delta;
	uint8_t current_mode=0;
	int16_t x_rate=10;
	uint8_t y_scale=1;

	OLED_Init();
	display_init();
	encoder_init();
	pwm_out_init();
	ad_init();

	pwm_out_set_compar1(50);
	ad_start();
	
	while (1)
	{
		encode_delta=encoder_getcnt();

		if(encoder_get_keyflag()==1)
		{
			current_mode=!current_mode;
		}

		if(encode_delta!=0)
		{
			if(current_mode==0)
			{
				x_rate+=encode_delta;
				if(x_rate<2)x_rate=2;
				if(x_rate>500)x_rate=500;

				ad_set_rate(x_rate);
			}
			else
			{
				y_scale+=(encode_delta>0)?1:-1;
				if(y_scale<1)y_scale=1;
				if(y_scale>5)y_scale=5;
			}
		}

		display_waveform(ad_data,y_scale);

		display_info(ad_data);

		if(current_mode==0)
		{
			OLED_ShowString(2, 1, "Mode: X"); 
            OLED_ShowString(2, 9, "ARR:");
            OLED_ShowNum(2, 13, x_rate, 3);
		}
		else
		{
			OLED_ShowString(2, 1, "Mode: Y"); 
            OLED_ShowString(2, 9, "Zoom:");
            OLED_ShowNum(2, 14, y_scale, 1);
		}

		oled_refresh();
		Delay_ms(20);
	}
}
