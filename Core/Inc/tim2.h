#ifndef TIM2_H
#define TIM2_H
#include "stm32f1xx.h"
#include "stm32f103c8t6.h"

#define TIM_CCER_CCxE_MASK  ((uint32_t)(TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E))
#define TIM_CCER_CCxNE_MASK ((uint32_t)(TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE))
#define TIM_CLOCKSOURCE_INTERNAL    TIM_SMCR_ETPS_0 
#define TIM_ENABLE_IT(__HANDLE__, __INTERRUPT__)    ((__HANDLE__)->Instance->DIER |= (__INTERRUPT__))
#define TIM_DISABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->DIER &= ~(__INTERRUPT__))
#define TIM_IT_UPDATE                      TIM_DIER_UIE                         /*!< Update interrupt            */
#define TIM_CLEAR_IT(__HANDLE__, __INTERRUPT__)      ((__HANDLE__)->Instance->SR = ~(__INTERRUPT__))
#define TIM_GET_FLAG(__HANDLE__, __FLAG__)          (((__HANDLE__)->Instance->SR &(__FLAG__)) == (__FLAG__))
#define TIM_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->DIER & (__INTERRUPT__)) \
                                                             == (__INTERRUPT__)) ? SET : RESET)
typedef struct
{
  uint32_t Prescaler;     
  uint32_t CounterMode;       
  uint32_t Period;           
  uint32_t ClockDivision;     
  uint32_t RepetitionCounter;  
  uint32_t AutoReloadPreload; 
} TIM_Base_InitTypeDef;
typedef enum
{
  DMA_BURST_STATE_RESET             = 0x00U,    /*!< DMA Burst initial state */
  DMA_BURST_STATE_READY             = 0x01U,    /*!< DMA Burst ready for use */
  DMA_BURST_STATE_BUSY              = 0x02U,    /*!< Ongoing DMA Burst       */
} TIM_DMABurstStateTypeDef;

typedef enum
{
  TIM_STATE_RESET             = 0x00U,    /*!< Peripheral not yet initialized or disabled  */
  TIM_STATE_READY             = 0x01U,    /*!< Peripheral Initialized and ready for use    */
  TIM_STATE_BUSY              = 0x02U,    /*!< An internal process is ongoing              */
  TIM_STATE_TIMEOUT           = 0x03U,    /*!< Timeout state                               */
  TIM_STATE_ERROR             = 0x04U     /*!< Reception process is ongoing                */
} TIM_StateTypeDef;

typedef struct {
  TIM_TypeDef                    *Instance;         /*!< Register base address                             */
  TIM_Base_InitTypeDef           Init;              /*!< TIM Time Base required parameters                 */
  __IO TIM_StateTypeDef          State;             /*!< TIM operation state                               */
  __IO TIM_DMABurstStateTypeDef  DMABurstState;     /*!< DMA burst operation state                         */                    
} TIM_HandleTypeDef;

typedef struct {
  uint32_t ClockSource;
  uint32_t ClockPolarity;        
  uint32_t ClockPrescaler;       
  uint32_t ClockFilter;
} TIM_ClockConfigTypeDef;

void TIM_Base_Init(TIM_HandleTypeDef *htim);
void TIM_Base_Stop_IT(TIM_HandleTypeDef *htim);
void TIM_Base_Start_IT(TIM_HandleTypeDef *htim);
void TIM_IRQHandler(TIM_HandleTypeDef *htim);

#endif /* TIM2_H */
