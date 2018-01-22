#include "timer_thread.h"


static void timer_config(timer_thread_p * p )
{
 //TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 //TIM_ICInitTypeDef  TIM_ICInitStructure;
 //TIM_OCInitTypeDef  TIM_OCInitStructure;
 //TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
}


void timer_thread(void * param)
{
timer_thread_p * p = param;
u8 restart_flag = 0;
while(!restart_flag)
{
//timer_config(p);
}
{   

}
}