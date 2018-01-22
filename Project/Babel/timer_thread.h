#ifndef __TIMER_THREAD
#define __TIMER_THREAD

#include "stm32f37x_conf.h"
#include "FreeRTOS.h"
#include "task.h"

#define TIMER_MODE_INPUT        0
#define TIMER_MODE_OUTPUT       1

typedef struct 
{
  TIM_TypeDef * timer;
  uint8_t channel;
  uint8_t mode;
}
timer_thread_p;


void timer_thread(void * param);


#endif