#include "stm32f1xx.h"
#include "stm32f103c8t6.h"

void TIM_DISABLE(TIM_HandleTypeDef *htim){ 
	if ((htim->Instance->CCER & TIM_CCER_CCxE_MASK) == 0UL) { 
		if((htim->Instance->CCER & TIM_CCER_CCxNE_MASK) == 0UL) { 
			htim->Instance->CR1 &= ~(TIM_CR1_CEN); 
		} 
	} 
}

void TIM_ENABLE(TIM_HandleTypeDef *htim) {
	htim->Instance->CR1|=(TIM_CR1_CEN);
}
void TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM2)
  {
    RCC_TIM2_CLK_ENABLE();
    NVIC_EnableIRQ(TIM2_IRQn);
  }
}

void TIM_IRQHandler(TIM_HandleTypeDef *htim)
{
  if (TIM_GET_FLAG(htim, TIM_SR_UIF_Msk) != RESET)
  {
    if (TIM_GET_IT_SOURCE(htim, TIM_IT_UPDATE) != RESET)
    {
      TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
    }
  }
}

void TIM_Base_SetConfig(TIM_TypeDef *TIMx, TIM_Base_InitTypeDef *Structure)
{
  uint32_t tmpcr1;
  tmpcr1 = TIMx->CR1;
	tmpcr1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
	tmpcr1 |= Structure->CounterMode;
	tmpcr1 &= ~TIM_CR1_CKD;
	tmpcr1 |= (uint32_t)Structure->ClockDivision;
  /* Set the auto-reload preload */
  MODIFY_REG(tmpcr1, TIM_CR1_ARPE, Structure->AutoReloadPreload);
  TIMx->CR1 = tmpcr1;
  /* Set the Autoreload value */
  TIMx->ARR = (uint32_t)Structure->Period ;
  /* Set the Prescaler value */
  TIMx->PSC = Structure->Prescaler;
	/* Set the Repetition Counter value */
	TIMx->RCR = Structure->RepetitionCounter;
  /* Generate an update event to reload the Prescaler
     and the repetition counter (only for advanced timer) value immediately */
  TIMx->EGR = TIM_EGR_UG;
}

void TIM_Base_Init(TIM_HandleTypeDef *htim) {
	TIM_Base_MspInit(htim);
	htim->State = TIM_STATE_BUSY;
  TIM_Base_SetConfig(htim->Instance, &htim->Init);
  htim->DMABurstState = DMA_BURST_STATE_READY;
  htim->State = TIM_STATE_READY;
}
void TIM_Base_Start_IT(TIM_HandleTypeDef *htim) {
  uint32_t tmpsmcr;
  htim->State = TIM_STATE_BUSY;
  TIM_ENABLE_IT(htim, TIM_IT_UPDATE);
	TIM_ENABLE(htim);
}
void TIM_Base_Stop_IT(TIM_HandleTypeDef *htim)
{
  TIM_DISABLE_IT(htim, TIM_IT_UPDATE);
  TIM_DISABLE(htim);
  htim->State = TIM_STATE_READY;
}