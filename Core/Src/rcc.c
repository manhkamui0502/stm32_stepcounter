#include "stm32f1xx.h"
#include "stm32f103c8t6.h"

uint32_t RCC_GetSysClockFreq(void)
{
  const uint8_t aPLLMULFactorTable[16] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16};
  const uint8_t aPredivFactorTable[2] = {1, 2};
  uint32_t tmpreg = 0U, prediv = 0U, pllclk = 0U, pllmul = 0U;
  uint32_t sysclockfreq = 0U;
  tmpreg = RCC->CFGR;

  /* Get SYSCLK source -------------------------------------------------------*/
  switch (tmpreg & RCC_CFGR_SWS)
  {
    case RCC_CFGR_SWS_HSE:  /* HSE used as system clock */
    {
      sysclockfreq = HSE_VALUE;
      break;
    }
    case RCC_CFGR_SWS_PLL:  /* PLL used as system clock */
    {
      pllmul = aPLLMULFactorTable[(uint32_t)(tmpreg & RCC_CFGR_PLLMULL) >> RCC_CFGR_PLLMULL_Pos];
      if ((tmpreg & RCC_CFGR_PLLSRC) != 0x00000000U)
      {
        prediv = aPredivFactorTable[(uint32_t)(RCC->CFGR & RCC_CFGR_PLLXTPRE) >> RCC_CFGR_PLLXTPRE_Pos];
        pllclk = (uint32_t)((HSE_VALUE  * pllmul) / prediv);
      }
      else
      {
        /* HSI used as PLL clock source : PLLCLK = HSI/2 * PLLMUL */
        pllclk = (uint32_t)((HSI_VALUE >> 1) * pllmul);
      }
      sysclockfreq = pllclk;
      break;
    }
    case RCC_CFGR_SWS_HSI:  /* HSI used as system clock source */
    default: /* HSI used as system clock */
    {
      sysclockfreq = HSI_VALUE;
      break;
    }
  }
  return sysclockfreq;
}

static void RCC_Delay(uint32_t mdelay)
{
  __IO uint32_t Delay = mdelay * (SystemCoreClock / 8U / 1000U);
  do
  {
    __NOP();
  }
  while (Delay --);
}

void RCC_GPIOD_CLK_ENABLE(void)
{
	volatile uint32_t tmpreg;
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPDEN);
	/* Delay after an RCC peripheral clock enabling */
	tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPDEN);
	UNUSED(tmpreg);
}

void RCC_GPIOB_CLK_ENABLE(void)
{
	volatile uint32_t tmpreg; 
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);
	/* Delay after an RCC peripheral clock enabling */
	tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);
	UNUSED(tmpreg); 
}

void RCC_GPIOA_CLK_ENABLE(void)
{
	volatile uint32_t tmpreg;
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	/* Delay after an RCC peripheral clock enabling */
	tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	UNUSED(tmpreg); 
}

void RCC_TIM2_CLK_ENABLE(void){ 
	volatile uint32_t tmpreg; 
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);
	/* Delay after an RCC peripheral clock enabling */
	tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);\
	UNUSED(tmpreg); 
}

void RCC_AFIO_CLK_ENABLE(void){ 
	__IO uint32_t tmpreg; 
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);
	/* Delay after an RCC peripheral clock enabling */
	tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);
	UNUSED(tmpreg); 
}

void RCC_I2C1_CLK_ENABLE(void) { 
	__IO uint32_t tmpreg; 
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
	/* Delay after an RCC peripheral clock enabling */
	tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
	UNUSED(tmpreg); 
}

void AFIO_REMAP_SWJ_NOJTAG() {
    AFIO_DBGAFR_CONFIG(AFIO_MAPR_SWJ_CFG_JTAGDISABLE);
}

uint32_t RCC_GetHCLKFreq(void)
{
  return SystemCoreClock;
}
uint32_t RCC_GetPCLK1Freq(void) {
  return (RCC_GetHCLKFreq() >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> 8U]);
}

void RCC_LSE_CONFIG(uint32_t state)
{
    if (state == RCC_LSE_ON)
    {
        SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);
    }
    else if (state == RCC_LSE_OFF)
    {
        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);
        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);
    }
    else if (state == RCC_LSE_BYPASS)
    {
        SET_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);
        SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);
    }
    else
    {
        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);
        CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);
    }
}


