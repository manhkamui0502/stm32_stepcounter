#include "stm32f1xx.h"
#include "stm32f103c8t6.h"

FlagStatus I2C_GET_FLAG(I2C_HandleTypeDef *hi2c, uint32_t flag) {
    if (((uint8_t)((flag) >> 16U)) == 0x01U) {
        if ((((hi2c)->Instance->SR1) & ((flag) & I2C_FLAG_MASK)) == ((flag) & I2C_FLAG_MASK)) {
            // Flag is set
            return SET;
        }
        else {
            // Flag is not set
            return RESET;
        }
    }
    else {
        if ((((hi2c)->Instance->SR2) & ((flag) & I2C_FLAG_MASK)) == ((flag) & I2C_FLAG_MASK)) {
            // Flag is set
            return SET;
        }
        else {
            // Flag is not set
            return RESET;
        }
    }
}

void I2C_CLEAR_FLAG(I2C_HandleTypeDef *hi2c, uint32_t flag) {
	((hi2c)->Instance->SR1 = ~((flag) & I2C_FLAG_MASK));
}
void I2C_ENABLE(I2C_HandleTypeDef *hi2c) {
    SET_BIT((hi2c)->Instance->CR1, I2C_CR1_PE);
}

void I2C_DISABLE(I2C_HandleTypeDef *hi2c) {
    CLEAR_BIT((hi2c)->Instance->CR1, I2C_CR1_PE);
}

static StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart)
{
  /* Wait until flag is set */
  while (I2C_GET_FLAG(hi2c, Flag) == Status)
  {
    /* Check for the Timeout */
    if (Timeout != MAX_DELAY)
    {
      if (((GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {
        hi2c->PreviousState     = 0x00000000U;
        hi2c->State             = I2C_STATE_READY;
        hi2c->Mode              = I2C_MODE_NONE;
        hi2c->ErrorCode         |= I2C_ERROR_TIMEOUT;

        /* Process Unlocked */
        hi2c->Lock = UNLOCKED;

        return EERROR;
      }
    }
  }
  return OK;
}

static StatusTypeDef I2C_WaitOnMasterAddressFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, uint32_t Timeout, uint32_t Tickstart)
{
  while (I2C_GET_FLAG(hi2c, Flag) == RESET)
  {
    if (I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
    {
      /* Generate Stop */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

      /* Clear AF Flag */
      I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

      hi2c->PreviousState       = I2C_STATE_NONE;
      hi2c->State               = I2C_STATE_READY;
      hi2c->Mode                = I2C_MODE_NONE;
      hi2c->ErrorCode           |= I2C_ERROR_AF;
      /* Process unlocked */
      hi2c->Lock = UNLOCKED;
      return EERROR;
    }

    /* Check for the Timeout */
    if (Timeout != MAX_DELAY)
    {
      if (((GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {
        hi2c->PreviousState       = I2C_STATE_NONE;
        hi2c->State               = I2C_STATE_READY;
        hi2c->Mode                = I2C_MODE_NONE;
        hi2c->ErrorCode           |= I2C_ERROR_TIMEOUT;
        /* Process Unlocked */
        hi2c->Lock = UNLOCKED;
        return EERROR;
      }
    }
  }
  return OK;
}

static StatusTypeDef I2C_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c)
{
  if (I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
  {
    /* Clear NACKF Flag */
    I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    hi2c->PreviousState       = I2C_STATE_NONE;
    hi2c->State               = I2C_STATE_READY;
    hi2c->Mode                = I2C_MODE_NONE;
    hi2c->ErrorCode           |= I2C_ERROR_AF;

    /* Process Unlocked */
    hi2c->Lock = UNLOCKED;

    return EERROR;
  }
  return OK;
}

static StatusTypeDef I2C_WaitOnTXEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{
  while (I2C_GET_FLAG(hi2c, I2C_FLAG_TXE) == RESET)
  {
    /* Check if a NACK is detected */
    if (I2C_IsAcknowledgeFailed(hi2c) != OK)
    {
      return EERROR;
    }

    /* Check for the Timeout */
    if (Timeout != MAX_DELAY)
    {
      if (((GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {
        hi2c->PreviousState       = I2C_STATE_NONE;
        hi2c->State               = I2C_STATE_READY;
        hi2c->Mode                = I2C_MODE_NONE;
        hi2c->ErrorCode           |= I2C_ERROR_TIMEOUT;

        /* Process Unlocked */
        hi2c->Lock = UNLOCKED;

        return EERROR;
      }
    }
  }
  return OK;
}

static StatusTypeDef I2C_WaitOnRXNEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{

  while (I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == RESET)
  {
    /* Check if a STOPF is detected */
    if (I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET)
    {
      /* Clear STOP Flag */
      I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

      hi2c->PreviousState       = I2C_STATE_NONE;
      hi2c->State               = I2C_STATE_READY;
      hi2c->Mode                = I2C_MODE_NONE;
      hi2c->ErrorCode           |= I2C_ERROR_NONE;

      /* Process Unlocked */
      hi2c->Lock = UNLOCKED;

      return EERROR;
    }
    /* Check for the Timeout */
    if (((GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
    {
      hi2c->PreviousState       = I2C_STATE_NONE;
      hi2c->State               = I2C_STATE_READY;
      hi2c->Mode                = I2C_MODE_NONE;
      hi2c->ErrorCode           |= I2C_ERROR_TIMEOUT;

      /* Process Unlocked */
      hi2c->Lock = UNLOCKED;

      return EERROR;
    }
  }
  return OK;
}



static StatusTypeDef I2C_WaitOnBTFFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{
  while (I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == RESET)
  {
    /* Check if a NACK is detected */
    if (I2C_IsAcknowledgeFailed(hi2c) != OK)
    {
      return EERROR;
    }

    /* Check for the Timeout */
    if (Timeout != MAX_DELAY)
    {
      if (((GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {
        hi2c->PreviousState       = I2C_STATE_NONE;
        hi2c->State               = I2C_STATE_READY;
        hi2c->Mode                = I2C_MODE_NONE;
        hi2c->ErrorCode           |= I2C_ERROR_TIMEOUT;

        /* Process Unlocked */
        hi2c->Lock = UNLOCKED;

        return EERROR;
      }
    }
  }
  return OK;
}

void I2C_CLEAR_ADDRFLAG(I2C_HandleTypeDef *hi2c) {
		volatile uint32_t tmpreg = 0x00U;               
    tmpreg = (hi2c)->Instance->SR1;       
    tmpreg = (hi2c)->Instance->SR2;       
    UNUSED(tmpreg); 
}

static StatusTypeDef I2C_MasterRequestWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout, uint32_t Tickstart)
{
  /* Declaration of temporary variable to prevent undefined behavior of volatile usage */
  uint32_t CurrentXferOptions = hi2c->XferOptions;

  /* Generate Start condition if first transfer */
  if ((CurrentXferOptions == I2C_FIRST_AND_LAST_FRAME) || (CurrentXferOptions == I2C_FIRST_FRAME) || (CurrentXferOptions == I2C_NO_OPTION_FRAME))
  {
    /* Generate Start */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);
  }
  else if (hi2c->PreviousState == I2C_STATE_MASTER_BUSY_RX)
  {
    /* Generate ReStart */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);
  }
  else
  {
    /* Do nothing */
  }

  /* Wait until SB flag is set */
  if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != OK)
  {
    if (READ_BIT(hi2c->Instance->CR1, I2C_CR1_START) == I2C_CR1_START)
    {
      hi2c->ErrorCode = I2C_WRONG_START;
    }
    return TIMEOUT;
  }

  if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
  {
    /* Send slave address */
    hi2c->Instance->DR = I2C_7BIT_ADD_WRITE(DevAddress);
  }
  else
  {
    /* Send header of slave address */
    hi2c->Instance->DR = I2C_10BIT_HEADER_WRITE(DevAddress);

    /* Wait until ADD10 flag is set */
    if (I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADD10, Timeout, Tickstart) != OK)
    {
      return EERROR;
    }

    /* Send slave address */
    hi2c->Instance->DR = I2C_10BIT_ADDRESS(DevAddress);
  }

  /* Wait until ADDR flag is set */
  if (I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout, Tickstart) != OK)
  {
    return EERROR;
  }

  return OK;
}

static StatusTypeDef I2C_RequestMemoryRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout, uint32_t Tickstart)
{
  /* Enable Acknowledge */
  SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

  /* Generate Start */
  SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);

  /* Wait until SB flag is set */
  if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != OK)
  {
    if (READ_BIT(hi2c->Instance->CR1, I2C_CR1_START) == I2C_CR1_START)
    {
      hi2c->ErrorCode = I2C_WRONG_START;
    }
    return TIMEOUT;
  }

  /* Send slave address */
  hi2c->Instance->DR = I2C_7BIT_ADD_WRITE(DevAddress);

  /* Wait until ADDR flag is set */
  if (I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout, Tickstart) != OK)
  {
    return EERROR;
  }

  /* Clear ADDR flag */
  I2C_CLEAR_ADDRFLAG(hi2c);

  /* Wait until TXE flag is set */
  if (I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, Tickstart) != OK)
  {
    if (hi2c->ErrorCode == I2C_ERROR_AF)
    {
      /* Generate Stop */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
    }
    return EERROR;
  }

  /* If Memory address size is 8Bit */
  if (MemAddSize == I2C_MEMADD_SIZE_8BIT)
  {
    /* Send Memory Address */
    hi2c->Instance->DR = I2C_MEM_ADD_LSB(MemAddress);
  }
  /* If Memory address size is 16Bit */
  else
  {
    /* Send MSB of Memory Address */
    hi2c->Instance->DR = I2C_MEM_ADD_MSB(MemAddress);

    /* Wait until TXE flag is set */
    if (I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, Tickstart) != OK)
    {
      if (hi2c->ErrorCode == I2C_ERROR_AF)
      {
        /* Generate Stop */
        SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
      }
      return EERROR;
    }

    /* Send LSB of Memory Address */
    hi2c->Instance->DR = I2C_MEM_ADD_LSB(MemAddress);
  }

  /* Wait until TXE flag is set */
  if (I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, Tickstart) != OK)
  {
    if (hi2c->ErrorCode == I2C_ERROR_AF)
    {
      /* Generate Stop */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
    }
    return EERROR;
  }

  /* Generate Restart */
  SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);

  /* Wait until SB flag is set */
  if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != OK)
  {
    if (READ_BIT(hi2c->Instance->CR1, I2C_CR1_START) == I2C_CR1_START)
    {
      hi2c->ErrorCode = I2C_WRONG_START;
    }
    return TIMEOUT;
  }

  /* Send slave address */
  hi2c->Instance->DR = I2C_7BIT_ADD_READ(DevAddress);

  /* Wait until ADDR flag is set */
  if (I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout, Tickstart) != OK)
  {
    return EERROR;
  }

  return OK;
}

