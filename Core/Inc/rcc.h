#ifndef RCC_H
#define RCC_H

#include "stm32f1xx.h"
#include "stm32f103c8t6.h"
                             
#define RCC_OSCILLATORTYPE_NONE            0x00000000U
#define RCC_OSCILLATORTYPE_HSE             0x00000001U
#define RCC_OSCILLATORTYPE_HSI             0x00000002U
#define RCC_OSCILLATORTYPE_LSE             0x00000004U
#define RCC_OSCILLATORTYPE_LSI             0x00000008U

#define RCC_CR_PLLRDY_Pos                    (25U) 
#define RCC_CR_HSERDY_Pos                    (17U)   
#define RCC_CR_HSITRIM_Pos                   (3U) 
#define RCC_CR_HSIRDY_Pos                    (1U)                              
#define RCC_CR_HSION_Pos                     (0U) 
#define RCC_CSR_LSION_Pos                    (0U) 
#define RCC_CSR_LSIRDY_Pos                   (1U)  
#define RCC_CR_PLLON_Pos                     (24U)                             
#define RCC_CFGR_SWS_Pos                     (2U)  
#define RCC_CFGR_PLLMULL_Pos                 (18U)                             
#define RCC_CFGR_PLLXTPRE_Pos                (17U)                             
#define RCC_CFGR_HPRE_Pos                    (4U)                              
#define RCC_CFGR_PPRE1_Pos                   (8U) 
#define AFIO_MAPR_SWJ_CFG_Pos                (24U)   

#define RCC_FLAG_MASK                    ((uint8_t)0x1F)
#define RCC_FLAG_LSIRDY                  ((uint8_t)((CSR_REG_INDEX << 5U) | RCC_CSR_LSIRDY_Pos))   /*!< Internal Low Speed oscillator Ready */

#define RCC_HSI_OFF                      0x00000000U                      /*!< HSI clock deactivation */
#define RCC_HSI_ON                       RCC_CR_HSION  

#define RCC_HSE_OFF                      0x00000000U                                /*!< HSE clock deactivation */
#define RCC_HSE_ON                       RCC_CR_HSEON                               /*!< HSE clock activation */
#define RCC_HSE_BYPASS                   ((uint32_t)(RCC_CR_HSEBYP | RCC_CR_HSEON)) /*!< External clock source for HSE clock */

#define RCC_PLL_NONE                      0x00000000U  /*!< PLL is not configured */
#define RCC_PLL_OFF                       0x00000001U  /*!< PLL deactivation */
#define RCC_PLL_ON                        0x00000002U  /*!< PLL activation */

#define RCC_HSICALIBRATION_DEFAULT       0x10U         /* Default HSI calibration trimming value */

#define RCC_CLOCKTYPE_SYSCLK             0x00000001U /*!< SYSCLK to configure */
#define RCC_CLOCKTYPE_HCLK               0x00000002U /*!< HCLK to configure */
#define RCC_CLOCKTYPE_PCLK1              0x00000004U /*!< PCLK1 to configure */
#define RCC_CLOCKTYPE_PCLK2              0x00000008U /*!< PCLK2 to configure */

#define RCC_SYSCLKSOURCE_HSI             RCC_CFGR_SW_HSI /*!< HSI selected as system clock */

#define FLASH_GET_LATENCY()     (READ_BIT((FLASH->ACR), FLASH_ACR_LATENCY))
#define FLASH_SET_LATENCY(__LATENCY__)    (FLASH->ACR = (FLASH->ACR&(~FLASH_ACR_LATENCY)) | (__LATENCY__))
#define RCC_GET_SYSCLK_SOURCE() ((uint32_t)(READ_BIT(RCC->CFGR,RCC_CFGR_SWS)))

#define RCC_SYSCLKSOURCE_STATUS_HSI      RCC_CFGR_SWS_HSI            /*!< HSI used as system clock */
#define RCC_SYSCLKSOURCE_STATUS_HSE      RCC_CFGR_SWS_HSE            /*!< HSE used as system clock */
#define RCC_SYSCLKSOURCE_STATUS_PLLCLK   RCC_CFGR_SWS_PLL            /*!< PLL used as system clock */

