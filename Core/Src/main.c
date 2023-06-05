#include "main.h"
#include "fonts.h"
#include "ssd1306.h"
#include "math.h"
#include "mpu6050.h"
#include <filter.h>
#include "string.h"
#include "stdbool.h"
#include <stdio.h>
#include <stdint.h>
#include "stm32f1xx.h"

#define SAMPLE_RATE 15

I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;

char str[16];

MPU6050_t MPU6050;

double magnitude[SAMPLE_RATE];
double dynamic_thres;
double x_final[SAMPLE_RATE], y_final[SAMPLE_RATE], z_final[SAMPLE_RATE];
		
int steps, step_run, step_walk ;
double threshold;
bool run = false;
bool walk = false;
bool stand = false;
bool check = false;
bool flag, ready;

double mag_mean;
double mag_sample_var;
double mag_std_dev;

KalmanFilter Kal_accX, Kal_accY, Kal_accZ;

typedef enum {
  RUNNING,
  PAUSED
} fsm_state_t;

void SystemClock_Config(void);
static void GPIO_INIT(void);
static void I2C_INIT(void);
static void TIM2_INIT(void);

void ledWrite(int cases) {
	if (cases == 0) { //Stay
		GPIO_WritePin(Led_GREEN_GPIO_Port, Led_GREEN_Pin, 0);
		GPIO_WritePin(Led_YELLOW_GPIO_Port, Led_YELLOW_Pin, 0);
		GPIO_WritePin(Led_RED_GPIO_Port, Led_RED_Pin, 1);
	} else if(cases == 1) {
		GPIO_WritePin(Led_GREEN_GPIO_Port, Led_GREEN_Pin, 0);
		GPIO_WritePin(Led_YELLOW_GPIO_Port, Led_YELLOW_Pin, 1);
		GPIO_WritePin(Led_RED_GPIO_Port, Led_RED_Pin, 0);
	} else {
		GPIO_WritePin(Led_GREEN_GPIO_Port, Led_GREEN_Pin, 1);
		GPIO_WritePin(Led_YELLOW_GPIO_Port, Led_YELLOW_Pin, 0);
		GPIO_WritePin(Led_RED_GPIO_Port, Led_RED_Pin, 0);
	}
}

void led_write(int r, int y, int g) {
	GPIO_WritePin(Led_GREEN_GPIO_Port, Led_GREEN_Pin, g);
	GPIO_WritePin(Led_YELLOW_GPIO_Port, Led_YELLOW_Pin, y);
	GPIO_WritePin(Led_RED_GPIO_Port, Led_RED_Pin, r);
}
double preProcessing(double x, double y, double z) {
  double m = sqrt(x*x + y*y + z*z);
  return m;
}

double CalculateMean(int k, double value[]) {
	double sum = 0;
	for(int i = 0; i < k; i++) {
		sum += value[i];
	}
	return (sum / k);
}

double CalculateSampleVariane(int k, double value[], double mean) {
	mean = CalculateMean(k, value);
	double temp = 0;
	for(int i = 0; i < k; i++) {
			 temp += (value[i] - mean) * (value[i] - mean) ;
	}
	return temp / (k-1);
}

double GetSampleStandardDeviation(int k, double value[], double mean) {
	return sqrt(CalculateSampleVariane(k, value, mean));
}