void RCC_HSE_CONFIG(uint32_t state)
{
    if (state == RCC_HSE_ON)
    {
        SET_BIT(RCC->CR, RCC_CR_HSEON);
    }
    else if (state == RCC_HSE_OFF)
    {
        CLEAR_BIT(RCC->CR, RCC_CR_HSEON);
        CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
    }
    else if (state == RCC_HSE_BYPASS)
    {
        SET_BIT(RCC->CR, RCC_CR_HSEBYP);
        SET_BIT(RCC->CR, RCC_CR_HSEON);
    }
    else
    {
        CLEAR_BIT(RCC->CR, RCC_CR_HSEON);
        CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
    }
}

StatusTypeDef RCC_OscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct)
{
  uint32_t tickstart;
  uint32_t pll_config;

  /* Check Null pointer */
  if (RCC_OscInitStruct == 0)
  {
    return EERROR;
  }

  /*------------------------------- HSE Configuration ------------------------*/
  if (((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSE) == RCC_OSCILLATORTYPE_HSE)
  {
    /* When the HSE is used as system clock or clock source for PLL in these cases it is not allowed to be disabled */
    if ((RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSE)
        || ((RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK) && (RCC_GET_PLL_OSCSOURCE() == RCC_CFGR_PLLSRC)))
    {
      if ((RCC_GET_FLAG(RCC_FLAG_HSERDY) != RESET) && (RCC_OscInitStruct->HSEState == RCC_HSE_OFF))
      {
        return EERROR;
      }
    }
    else
    {
      /* Set the new HSE configuration ---------------------------------------*/
      RCC_HSE_CONFIG(RCC_OscInitStruct->HSEState);


      /* Check the HSE State */
      if (RCC_OscInitStruct->HSEState != RCC_HSE_OFF)
      {
        /* Get Start Tick */
        tickstart = GetTick();

        /* Wait till HSE is ready */
        while (RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET)
        {
          if ((GetTick() - tickstart) > 100U /*Timeout value*/)
          {
            return TIMEOUT;
          }
        }
      }
      else
      {
        /* Get Start Tick */
        tickstart = GetTick();

        /* Wait till HSE is disabled */
        while (RCC_GET_FLAG(RCC_FLAG_HSERDY) != RESET)
        {
          if ((GetTick() - tickstart) > 100U /*Timeout value*/)
          {
            return TIMEOUT;
          }
        }
      }
    }
  }
  /*----------------------------- HSI Configuration --------------------------*/
  if (((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI)
  {
    /* Check if HSI is used as system clock or as PLL source when PLL is selected as system clock */
    if ((RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSI)
        || ((RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK) && (RCC_GET_PLL_OSCSOURCE() == 0x00000000U)))
    {
      /* When HSI is used as system clock it will not disabled */
      if ((RCC_GET_FLAG(RCC_FLAG_HSIRDY) != RESET) && (RCC_OscInitStruct->HSIState != RCC_HSI_ON))
      {
        return EERROR;
      }
      /* Otherwise, just the calibration is allowed */
      else
      {
        /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
        RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct->HSICalibrationValue);
      }
    }
    else
    {
      /* Check the HSI State */
      if (RCC_OscInitStruct->HSIState != RCC_HSI_OFF)
      {
        /* Enable the Internal High Speed oscillator (HSI). */
        RCC_HSI_ENABLE();

        /* Get Start Tick */
        tickstart = GetTick();

        /* Wait till HSI is ready */
        while (RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET)
        {
          if ((GetTick() - tickstart) > 100U)
          {
            return TIMEOUT;
          }
        }

        /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
        RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct->HSICalibrationValue);
      }
      else
      {
        /* Disable the Internal High Speed oscillator (HSI). */
        RCC_HSI_DISABLE();

        /* Get Start Tick */
        tickstart = GetTick();

        /* Wait till HSI is disabled */
        while (RCC_GET_FLAG(RCC_FLAG_HSIRDY) != RESET)
        {
          if ((GetTick() - tickstart) > 100U)
          {
            return TIMEOUT;
          }
        }
      }
    }
  }
  /*------------------------------ LSI Configuration -------------------------*/
  if (((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI)
  {

    /* Check the LSI State */
    if (RCC_OscInitStruct->LSIState != 0x00000000U)
    {
      /* Enable the Internal Low Speed oscillator (LSI). */
      RCC_LSI_ENABLE();

      /* Get Start Tick */
      tickstart = GetTick();

      /* Wait till LSI is ready */
      while (RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET)
      {
        if ((GetTick() - tickstart) > 2U)
        {
          return TIMEOUT;
        }
      }
      /*  To have a fully stabilized clock in the specified range, a software delay of 1ms
          should be added.*/
      RCC_Delay(1);
    }
    else
    {
      /* Disable the Internal Low Speed oscillator (LSI). */
      RCC_LSI_DISABLE();

      /* Get Start Tick */
      tickstart = GetTick();

      /* Wait till LSI is disabled */
      while (RCC_GET_FLAG(RCC_FLAG_LSIRDY) != RESET)
      {
        if ((GetTick() - tickstart) > 2U)
        {
          return TIMEOUT;
        }
      }
    }
  }
  /*------------------------------ LSE Configuration -------------------------*/
  if (((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE)
  {
    FlagStatus pwrclkchanged = RESET;
    /* Update LSE configuration in Backup Domain control register    */
    /* Requires to enable write access to Backup Domain of necessary */
    if (RCC_PWR_IS_CLK_DISABLED())
    {
      RCC_PWR_CLK_ENABLE();
      pwrclkchanged = SET;
    }

    if (IS_BIT_CLR(PWR->CR, PWR_CR_DBP))
    {
      /* Enable write access to Backup domain */
      SET_BIT(PWR->CR, PWR_CR_DBP);

      /* Wait for Backup domain Write protection disable */
      tickstart = GetTick();

      while (IS_BIT_CLR(PWR->CR, PWR_CR_DBP))
      {
        if ((GetTick() - tickstart) > 100U) //100ms
        {
          return TIMEOUT;
        }
      }
    }

    /* Set the new LSE configuration -----------------------------------------*/
    RCC_LSE_CONFIG(RCC_OscInitStruct->LSEState);
    /* Check the LSE State */
    if (RCC_OscInitStruct->LSEState != RCC_LSE_OFF)
    {
      /* Get Start Tick */
      tickstart = GetTick();

      /* Wait till LSE is ready */
      while (RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET)
      {
        if ((GetTick() - tickstart) > 100U)
        {
          return TIMEOUT;
        }
      }
    }
    else
    {
      /* Get Start Tick */
      tickstart = GetTick();

      /* Wait till LSE is disabled */
      while (RCC_GET_FLAG(RCC_FLAG_LSERDY) != RESET)
      {
        if ((GetTick() - tickstart) > 100U)
        {
          return TIMEOUT;
        }
      }
    }

    /* Require to disable power clock if necessary */
    if (pwrclkchanged == SET)
    {
      RCC_PWR_CLK_DISABLE();
    }
  }

  /*-------------------------------- PLL Configuration -----------------------*/
  /* Check the parameters */
  if ((RCC_OscInitStruct->PLL.PLLState) != RCC_PLL_NONE)
  {
    /* Check if the PLL is used as system clock or not */
    if (RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK)
    {
      if ((RCC_OscInitStruct->PLL.PLLState) == RCC_PLL_ON)
      {
        /* Disable the main PLL. */
        RCC_PLL_DISABLE();

        /* Get Start Tick */
        tickstart = GetTick();

        /* Wait till PLL is disabled */
        while (RCC_GET_FLAG(RCC_FLAG_PLLRDY)  != RESET)
        {
          if ((GetTick() - tickstart) > 2U)
          {
            return TIMEOUT;
          }
        }

        /* Configure the HSE prediv factor --------------------------------*/
        /* It can be written only when the PLL is disabled. Not used in PLL source is different than HSE */
        if (RCC_OscInitStruct->PLL.PLLSource == RCC_PLLSOURCE_HSE)
        {
          /* Set PREDIV1 Value */
          RCC_HSE_PREDIV_CONFIG(RCC_OscInitStruct->HSEPredivValue);
        }

        /* Configure the main PLL clock source and multiplication factors. */
        RCC_PLL_CONFIG(RCC_OscInitStruct->PLL.PLLSource,
                             RCC_OscInitStruct->PLL.PLLMUL);
        /* Enable the main PLL. */
        RCC_PLL_ENABLE();

        /* Get Start Tick */
        tickstart = GetTick();

        /* Wait till PLL is ready */
        while (RCC_GET_FLAG(RCC_FLAG_PLLRDY)  == RESET)
        {
          if ((GetTick() - tickstart) > 2U)
          {
            return TIMEOUT;
          }
        }
      }
      else
      {
        /* Disable the main PLL. */
        RCC_PLL_DISABLE();

        /* Get Start Tick */
        tickstart = GetTick();

        /* Wait till PLL is disabled */
        while (RCC_GET_FLAG(RCC_FLAG_PLLRDY)  != RESET)
        {
          if ((GetTick() - tickstart) > 2U)
          {
            return TIMEOUT;
          }
        }
      }
    }
    else
    {
      /* Check if there is a request to disable the PLL used as System clock source */
      if ((RCC_OscInitStruct->PLL.PLLState) == RCC_PLL_OFF)
      {
        return EERROR;
      }
      else
      {
        /* Do not return ERROR if request repeats the current configuration */
        pll_config = RCC->CFGR;
        if ((READ_BIT(pll_config, RCC_CFGR_PLLSRC) != RCC_OscInitStruct->PLL.PLLSource) ||
            (READ_BIT(pll_config, RCC_CFGR_PLLMULL) != RCC_OscInitStruct->PLL.PLLMUL))
        {
          return EERROR;
        }
      }
    }
  }
  return OK;
}

StatusTypeDef RCC_ClockConfig(RCC_ClkInitTypeDef  *RCC_ClkInitStruct, uint32_t FLatency)
{
  uint32_t tickstart;

  /* Check Null pointer */
  if (RCC_ClkInitStruct == 0)
  {
    return EERROR;
  }

/*-------------------------- HCLK Configuration --------------------------*/
if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_HCLK) == RCC_CLOCKTYPE_HCLK) {
    if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK1) == RCC_CLOCKTYPE_PCLK1) {
      MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV16);
    }
    if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK2) == RCC_CLOCKTYPE_PCLK2) {
      MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, (RCC_CFGR_PPRE1_DIV16 << 3));
    }
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_ClkInitStruct->AHBCLKDivider);
  }

  /*------------------------- SYSCLK Configuration ---------------------------*/
  if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_SYSCLK) == RCC_CLOCKTYPE_SYSCLK)
  {
    /* HSE is selected as System Clock Source */
    if (RCC_ClkInitStruct->SYSCLKSource == RCC_CFGR_SW_HSE)
    {
      /* Check the HSE ready flag */
      if (RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET)
      {
        return EERROR;
      }
    }
    /* PLL is selected as System Clock Source */
    else if (RCC_ClkInitStruct->SYSCLKSource == RCC_CFGR_SW_PLL)
    {
      /* Check the PLL ready flag */
      if (RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
      {
        return EERROR;
      }
    }
    /* HSI is selected as System Clock Source */
    else
    {
      /* Check the HSI ready flag */
      if (RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET)
      {
        return EERROR;
      }
    }
    RCC_SYSCLK_CONFIG(RCC_ClkInitStruct->SYSCLKSource);

    /* Get Start Tick */
    tickstart = GetTick();

    while (RCC_GET_SYSCLK_SOURCE() != (RCC_ClkInitStruct->SYSCLKSource << RCC_CFGR_SWS_Pos))
    {
      if ((GetTick() - tickstart) > 5000)
      {
        return TIMEOUT;
      }
    }
  }
/*-------------------------- PCLK1 Configuration ---------------------------*/
if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK1) == RCC_CLOCKTYPE_PCLK1)
  {
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_ClkInitStruct->APB1CLKDivider);
  }
  /*-------------------------- PCLK2 Configuration ---------------------------*/
  if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK2) == RCC_CLOCKTYPE_PCLK2)
  {
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, ((RCC_ClkInitStruct->APB2CLKDivider) << 3));
  }
  /* Update the SystemCoreClock global variable */
  SystemCoreClock = RCC_GetSysClockFreq() >> AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos];
  /* Configure the source of time base considering new system clocks settings*/
  InitTick(uwTickPrio);
  return OK;
}

	