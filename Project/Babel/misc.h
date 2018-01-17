#ifndef __MISC
#define __MISC

#include "stm32f37x.h"

typedef struct 
{
uint16_t pin;
GPIO_TypeDef * port;
uint16_t period;
}
led_blinker_param;

void led_blinker(void * param);

#endif