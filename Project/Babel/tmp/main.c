
#include "stm32f37x_conf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "misc.h"
#include "canard.h"
#include "canard_stm32.h"
#include "main.h"
#include <stdio.h>

void readUniqueID(uint8_t* out_uid);
void makeNodeStatusMessage(uint8_t buffer[UAVCAN_NODE_STATUS_MESSAGE_SIZE]);

RCC_ClocksTypeDef RCC_Clocks;
CanardSTM32CANTimings timings;
static CanardInstance canard;                       ///< The library instance
static uint8_t canard_memory_pool[1024];            ///< Arena for memory allocation, used by the library

static uint8_t node_health = UAVCAN_NODE_HEALTH_OK;
static uint8_t node_mode   = UAVCAN_NODE_MODE_INITIALIZATION;

int res = 0;
/////////////////////////////////////////////////////////////////////////////////

static void onTransferReceived(CanardInstance* ins,
                               CanardRxTransfer* transfer)
{

  if ((transfer->transfer_type == CanardTransferTypeRequest) &&
        (transfer->data_type_id == UAVCAN_GET_NODE_INFO_DATA_TYPE_ID))
    {
        //printf("GetNodeInfo request from %d\n", transfer->source_node_id);

        uint8_t buffer[UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE];
        memset(buffer, 0, UAVCAN_GET_NODE_INFO_RESPONSE_MAX_SIZE);

        // NodeStatus
        //makeNodeStatusMessage(buffer);

        // SoftwareVersion
        buffer[7] = APP_VERSION_MAJOR;
        buffer[8] = APP_VERSION_MINOR;
        buffer[9] = 1;                          // Optional field flags, VCS commit is set
        uint32_t uu32 = 0xDEADBEEF; //GIT_HASH;
        canardEncodeScalar(buffer, 80, 32, &uu32);
        // Image CRC skipped

        // HardwareVersion
        // Major skipped
        // Minor skipped
        //readUniqueID(&buffer[24]);
        // Certificate of authenticity skipped

        // Name
        const size_t name_len = strlen(APP_NODE_NAME);
        memcpy(&buffer[41], APP_NODE_NAME, name_len);

        const size_t total_size = 41 + name_len;

        /*
         * Transmitting; in this case we don't have to release the payload because it's empty anyway.
         */
        const int resp_res = canardRequestOrRespond(ins,
                                                    transfer->source_node_id,
                                                    UAVCAN_GET_NODE_INFO_DATA_TYPE_SIGNATURE,
                                                    UAVCAN_GET_NODE_INFO_DATA_TYPE_ID,
                                                    &transfer->transfer_id,
                                                    transfer->priority,
                                                    CanardResponse,
                                                    &buffer[0],
                                                    (uint16_t)total_size);
        if (resp_res <= 0)
        {
            (void)fprintf(stderr, "Could not respond to GetNodeInfo; error %d\n", resp_res);
        }
    }
  
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
  res = canardSTM32Init(&timings, CanardSTM32IfaceModeNormal);

  canardInit(&canard,                           //< Uninitialized library instance
             canard_memory_pool,                ///< Raw memory chunk used for dynamic allocation
             sizeof(canard_memory_pool),        //< Size of the above, in bytes
             onTransferReceived,                //< Callback, see CanardOnTransferReception
             shouldAcceptTransfer,              //< Callback, see CanardShouldAcceptTransfer
             NULL); 
  canardSetLocalNodeID(&canard, 100);
 
  u32 timestamp = 0xABABABAB;
  uint8_t buffer[UAVCAN_NODE_STATUS_MESSAGE_SIZE];
  static uint8_t transfer_id = 0;
  
  while(1)
  {    
   gpio_toggle(LED_GPIO_PORT, LED1_PIN);
   gpio_toggle(LED_GPIO_PORT, LED2_PIN);
   
    canardEncodeScalar(buffer,  0, 32, &timestamp);
    canardEncodeScalar(buffer, 32,  2, &node_health);
    canardEncodeScalar(buffer, 34,  3, &node_mode);
    
    res = canardBroadcast(&canard, UAVCAN_NODE_STATUS_DATA_TYPE_SIGNATURE,
                                           UAVCAN_NODE_STATUS_DATA_TYPE_ID, &transfer_id, CANARD_TRANSFER_PRIORITY_LOW,
                                           buffer, UAVCAN_NODE_STATUS_MESSAGE_SIZE);
    
    const CanardCANFrame* txf = canardPeekTxQueue(&canard);
    
    res = canardSTM32Transmit(txf);

    for(u32 i = 0; i < 0x5FFFF; i++){}
    canardPopTxQueue(&canard);
  }
}

/////////////////////////////////////////////////////////////////

void hw_init(void)
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
//
//void canard_thread(void)
//{
//
//}