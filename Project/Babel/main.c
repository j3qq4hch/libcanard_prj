
#include "stm32f37x_conf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "misc.h"
#include "main.h"
#include <stdio.h>
#include "canard.h"
#include "canard_stm32.h"
#include "timer_thread.h"
#include "canard_threads.h"

RCC_ClocksTypeDef RCC_Clocks;
CanardSTM32CANTimings timings;
CanardInstance canard;                       //< The library instance
static uint8_t canard_memory_pool[1024];     //< Arena for memory allocation, used by the library

int res = 0;

led_blinker_param blink1_param = 
{
GPIO_Pin_8,
GPIOE,
800
};

timer_thread_p timer1 = {TIM12, 1, TIMER_MODE_OUTPUT};
timer_thread_p timer2 = {TIM12, 2, TIMER_MODE_OUTPUT};
timer_thread_p timer3 = {TIM3,  1, TIMER_MODE_OUTPUT};
timer_thread_p timer4 = {TIM3,  2, TIMER_MODE_OUTPUT};
timer_thread_p timer5 = {TIM3,  3, TIMER_MODE_OUTPUT};
timer_thread_p timer6 = {TIM3,  4, TIMER_MODE_OUTPUT};

/////////////////////////////////////////////////////////////////////////////////

static void onTransferReceived(CanardInstance* ins,
                               CanardRxTransfer* transfer)
{

//  if ((transfer->transfer_type == CanardTransferTypeRequest) &&
//        (transfer->data_type_id == UAVCAN_GET_NODE_INFO_DATA_TYPE_ID))
//    {
//        uint8_t buffer[UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE];
//        memset(buffer, 0, UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE);
//        const size_t name_len = strlen(APP_NODE_NAME);
//        memcpy(&buffer[41], APP_NODE_NAME, name_len);
//       const size_t total_size = 41 + name_len;
//
//        const int resp_res = canardRequestOrRespond(ins,
//                                                    transfer->source_node_id,
//                                                    UAVCAN_GET_NODE_INFO_DATA_TYPE_SIGNATURE,
//                                                    UAVCAN_GET_NODE_INFO_DATA_TYPE_ID,
//                                                    &transfer->transfer_id,
//                                                    transfer->priority,
//                                                    CanardResponse,
//                                                    &buffer[0],
//                                                    (uint16_t)total_size);
//        if (resp_res <= 0)
//        {
//            (void)fprintf(stderr, "Could not respond to GetNodeInfo; error %d\n", resp_res);
//        }
//    }
//  
}

/////////////////////////////////////////////////////////////////////////////////

static bool shouldAcceptTransfer(const CanardInstance* ins,
                                 uint64_t* out_data_type_signature,
                                 uint16_t data_type_id,
                                 CanardTransferType transfer_type,
                                 uint8_t source_node_id)
{

//  if ((transfer_type == CanardTransferTypeRequest) &&
//            (data_type_id == UAVCAN_GET_NODE_INFO_DATA_TYPE_ID))
//        {
//            *out_data_type_signature = UAVCAN_GET_NODE_INFO_DATA_TYPE_SIGNATURE;
//            return true;
//        }
  
  
  return true;
}

/////////////////////////////////////////////////////////////////////////////////


int main(void)
{ 
  RCC_GetClocksFreq(&RCC_Clocks);
  hw_init();

  res = canardSTM32ComputeCANTimings(RCC_Clocks.PCLK1_Frequency, 1000000, &timings);
  if(res)
  {
  __ASM volatile("BKPT #01"); 
  }
  res = canardSTM32Init(&timings, CanardSTM32IfaceModeNormal);
  if(res)
  {
  __ASM volatile("BKPT #01"); 
  }
  
  canardInit(&canard,                           //< Uninitialized library instance
             canard_memory_pool,                ///< Raw memory chunk used for dynamic allocation
             sizeof(canard_memory_pool),        //< Size of the above, in bytes
             onTransferReceived,                //< Callback, see CanardOnTransferReception
             shouldAcceptTransfer,              //< Callback, see CanardShouldAcceptTransfer
             NULL); 
  
  canardSetLocalNodeID(&canard, 100);
 
  xTaskCreate( led_blinker,                     /* Function that implements the task. */
              "blink1",                         /* Text name for the task. */
              configMINIMAL_STACK_SIZE,         /* Stack size in words, not bytes. */
              &blink1_param ,                   /* Parameter passed into the task. */
              tskIDLE_PRIORITY+1,               /* Priority at which the task is created. */
              NULL);
  
  xTaskCreate( canard_status_sender,            /* Function that implements the task. */
              "can_stat",                       /* Text name for the task. */
              configMINIMAL_STACK_SIZE * 2,     /* Stack size in words, not bytes. */
              NULL,                             /* Parameter passed into the task. */
              tskIDLE_PRIORITY+1,               /* Priority at which the task is created. */
              NULL);
 
   xTaskCreate( canard_rx,            /* Function that implements the task. */
              "can_rx",                       /* Text name for the task. */
              configMINIMAL_STACK_SIZE * 2,     /* Stack size in words, not bytes. */
              NULL,                             /* Parameter passed into the task. */
              tskIDLE_PRIORITY+1,               /* Priority at which the task is created. */
              NULL);
   
    xTaskCreate(canard_tx,            /* Function that implements the task. */
              "can_tx",                       /* Text name for the task. */
              configMINIMAL_STACK_SIZE * 2,     /* Stack size in words, not bytes. */
              NULL,                             /* Parameter passed into the task. */
              tskIDLE_PRIORITY+1,               /* Priority at which the task is created. */
              NULL);
    
  SysTick_Config(SystemCoreClock / 1000);
  vTaskStartScheduler();
}

/////////////////////////////////////////////////////////////////