#define RCC_SYSCLK_CONFIG(__SYSCLKSOURCE__) \
                  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, (__SYSCLKSOURCE__))
									
/* Alias word address of PLLON bit */
#define RCC_PLLON_BIT_NUMBER      RCC_CR_PLLON_Pos
#define RCC_CR_PLLON_BB           ((uint32_t)(PERIPH_BB_BASE + (RCC_CR_OFFSET_BB * 32U) + (RCC_PLLON_BIT_NUMBER * 4U)))
#define RCC_PLL_DISABLE()         (*(__IO uint32_t *) RCC_CR_PLLON_BB = DISABLE)


#define RCC_FLAG_HSERDY                  ((uint8_t)((CR_REG_INDEX << 5U) | RCC_CR_HSERDY_Pos)) /*!< External High Speed clock ready flag */
#define RCC_FLAG_HSIRDY                  ((uint8_t)((CR_REG_INDEX << 5U) | RCC_CR_HSIRDY_Pos)) /*!< Internal High Speed clock ready flag */
#define RCC_FLAG_PLLRDY                  ((uint8_t)((CR_REG_INDEX << 5U) | RCC_CR_PLLRDY_Pos)) /*!< PLL clock ready flag */

#define CR_REG_INDEX                     ((uint8_t)1)
#define BDCR_REG_INDEX                   ((uint8_t)2)
#define CSR_REG_INDEX                    ((uint8_t)3)

#define RCC_CFGR_PLLSRC_Pos                  (16U)                             
#define RCC_CFGR_PLLSRC_Msk                  (0x1UL << RCC_CFGR_PLLSRC_Pos)     /*!< 0x00010000 */

#define RCC_PLLSOURCE_HSE           RCC_CFGR_PLLSRC_Msk            /*!< HSE clock selected as PLL entry clock source */
#define HSE_VALUE    ((uint32_t)8000000)
#define HSI_VALUE    ((uint32_t)8000000) /*!< Value of the Internal oscillator in Hz*/

#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)
#define RCC_CR_OFFSET             0x00U
#define RCC_CR_OFFSET_BB          (RCC_OFFSET + RCC_CR_OFFSET)

#define RCC_HSION_BIT_NUMBER      RCC_CR_HSION_Pos
#define RCC_CR_HSION_BB           ((uint32_t)(PERIPH_BB_BASE + (RCC_CR_OFFSET_BB * 32U) + (RCC_HSION_BIT_NUMBER * 4U)))

#define RCC_CSR_OFFSET_BB         (RCC_OFFSET + 0x24U)
#define RCC_LSION_BIT_NUMBER      RCC_CSR_LSION_Pos
#define RCC_CSR_LSION_BB          ((uint32_t)(PERIPH_BB_BASE + (RCC_CSR_OFFSET_BB * 32U) + (RCC_LSION_BIT_NUMBER * 4U)))

#define RCC_LSE_OFF                      0x00000000U                                    /*!< LSE clock deactivation */
#define RCC_LSE_ON                       RCC_BDCR_LSEON                                 /*!< LSE clock activation */
#define RCC_LSE_BYPASS                   ((uint32_t)(RCC_BDCR_LSEBYP | RCC_BDCR_LSEON)) /*!< External clock source for LSE clock */

#define RCC_FLAG_LSERDY                  ((uint8_t)((BDCR_REG_INDEX << 5U) | 1U)) /*!< External Low Speed oscillator Ready */

#define RCC_PWR_CLK_DISABLE()       (RCC->APB1ENR &= ~(RCC_APB1ENR_PWREN))

#define RCC_GET_FLAG(__FLAG__) (((((__FLAG__) >> 5U) == CR_REG_INDEX)?   RCC->CR   : \
                                      ((((__FLAG__) >> 5U) == BDCR_REG_INDEX)? RCC->BDCR : \
                                                                              RCC->CSR)) & (1U << ((__FLAG__) & RCC_FLAG_MASK)))
