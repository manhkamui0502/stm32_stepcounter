#ifndef __DEF_H
#define __DEF_H

#include "stm32f1xx.h"
#include <stddef.h>

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */

typedef enum
{
  OK       = 0x00U,
  EERROR    = 0x01U,
  BUSY     = 0x02U,
  TIMEOUT  = 0x03U
} StatusTypeDef;

typedef enum
{
  UNLOCKED = 0x00U,
  LOCKED   = 0x01U
} LockTypeDef;

typedef struct
{
  uint32_t Direction;
  uint32_t PeriphInc;
  uint32_t MemInc;
  uint32_t PeriphDataAlignment;
  uint32_t MemDataAlignment;
  uint32_t Mode;
  uint32_t Priority;                   
} DMA_InitTypeDef;

typedef enum
{
  DMA_STATE_RESET             = 0x00U,  /*!< DMA not yet initialized or disabled    */
  DMA_STATE_READY             = 0x01U,  /*!< DMA initialized and ready for use      */
  DMA_STATE_BUSY              = 0x02U,  /*!< DMA process is ongoing                 */
  DMA_STATE_TIMEOUT           = 0x03U   /*!< DMA timeout state                      */
}DMA_StateTypeDef;

typedef struct __DMA_HandleTypeDef
{
  DMA_Channel_TypeDef   *Instance; 
  DMA_InitTypeDef       Init;
  LockTypeDef       Lock; 
  DMA_StateTypeDef  State;
  void                  *Parent;                                                      /*!< Parent object state                    */  
  void                  (* XferCpltCallback)( struct __DMA_HandleTypeDef * hdma);     /*!< DMA transfer complete callback         */
  void                  (* XferHalfCpltCallback)( struct __DMA_HandleTypeDef * hdma); /*!< DMA Half transfer complete callback    */
  void                  (* XferErrorCallback)( struct __DMA_HandleTypeDef * hdma);    /*!< DMA transfer error callback            */
  void                  (* XferAbortCallback)( struct __DMA_HandleTypeDef * hdma);    /*!< DMA transfer abort callback            */  
  __IO uint32_t         ErrorCode;                                                    /*!< DMA Error code                         */
  DMA_TypeDef            *DmaBaseAddress;                                             /*!< DMA Channel Base Address               */
  uint32_t               ChannelIndex;                                                /*!< DMA Channel Index                      */  
} DMA_HandleTypeDef;    

typedef enum
{
  I2C_MODE_NONE               = 0x00U,   /*!< No I2C communication on going             */
  I2C_MODE_MASTER             = 0x10U,   /*!< I2C communication is in Master Mode       */
  I2C_MODE_SLAVE              = 0x20U,   /*!< I2C communication is in Slave Mode        */
  I2C_MODE_MEM                = 0x40U    /*!< I2C communication is in Memory Mode       */
} I2C_ModeTypeDef;

typedef struct
{
  uint32_t ClockSpeed;       
  uint32_t DutyCycle;       
  uint32_t OwnAddress1;      
  uint32_t AddressingMode;   
  uint32_t DualAddressMode; 
  uint32_t OwnAddress2;      
  uint32_t GeneralCallMode;  
  uint32_t NoStretchMode;    
} I2C_InitTypeDef;

typedef enum
{
  I2C_STATE_RESET             = 0x00U,   /*!< Peripheral is not yet Initialized         */
  I2C_STATE_READY             = 0x20U,   /*!< Peripheral Initialized and ready for use  */
  I2C_STATE_BUSY              = 0x24U,   /*!< An internal process is ongoing            */
  I2C_STATE_BUSY_TX           = 0x21U,   /*!< Data Transmission process is ongoing      */
  I2C_STATE_BUSY_RX           = 0x22U,   /*!< Data Reception process is ongoing         */
  I2C_STATE_LISTEN            = 0x28U,   /*!< Address Listen Mode is ongoing            */
  I2C_STATE_BUSY_TX_LISTEN    = 0x29U,   /*!< Address Listen Mode and Data Transmission
                                                 process is ongoing                         */
  I2C_STATE_BUSY_RX_LISTEN    = 0x2AU,   /*!< Address Listen Mode and Data Reception
                                                 process is ongoing                         */
  I2C_STATE_ABORT             = 0x60U,   /*!< Abort user request ongoing                */
  I2C_STATE_TIMEOUT           = 0xA0U,   /*!< Timeout state                             */
  I2C_STATE_ERROR             = 0xE0U    /*!< Error                                     */

} I2C_StateTypeDef;

typedef struct {
  I2C_TypeDef                		 *Instance;      /*!< I2C registers base address               */
  I2C_InitTypeDef            		 Init;           /*!< I2C communication parameters             */
  uint8_t                    		 *pBuffPtr;      /*!< Pointer to I2C transfer buffer           */
  uint16_t                   		 XferSize;       /*!< I2C transfer size                        */
  volatile uint16_t              XferCount;      /*!< I2C transfer counter                     */
  volatile uint32_t              XferOptions;    /*!< I2C transfer options                     */
  volatile uint32_t              PreviousState;  /*!< I2C communication Previous state and mode
                                                  context for internal usage               */
  volatile I2C_StateTypeDef  		 State;          /*!< I2C communication state                  */
	volatile I2C_ModeTypeDef   		 Mode;           /*!< I2C communication mode                   */
	
	DMA_HandleTypeDef          *hdmatx;        /*!< I2C Tx DMA handle parameters             */
  DMA_HandleTypeDef          *hdmarx;        /*!< I2C Rx DMA handle parameters             */
	
	LockTypeDef								 		 Lock;
	volatile uint32_t              ErrorCode;      /*!< I2C Error code                           */
  volatile uint32_t              Devaddress;     /*!< I2C Target device address                */
  volatile uint32_t              Memaddress;     /*!< I2C Target memory address                */
  volatile uint32_t              MemaddSize;     /*!< I2C Target memory address  size          */
  volatile uint32_t              EventCount;     /*!< I2C Event counter                        */

} I2C_HandleTypeDef;

#endif /* __DEF_H */