#include "stm32f1xx.h"
#include "stm32f103c8t6.h"
#include "main.h"

volatile uint32_t uwTick;
uint32_t uwTickPrio   = (1UL << __NVIC_PRIO_BITS); /* Invalid PRIO */
TickFreqTypeDef uwTickFreq = TICK_FREQ_1KHZ;  /* 1KHz */

void IncTick(void)
{
  uwTick += uwTickFreq;
}
																							 
void Delay(uint32_t delay) {
  uint32_t tickstart = GetTick();
  uint32_t wait = delay;

  /* Add a freq to guarantee minimum wait */
  if (wait < 0xFFFFFFFFU)
  {
    wait += (uint32_t)(uwTickFreq);
  }

  while ((GetTick() - tickstart) < wait)
  {
  }
}

uint32_t GetTick(void) {
  return uwTick;
}

void SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{ 
  uint32_t prioritygroup = 0x00U;
  prioritygroup = NVIC_GetPriorityGrouping();
  NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
}

uint32_t SYSTICK_Config(uint32_t TicksNumb)
{
   return SysTick_Config(TicksNumb);
}

void InitTick(uint32_t TickPriority)
{
	if (SYSTICK_Config(SystemCoreClock / (1000U / uwTickFreq)) < 0U) {
    if (TickPriority < (1UL << __NVIC_PRIO_BITS)) {
			SetPriority(SysTick_IRQn, TickPriority, 0U);
			uwTickPrio = TickPriority;
		}
  }
}

extern TIM_HandleTypeDef htim2;

void SysTick_Handler(void)
{
  IncTick();
}

void TIM2_IRQHandler(void)
{
  TIM_IRQHandler(&htim2);
	GPIO_TogglePin(Led_GREEN_GPIO_Port, Led_GREEN_Pin);
}


void MspInit(void)
{
  RCC_AFIO_CLK_ENABLE();
  RCC_PWR_CLK_ENABLE();
  AFIO_REMAP_SWJ_NOJTAG();
}

void I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitStruct_t GPIO_InitStruct = {0};
  if(hi2c->Instance==I2C1)
  {
    RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    RCC_I2C1_CLK_ENABLE();
  }
}

void Init(void){
	InitTick(15U);
	MspInit();
}
