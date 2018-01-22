#ifndef __MAIN
#define __MAIN


#define APP_VERSION_MAJOR                                           2
#define APP_VERSION_MINOR                                           3
#define APP_NODE_NAME                                               "babel.demo"

#define UAVCAN_GET_NODE_INFO_DATA_TYPE_SIGNATURE                    0xee468a8121c46a9e
#define UAVCAN_GET_NODE_INFO_DATA_TYPE_ID                           1
#define UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE                      ((3015 + 7) / 8)

#define UNIQUE_ID_LENGTH_BYTES                                      16

#define UAVCAN_NODE_HEALTH_OK                                       0
#define UAVCAN_NODE_HEALTH_WARNING                                  1
#define UAVCAN_NODE_HEALTH_ERROR                                    2
#define UAVCAN_NODE_HEALTH_CRITICAL                                 3

#define UAVCAN_NODE_MODE_OPERATIONAL                                0
#define UAVCAN_NODE_MODE_INITIALIZATION                             1

#define UAVCAN_NODE_STATUS_MESSAGE_SIZE                             7

#define UAVCAN_NODE_STATUS_MESSAGE_SIZE                             7
#define UAVCAN_NODE_STATUS_DATA_TYPE_ID                             341
#define UAVCAN_NODE_STATUS_DATA_TYPE_SIGNATURE                      0x0f0868d0c1a7c6f1

#define CAN_RX_PIN                 GPIO_Pin_8
#define CAN_TX_PIN                 GPIO_Pin_9
#define CAN_GPIO_PORT              GPIOB
#define CAN_AF_PORT                GPIO_AF_9
#define CAN_RX_SOURCE              GPIO_PinSource8
#define CAN_TX_SOURCE              GPIO_PinSource9

#define LED1_PIN                   GPIO_Pin_8
#define LED2_PIN                   GPIO_Pin_9
#define LED_GPIO_PORT              GPIOE



void hw_init(void);

static inline void gpio_toggle(GPIO_TypeDef * port, uint16_t pin)
{
  if(port->ODR & pin) {port->BRR |= pin;}
  else {port->BSRR |= pin;}
}
#endif 