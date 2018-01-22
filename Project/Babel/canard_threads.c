#include "FreeRTOS.h"
#include "task.h"
#include "canard_threads.h"
#include "canard.h"
#include "canard_stm32.h"



static uint8_t node_health = UAVCAN_NODE_HEALTH_OK;
static uint8_t node_mode   = UAVCAN_NODE_MODE_OPERATIONAL;

extern CanardInstance canard;

void canard_status_sender(void * param)
{
  uint32_t timestamp_uS = 0;
  uint8_t buffer[UAVCAN_NODE_STATUS_MESSAGE_SIZE];
  uint8_t transfer_id = 0;
  int res = 0;
  while(1)
  {
    timestamp_uS = xTaskGetTickCount() / configTICK_RATE_HZ;
    canardEncodeScalar(buffer,  0, 32, &timestamp_uS);
    canardEncodeScalar(buffer, 32,  2, &node_health);
    canardEncodeScalar(buffer, 34,  3, &node_mode);
    
    res = canardBroadcast(&canard, 
                          UAVCAN_NODE_STATUS_DATA_TYPE_SIGNATURE,
                          UAVCAN_NODE_STATUS_DATA_TYPE_ID,
                          &transfer_id,
                          CANARD_TRANSFER_PRIORITY_LOW,
                          buffer, 
                          UAVCAN_NODE_STATUS_MESSAGE_SIZE);
    
    const CanardCANFrame* txf = canardPeekTxQueue(&canard);
    res = canardSTM32Transmit(txf);
    canardPopTxQueue(&canard);
    vTaskDelay( 500 / portTICK_PERIOD_MS);
   }
}

void canard_tx(void *param)
{
  while(1)
  {
    for (const CanardCANFrame* txf = NULL; (txf = canardPeekTxQueue(&canard)) != NULL;)
    {
        const int tx_res =  canardSTM32Transmit(txf);
        if (tx_res)         // Failure - drop the frame and report
        {    
          canardPopTxQueue(&canard);
         //   (void)fprintf(stderr, "Transmit error %d, frame dropped, errno '%s'\n", tx_res, strerror(errno));
        }
        else
        {
          break;
        }
    }
vTaskDelay( 10 / portTICK_PERIOD_MS);
  }
}

void canard_rx(void * p)
{
  CanardCANFrame rxf;
  int res=0;
  uint64_t timestamp = 0;
  while(1)
  {
    timestamp = xTaskGetTickCount() * 1000;
    res = canardSTM32Receive(&rxf);
    if (res < 0)             // Failure - report
    {
        //__ASM volatile("BKPT #01"); 
        //(void)fprintf(stderr, "Receive error %d, errno '%s'\n", rx_res, strerror(errno));
      continue;
    }
    if (res > 0)        // Success - process the frame
    {
        canardHandleRxFrame(&canard, &rxf, timestamp);
        continue;
    }
    if(res == 0)
    {
    vTaskDelay( 10 / portTICK_PERIOD_MS);
    }
}

}