static StatusTypeDef I2C_RequestMemoryWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout, uint32_t Tickstart)
{
  /* Generate Start */
  SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);

  /* Wait until SB flag is set */
  if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != OK)
  {
    if (READ_BIT(hi2c->Instance->CR1, I2C_CR1_START) == I2C_CR1_START)
    {
      hi2c->ErrorCode = I2C_WRONG_START;
    }
    return TIMEOUT;
  }

  /* Send slave address */
  hi2c->Instance->DR = I2C_7BIT_ADD_WRITE(DevAddress);

  /* Wait until ADDR flag is set */
  if (I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout, Tickstart) != OK)
  {
    return EERROR;
  }

  /* Clear ADDR flag */
  I2C_CLEAR_ADDRFLAG(hi2c);

  /* Wait until TXE flag is set */
  if (I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, Tickstart) != OK)
  {
    if (hi2c->ErrorCode == I2C_ERROR_AF)
    {
      /* Generate Stop */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
    }
    return EERROR;
  }

  /* If Memory address size is 8Bit */
  if (MemAddSize == I2C_MEMADD_SIZE_8BIT)
  {
    /* Send Memory Address */
    hi2c->Instance->DR = I2C_MEM_ADD_LSB(MemAddress);
  }
  /* If Memory address size is 16Bit */
  else
  {
    /* Send MSB of Memory Address */
    hi2c->Instance->DR = I2C_MEM_ADD_MSB(MemAddress);

    /* Wait until TXE flag is set */
    if (I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, Tickstart) != OK)
    {
      if (hi2c->ErrorCode == I2C_ERROR_AF)
      {
        /* Generate Stop */
        SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
      }
      return EERROR;
    }

    /* Send LSB of Memory Address */
    hi2c->Instance->DR = I2C_MEM_ADD_LSB(MemAddress);
  }

  return OK;
}

