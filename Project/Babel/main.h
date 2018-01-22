#ifndef __MAIN
#define __MAIN




#define CAN_RX_PIN                 GPIO_Pin_8
#define CAN_TX_PIN                 GPIO_Pin_9
#define CAN_GPIO_PORT              GPIOB
#define CAN_AF_PORT                GPIO_AF_9
#define CAN_RX_SOURCE              GPIO_PinSource8
#define CAN_TX_SOURCE              GPIO_PinSource9

#define LED1_PIN                   GPIO_Pin_8
#define LED2_PIN                   GPIO_Pin_9
#define LED_GPIO_PORT              GPIOE


static void hw_init(void)
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOE, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
 
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
  GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT); 

  GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; // GPIO_OType_PP 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = LED1_PIN | LED2_PIN;
  GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
}

void canard_thread(void *param);




void timer_thread(void * param);
#endif 