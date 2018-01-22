#include "FreeRTOS.h"
#include "task.h"
#include "misc.h"
#include "stm32f37x_conf.h"

void led_blinker(void * param)
{
  led_blinker_param * p = param;
  TickType_t xDelay = p->period / portTICK_PERIOD_MS;
  while(1)
  {
    gpio_toggle(p->port, p->pin);
    vTaskDelay( xDelay );
  }
}