StatusTypeDef I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout)
{
  /* Get tick */
  uint32_t tickstart = GetTick();
  uint32_t I2C_Trials = 1U;
  FlagStatus tmp1;
  FlagStatus tmp2;

  if (hi2c->State == I2C_STATE_READY)
  {
    /* Wait until BUSY flag is reset */
    if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != OK)
    {
      return BUSY;
    }

    /* Process Locked */
    hi2c->Lock = LOCKED;

    /* Check if the I2C is already enabled */
    if ((hi2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
    {
      /* Enable I2C peripheral */
      I2C_ENABLE(hi2c);
    }

    /* Disable Pos */
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);

    hi2c->State = I2C_STATE_BUSY;
    hi2c->ErrorCode = I2C_ERROR_NONE;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;

    do
    {
      /* Generate Start */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);

      /* Wait until SB flag is set */
      if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, tickstart) != OK)
      {
        if (READ_BIT(hi2c->Instance->CR1, I2C_CR1_START) == I2C_CR1_START)
        {
          hi2c->ErrorCode = I2C_WRONG_START;
        }
        return TIMEOUT;
      }

      /* Send slave address */
      hi2c->Instance->DR = I2C_7BIT_ADD_WRITE(DevAddress);

      /* Wait until ADDR or AF flag are set */
      /* Get tick */
      tickstart = GetTick();

      tmp1 = I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR);
      tmp2 = I2C_GET_FLAG(hi2c, I2C_FLAG_AF);
      while ((hi2c->State != I2C_STATE_TIMEOUT) && (tmp1 == RESET) && (tmp2 == RESET))
      {
        if (((GetTick() - tickstart) > Timeout) || (Timeout == 0U))
        {
          hi2c->State = I2C_STATE_TIMEOUT;
        }
        tmp1 = I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR);
        tmp2 = I2C_GET_FLAG(hi2c, I2C_FLAG_AF);
      }

      hi2c->State = I2C_STATE_READY;

      /* Check if the ADDR flag has been set */
      if (I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR) == SET)
      {
        /* Generate Stop */
        SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

        /* Clear ADDR Flag */
        I2C_CLEAR_ADDRFLAG(hi2c);

        /* Wait until BUSY flag is reset */
        if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != OK)
        {
          return EERROR;
        }

        hi2c->State = I2C_STATE_READY;

        /* Process Unlocked */
        hi2c->Lock = UNLOCKED;

        return OK;
      }
      else
      {
        /* Generate Stop */
        SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

        /* Clear AF Flag */
        I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

        /* Wait until BUSY flag is reset */
        if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != OK)
        {
          return EERROR;
        }
      }

      /* Increment Trials */
      I2C_Trials++;
    }
    while (I2C_Trials < Trials);

    hi2c->State = I2C_STATE_READY;

    /* Process Unlocked */
    hi2c->Lock = UNLOCKED;

    return EERROR;
  }
  else
  {
    return BUSY;
  }
}