#define RCC_HSI_CALIBRATIONVALUE_ADJUST(_HSICALIBRATIONVALUE_) \
          (MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, (uint32_t)(_HSICALIBRATIONVALUE_) << RCC_CR_HSITRIM_Pos))

#define RCC_GET_PLL_OSCSOURCE() ((uint32_t)(READ_BIT(RCC->CFGR, RCC_CFGR_PLLSRC)))

#define RCC_HSI_ENABLE()  (*(__IO uint32_t *) RCC_CR_HSION_BB = ENABLE)
#define RCC_HSI_DISABLE() (*(__IO uint32_t *) RCC_CR_HSION_BB = DISABLE)

#define RCC_LSI_ENABLE()  (*(__IO uint32_t *) RCC_CSR_LSION_BB = ENABLE)
#define RCC_LSI_DISABLE() (*(__IO uint32_t *) RCC_CSR_LSION_BB = DISABLE)

#define RCC_PWR_IS_CLK_DISABLED()       ((RCC->APB1ENR & (RCC_APB1ENR_PWREN)) == RESET)

#define RCC_PWR_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);\
                                        /* Delay after an RCC peripheral clock enabling */\
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);\
                                        UNUSED(tmpreg); \
                                      } while(0U)

#define RCC_FLAG_PLLRDY                  ((uint8_t)((CR_REG_INDEX << 5U) | RCC_CR_PLLRDY_Pos)) /*!< PLL clock ready flag */


#define RCC_PLL_ENABLE()          (*(__IO uint32_t *) RCC_CR_PLLON_BB = ENABLE)
#define RCC_PLL_CONFIG(__RCC_PLLSOURCE__, __PLLMUL__)\
          MODIFY_REG(RCC->CFGR, (RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL),((__RCC_PLLSOURCE__) | (__PLLMUL__) ))

#define RCC_HSE_PREDIV_CONFIG(__HSE_PREDIV_VALUE__) \
                  MODIFY_REG(RCC->CFGR,RCC_CFGR_PLLXTPRE, (uint32_t)(__HSE_PREDIV_VALUE__))

#define AFIO_MAPR_SWJ_CFG_Msk                (0x7UL << AFIO_MAPR_SWJ_CFG_Pos)   /*!< 0x07000000 */

#define AFIO_DBGAFR_CONFIG(DBGAFR_SWJCFG)  do{ uint32_t tmpreg = AFIO->MAPR;     \
                                               tmpreg &= ~AFIO_MAPR_SWJ_CFG_Msk; \
                                               tmpreg |= DBGAFR_SWJCFG;          \
                                               AFIO->MAPR = tmpreg;              \
                                               }while(0u)																			
void RCC_GPIOA_CLK_ENABLE(void);
void RCC_GPIOB_CLK_ENABLE(void);
void RCC_GPIOD_CLK_ENABLE(void);
void RCC_TIM2_CLK_ENABLE(void);
void RCC_AFIO_CLK_ENABLE(void);
void RCC_I2C1_CLK_ENABLE(void);
void AFIO_REMAP_SWJ_NOJTAG();
uint32_t RCC_GetHCLKFreq(void);
uint32_t RCC_GetPCLK1Freq(void);

typedef struct
{
  uint32_t PLLState;      
  uint32_t PLLSource;     
  uint32_t PLLMUL;  
} RCC_PLLInitTypeDef;

typedef struct
{
  uint32_t OscillatorType;
  uint32_t HSEState;
  uint32_t HSEPredivValue;
  uint32_t LSEState; 
  uint32_t HSIState;              
  uint32_t HSICalibrationValue;
  uint32_t LSIState;        
  RCC_PLLInitTypeDef PLL;
} RCC_OscInitTypeDef;

typedef struct
{
  uint32_t ClockType;
  uint32_t SYSCLKSource;
  uint32_t AHBCLKDivider;
  uint32_t APB1CLKDivider;
  uint32_t APB2CLKDivider;            
} RCC_ClkInitTypeDef;

StatusTypeDef RCC_OscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct);
StatusTypeDef RCC_ClockConfig(RCC_ClkInitTypeDef  *RCC_ClkInitStruct, uint32_t FLatency);

#endif /* RCC_H */