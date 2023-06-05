#ifndef STM32F103C8T6_H
#define STM32F103C8T6_H
#include "stm32f1xx.h"
#include "stm32_def.h"
#include "i2c.h"
#include "gpio.h"
#include "rcc.h"
#include "tim2.h"

#define IS_BIT_SET(REG, BIT)         (((REG) & (BIT)) != 0U)
#define IS_BIT_CLR(REG, BIT)         (((REG) & (BIT)) == 0U)

typedef enum
{
  TICK_FREQ_10HZ         = 100U,
  TICK_FREQ_100HZ        = 10U,
  TICK_FREQ_1KHZ         = 1U,
  TICK_FREQ_DEFAULT      = TICK_FREQ_1KHZ
} TickFreqTypeDef;

extern __IO uint32_t uwTick;
extern uint32_t uwTickPrio;
extern TickFreqTypeDef uwTickFreq;

void IncTick(void);
uint32_t GetTick(void);
void Delay(uint32_t delay);
void InitTick(uint32_t TickPriority);
void MspInit(void);
void I2C_MspInit(I2C_HandleTypeDef* hi2c);
void Init(void);
#endif /* STM32F103C8T6_H */