//I2C_HandleTypeDef hi2c1;
/* I2C1 init function */
void I2C_Init(I2C_HandleTypeDef *hi2c)
{
  uint32_t freqrange;
  uint32_t pclk1;
	
	I2C_MspInit(hi2c);
  hi2c->State = I2C_STATE_BUSY;

  /* Disable the selected I2C peripheral */
  I2C_DISABLE(hi2c);

  /*Reset I2C*/
  hi2c->Instance->CR1 |= I2C_CR1_SWRST;
  hi2c->Instance->CR1 &= ~I2C_CR1_SWRST;

  /* Get PCLK1 frequency */
  pclk1 = RCC_GetPCLK1Freq();

  /* Calculate frequency range */
  freqrange = I2C_FREQRANGE(pclk1);

  /*---------------------------- I2Cx CR2 Configuration ----------------------*/
  /* Configure I2Cx: Frequency range */
  MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_FREQ, freqrange);

  /*---------------------------- I2Cx TRISE Configuration --------------------*/
  /* Configure I2Cx: Rise Time */
  MODIFY_REG(hi2c->Instance->TRISE, I2C_TRISE_TRISE, I2C_RISE_TIME(freqrange, hi2c->Init.ClockSpeed));

  /*---------------------------- I2Cx CCR Configuration ----------------------*/
  /* Configure I2Cx: Speed */
  MODIFY_REG(hi2c->Instance->CCR, (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR), I2C_SPEED(pclk1, hi2c->Init.ClockSpeed, hi2c->Init.DutyCycle));

  /*---------------------------- I2Cx CR1 Configuration ----------------------*/
  /* Configure I2Cx: Generalcall and NoStretch mode */
  MODIFY_REG(hi2c->Instance->CR1, (I2C_CR1_ENGC | I2C_CR1_NOSTRETCH), (hi2c->Init.GeneralCallMode | hi2c->Init.NoStretchMode));

  /*---------------------------- I2Cx OAR1 Configuration ---------------------*/
  /* Configure I2Cx: Own Address1 and addressing mode */
  MODIFY_REG(hi2c->Instance->OAR1, (I2C_OAR1_ADDMODE | I2C_OAR1_ADD8_9 | I2C_OAR1_ADD1_7 | I2C_OAR1_ADD0), (hi2c->Init.AddressingMode | hi2c->Init.OwnAddress1));

  /*---------------------------- I2Cx OAR2 Configuration ---------------------*/
  /* Configure I2Cx: Dual mode and Own Address2 */
  MODIFY_REG(hi2c->Instance->OAR2, (I2C_OAR2_ENDUAL | I2C_OAR2_ADD2), (hi2c->Init.DualAddressMode | hi2c->Init.OwnAddress2));

  /* Enable the selected I2C peripheral */
  I2C_ENABLE(hi2c);

  hi2c->ErrorCode = 0x00000000U;
  hi2c->State = I2C_STATE_READY;
  hi2c->PreviousState = 0x00000000U;
	hi2c->Mode = I2C_MODE_NONE;
}