int main(void) {
  Init();
  SystemClock_Config();
  GPIO_INIT();
  I2C_INIT();
	TIM2_INIT();
	SSD1306_Init();
	MPU6050_Init(&hi2c1);
	
	KalmanFilter_init(&Kal_accX, 2, 2, 0.001);
	KalmanFilter_init(&Kal_accY, 2, 2, 0.001);
	KalmanFilter_init(&Kal_accZ, 2, 2, 0.001);
	
  SSD1306_GotoXY(15,0);
  SSD1306_Puts("STEP COUNTER", &Font_7x10, 1);
  SSD1306_GotoXY(15, 30);
  SSD1306_Puts("PEDOMETER", &Font_11x18, 1);
  SSD1306_UpdateScreen();
  Delay(500);
  SSD1306_Clear();
	
	for(int i = 0; i < SAMPLE_RATE; i++) {
		MPU6050_Read_Accel(&hi2c1, &MPU6050);
		x_final[i] = KalmanFilter_updateEstimate(&Kal_accX, fabs(MPU6050.Ax));
		y_final[i] = KalmanFilter_updateEstimate(&Kal_accY, fabs(MPU6050.Ay));
		z_final[i] = KalmanFilter_updateEstimate(&Kal_accZ, fabs(MPU6050.Az));
		SSD1306_GotoXY(10,30);
		sprintf(str, "Loading = %d...",i*100/SAMPLE_RATE);
		SSD1306_Puts(str, &Font_7x10, 1);
		SSD1306_UpdateScreen();
		led_write(0,1,0);
		Delay(20);
	}
	
	led_write(0,0,1);
	Delay(50);
	led_write(0,1,0);
	Delay(50);
	led_write(1,0,0);
	Delay(50);
	led_write(0,1,0);
	Delay(50);
	led_write(0,0,1);
	SSD1306_Clear();	
	flag = true;
	fsm_state_t state = RUNNING;
  while (1) {
		double x[SAMPLE_RATE];
		int pos;
		threshold = 3;
		for(int i = 0; i < SAMPLE_RATE; i++) {
			MPU6050_Read_Accel(&hi2c1, &MPU6050);
			x[i] = fabs(MPU6050.Ax);
			x_final[i] = KalmanFilter_updateEstimate(&Kal_accX, fabs(MPU6050.Ax));
			y_final[i] = KalmanFilter_updateEstimate(&Kal_accY, fabs(MPU6050.Ay));
			z_final[i] = KalmanFilter_updateEstimate(&Kal_accZ, fabs(MPU6050.Az));
			//Caculate a_all
			magnitude[i] = preProcessing(x_final[i], y_final[i], z_final[i]);
			Delay(2);
		}
		mag_mean = CalculateMean(SAMPLE_RATE, magnitude);
		mag_std_dev = GetSampleStandardDeviation(SAMPLE_RATE, magnitude, mag_mean);
		dynamic_thres = threshold * mag_std_dev + 9;			
		
		if (GPIO_ReadPin(Pause_GPIO_Port,Pause_Pin) == 0) {
			while(GPIO_ReadPin(Pause_GPIO_Port,Pause_Pin) == 0){
				led_write(1,1,1);
			}
			Delay(100);
			led_write(0,0,0);
			switch (state) {
			case RUNNING:
				state = PAUSED;
				break;
			case PAUSED:
				state = RUNNING;
				break;
      }
    }
    switch (state) {
      case RUNNING:
				TIM_Base_Start_IT(&htim2);
				SSD1306_GotoXY (5, 5);
				SSD1306_Puts("State:    Running", &Font_7x10, 1);
				for (int i = 0;i < SAMPLE_RATE; i++) {
					if((magnitude[i] > magnitude[i+1]) && (magnitude[i] > magnitude[i+1])) {
						if((fabs(magnitude[i] - mag_mean) > 1.13 * dynamic_thres) && (flag == true)) {
							step_run++;
							steps++;
							flag = false;
							run = true;
							pos = i;
							break;
						} else if ((fabs(magnitude[i] - mag_mean) > dynamic_thres) && (fabs(magnitude[i] - mag_mean) < 1.13 * dynamic_thres) && (flag == true)) {
							step_walk++;
							steps++;
							flag = false;
							walk = true;
							pos = i;
							break;
						}
					}
				}
				for (int i = pos;i < SAMPLE_RATE; i++) {
					if (fabs(magnitude[i] - mag_mean) < dynamic_thres && (flag == false)) {
						flag = true;
						break;
					}
				}
				SSD1306_UpdateScreen();
        break;
      case PAUSED:
				TIM_Base_Stop_IT(&htim2);
				led_write(1,0,0);
        SSD1306_GotoXY (5, 5);
        SSD1306_Puts("State:    Paused ", &Font_7x10, 1);
				SSD1306_UpdateScreen();
        break;
    }
		
		if (GPIO_ReadPin(Button_GPIO_Port,Button_Pin) == 0) {
			while(GPIO_ReadPin(Button_GPIO_Port,Button_Pin) == 0){}
			led_write(1,0,0);
			Delay(100);
			led_write(0,1,0);
			Delay(100);
			led_write(0,0,1);
			Delay(100);
			led_write(0,0,0);
			steps = 0;
			step_run = 0;
			step_walk = 0;
			SSD1306_Clear();
		}
		SSD1306_GotoXY(10,20);
		sprintf(str, "WALK = %d", step_walk);
		SSD1306_Puts(str, &Font_7x10, 1);
		SSD1306_GotoXY(10,30);
		sprintf(str, "RUN  = %d", step_run);
		SSD1306_Puts(str, &Font_7x10, 1);
		SSD1306_GotoXY(10,40);
		sprintf(str, "STEP = %d", steps);
		SSD1306_Puts(str, &Font_11x18, 1);
	
		SSD1306_UpdateScreen();
		if (run == true || walk == true){
			Delay(200);
			run = false;
			walk = false;
		}
	}
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = 0x00000000U;
  if (RCC_OscConfig(&RCC_OscInitStruct) != OK)
  {
    Error_Handler();
  }


  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_CFGR_HPRE_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_CFGR_PPRE1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_CFGR_PPRE1_DIV1;

  if (RCC_ClockConfig(&RCC_ClkInitStruct, 0x00000000U) != OK)
  {
    Error_Handler();
  }
}

static void I2C_INIT(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = 0x00000000U;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = 0x00004000U;
  hi2c1.Init.DualAddressMode = 0x00000000U;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = 0x00000000U;
  hi2c1.Init.NoStretchMode = 0x00000000U;
  I2C_Init(&hi2c1);
}

static void TIM2_INIT(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 8000;
  htim2.Init.CounterMode = 0x00000000U;
  htim2.Init.Period = 499;
  htim2.Init.ClockDivision = 0x00000000U;
  htim2.Init.AutoReloadPreload = 0x00000000U;
  TIM_Base_Init(&htim2);
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
}

static void GPIO_INIT(void)
{
  GPIO_InitStruct_t GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  RCC_GPIOD_CLK_ENABLE();
  RCC_GPIOA_CLK_ENABLE();
  RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  GPIO_WritePin(GPIOA, Led_GREEN_Pin|Led_YELLOW_Pin|Led_RED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : Button_Pin Pause_Pin */
  GPIO_InitStruct.Pin = Button_Pin|Pause_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Led_GREEN_Pin Led_YELLOW_Pin Led_RED_Pin */
  GPIO_InitStruct.Pin = Led_GREEN_Pin|Led_YELLOW_Pin|Led_RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
}


void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
