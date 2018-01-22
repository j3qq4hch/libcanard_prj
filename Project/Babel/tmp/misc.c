#include "FreeRTOS.h"
#include "task.h"
#include "misc.h"
#include "stm32f37x_conf.h"

void led_blinker(void * param)
{
  led_blinker_param * p = param;
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; // GPIO_OType_PP 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = p->pin;
  
  GPIO_Init(p->port, &GPIO_InitStructure);
  TickType_t xDelay = p->period / portTICK_PERIOD_MS;
  while(1)
  {
    GPIO_SetBits(p->port, p->pin);
    vTaskDelay( xDelay );
    GPIO_ResetBits(p->port, p->pin);
    vTaskDelay( xDelay );
  }
}

//void test(void)
//{
//  GPIO_InitTypeDef GPIO_InitStructure;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;//GPIO_OType_OD;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(p->port, &GPIO_InitStructure);
//  GPIO_InitStructure.GPIO_Pin = p->pin;
//
//}