StatusTypeDef I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  __IO uint32_t count = 0U;

  /* Init tickstart for timeout management*/
  uint32_t tickstart = GetTick();
  if (hi2c->State == I2C_STATE_READY)
  {
		/* Wait until BUSY flag is reset */
    if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != OK)
    {
      return BUSY;
    }
    /* Process Locked */
    hi2c->Lock = LOCKED;

    /* Check if the I2C is already enabled */
    if ((hi2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
    {
      /* Enable I2C peripheral */
      SET_BIT((hi2c)->Instance->CR1, I2C_CR1_PE);
    }

    /* Disable Pos */
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);

    hi2c->State     = I2C_STATE_BUSY_RX;
    hi2c->Mode      = I2C_MODE_MEM;
    hi2c->ErrorCode = 0x00000000U;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferSize    = hi2c->XferCount;
    hi2c->XferOptions = 0xFFFF0000U;

    /* Send Slave Address and Memory Address */
    if (I2C_RequestMemoryRead(hi2c, DevAddress, MemAddress, MemAddSize, Timeout, tickstart) != OK)
    {
      return EERROR;
    }

    if (hi2c->XferSize == 0U)
    {
      /* Clear ADDR flag */
      I2C_CLEAR_ADDRFLAG(hi2c);

      /* Generate Stop */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
    }
    else if (hi2c->XferSize == 1U)
    {
      /* Disable Acknowledge */
      CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

      /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
         software sequence must complete before the current byte end of transfer */
      __disable_irq();

      /* Clear ADDR flag */
      I2C_CLEAR_ADDRFLAG(hi2c);

      /* Generate Stop */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

      /* Re-enable IRQs */
      __enable_irq();
    }
    else if (hi2c->XferSize == 2U)
    {
      /* Enable Pos */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_POS);

      /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
         software sequence must complete before the current byte end of transfer */
      __disable_irq();

      /* Clear ADDR flag */
      I2C_CLEAR_ADDRFLAG(hi2c);

      /* Disable Acknowledge */
      CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

      /* Re-enable IRQs */
      __enable_irq();
    }
    else
    {
      /* Enable Acknowledge */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);
      /* Clear ADDR flag */
      I2C_CLEAR_ADDRFLAG(hi2c);
    }

    while (hi2c->XferSize > 0U)
    {
      if (hi2c->XferSize <= 3U)
      {
        /* One byte */
        if (hi2c->XferSize == 1U)
        {
          /* Wait until RXNE flag is set */
          if (I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart) != OK)
          {
            return EERROR;
          }

          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;
        }
        /* Two bytes */
        else if (hi2c->XferSize == 2U)
        {
          /* Wait until BTF flag is set */
          if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout, tickstart) != OK)
          {
            return EERROR;
          }

          /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
             software sequence must complete before the current byte end of transfer */
          __disable_irq();

          /* Generate Stop */
          SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;

          /* Re-enable IRQs */
          __enable_irq();

          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;
        }
        /* 3 Last bytes */
        else
        {
          /* Wait until BTF flag is set */
          if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout, tickstart) != OK)
          {
            return EERROR;
          }

          /* Disable Acknowledge */
          CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

          /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
             software sequence must complete before the current byte end of transfer */
          __disable_irq();

          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;

          /* Wait until BTF flag is set */
          count = I2C_TIMEOUT_FLAG * (SystemCoreClock / 25U / 1000U);
          do
          {
            count--;
            if (count == 0U)
            {
              hi2c->PreviousState       = I2C_STATE_NONE;
              hi2c->State               = I2C_STATE_READY;
              hi2c->Mode                = I2C_MODE_NONE;
              hi2c->ErrorCode           |= I2C_ERROR_TIMEOUT;

              /* Re-enable IRQs */
              __enable_irq();

              /* Process Unlocked */
              hi2c->Lock = UNLOCKED;

              return EERROR;
            }
          }
          while (I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == RESET);

          /* Generate Stop */
          SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;

          /* Re-enable IRQs */
          __enable_irq();

          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;
        }
      }
      else
      {
        /* Wait until RXNE flag is set */
        if (I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart) != OK)
        {
          return EERROR;
        }

        /* Read data from DR */
        *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

        /* Increment Buffer pointer */
        hi2c->pBuffPtr++;

        /* Update counter */
        hi2c->XferSize--;
        hi2c->XferCount--;

        if (I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET)
        {
          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;
        }
      }
    }

    hi2c->State = I2C_STATE_READY;
    //hi2c->Mode = I2C_MODE_NONE;

    /* Process Unlocked */
    hi2c->Lock = UNLOCKED;
		return OK;
  }
	else
  {
    return BUSY;
  }
}

StatusTypeDef I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  /* Init tickstart for timeout management*/
  uint32_t tickstart = GetTick();


  if (hi2c->State == I2C_STATE_READY)
  {
    /* Wait until BUSY flag is reset */
    if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != OK)
    {
      return BUSY;
    }

    /* Process Locked */
    hi2c->Lock = LOCKED;

    /* Check if the I2C is already enabled */
    if ((hi2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
    {
      /* Enable I2C peripheral */
      I2C_ENABLE(hi2c);
    }

    /* Disable Pos */
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);

    hi2c->State     = I2C_STATE_BUSY_TX;
    hi2c->Mode      = I2C_MODE_MEM;
    hi2c->ErrorCode = I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferSize    = hi2c->XferCount;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;

    /* Send Slave Address and Memory Address */
    if (I2C_RequestMemoryWrite(hi2c, DevAddress, MemAddress, MemAddSize, Timeout, tickstart) != OK)
    {
      return EERROR;
    }

    while (hi2c->XferSize > 0U)
    {
      /* Wait until TXE flag is set */
      if (I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, tickstart) != OK)
      {
        if (hi2c->ErrorCode == I2C_ERROR_AF)
        {
          /* Generate Stop */
          SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
        }
        return EERROR;
      }

      /* Write data to DR */
      hi2c->Instance->DR = *hi2c->pBuffPtr;

      /* Increment Buffer pointer */
      hi2c->pBuffPtr++;

      /* Update counter */
      hi2c->XferSize--;
      hi2c->XferCount--;

      if ((I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET) && (hi2c->XferSize != 0U))
      {
        /* Write data to DR */
        hi2c->Instance->DR = *hi2c->pBuffPtr;

        /* Increment Buffer pointer */
        hi2c->pBuffPtr++;

        /* Update counter */
        hi2c->XferSize--;
        hi2c->XferCount--;
      }
    }

    /* Wait until BTF flag is set */
    if (I2C_WaitOnBTFFlagUntilTimeout(hi2c, Timeout, tickstart) != OK)
    {
      if (hi2c->ErrorCode == I2C_ERROR_AF)
      {
        /* Generate Stop */
        SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
      }
      return EERROR;
    }

    /* Generate Stop */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

    hi2c->State = I2C_STATE_READY;
    hi2c->Mode = I2C_MODE_NONE;

    /* Process Unlocked */
    hi2c->Lock = UNLOCKED;
		return OK;
	} else {
		return BUSY;
	}
}

StatusTypeDef I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  /* Init tickstart for timeout management*/
  uint32_t tickstart = GetTick();

  if (hi2c->State == I2C_STATE_READY)
  {
    /* Wait until BUSY flag is reset */
    if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != OK)
    {
      return BUSY;
    }

    /* Process Locked */
    hi2c->Lock = LOCKED;

    /* Check if the I2C is already enabled */
    if ((hi2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
    {
      /* Enable I2C peripheral */
      I2C_ENABLE(hi2c);
    }

    /* Disable Pos */
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);

    hi2c->State       = I2C_STATE_BUSY_TX;
    hi2c->Mode        = I2C_MODE_MASTER;
    hi2c->ErrorCode   = I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = pData;
    hi2c->XferCount   = Size;
    hi2c->XferSize    = hi2c->XferCount;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;

    /* Send Slave Address */
    if (I2C_MasterRequestWrite(hi2c, DevAddress, Timeout, tickstart) != OK)
    {
      return EERROR;
    }

    /* Clear ADDR flag */
    I2C_CLEAR_ADDRFLAG(hi2c);

    while (hi2c->XferSize > 0U)
    {
      /* Wait until TXE flag is set */
      if (I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, tickstart) != OK)
      {
        if (hi2c->ErrorCode == I2C_ERROR_AF)
        {
          /* Generate Stop */
          SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
        }
        return EERROR;
      }

      /* Write data to DR */
      hi2c->Instance->DR = *hi2c->pBuffPtr;

      /* Increment Buffer pointer */
      hi2c->pBuffPtr++;

      /* Update counter */
      hi2c->XferCount--;
      hi2c->XferSize--;

      if ((I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET) && (hi2c->XferSize != 0U))
      {
        /* Write data to DR */
        hi2c->Instance->DR = *hi2c->pBuffPtr;

        /* Increment Buffer pointer */
        hi2c->pBuffPtr++;

        /* Update counter */
        hi2c->XferCount--;
        hi2c->XferSize--;
      }

      /* Wait until BTF flag is set */
      if (I2C_WaitOnBTFFlagUntilTimeout(hi2c, Timeout, tickstart) != OK)
      {
        if (hi2c->ErrorCode == I2C_ERROR_AF)
        {
          /* Generate Stop */
          SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
        }
        return EERROR;
      }
    }

    /* Generate Stop */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

    hi2c->State = I2C_STATE_READY;
    hi2c->Mode = I2C_MODE_NONE;

    /* Process Unlocked */
    hi2c->Lock=UNLOCKED;

    return OK;
  }
  else
  {
    return BUSY;
  }
}
