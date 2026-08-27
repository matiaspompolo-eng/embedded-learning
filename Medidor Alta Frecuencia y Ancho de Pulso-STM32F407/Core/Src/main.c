/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
DMA_HandleTypeDef hdma_tim2_ch2_ch4;
DMA_HandleTypeDef hdma_tim2_ch1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define TIMCLOCK   84000000
#define PSCALAR    84

#define numval   100

#define MIN(a,b) ((a) < (b) ? (a) : (b))

int riseCaptured = 0;
int fallCaptured = 0;

uint32_t riseData[numval];
uint32_t fallData[numval];

float frequency = 0;
float width = 0;

int isMeasured = 0;
float promFr=0;
float promAn=0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	// If the Interrupt is triggered by 1st Channel
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		riseCaptured = 1;
	}

	// If the Interrupt is triggered by 2nd Channel
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		fallCaptured = 1;
	}


	/* Rest of the calculations will be done,
	 * once both the DMAs have finished capturing enough data */
	if ((riseCaptured) && (fallCaptured))
	{

		// calculate the reference clock
		float refClock = TIMCLOCK/(PSCALAR);

		int indxr = 0;
		int indxf = 0;

		int countr = 0;
		int countrf = 0;

		float riseavg = 0;
		float rfavg = 0;

		/* In case of high Frequencies, the DMA sometimes captures 0's in the beginning.
		 * increment the index until some useful data shows up
		 */
		while (riseData[indxr] == 0) indxr++;

		/* Again at very high frequencies, sometimes the values don't change
		 * So we will wait for the update among the values
		 */
		while ((MIN((riseData[indxr+1]-riseData[indxr]), (riseData[indxr+2]-riseData[indxr+1]))) == 0) indxr++;

		/* riseavg is the difference in the 2 consecutive rise Time */

		/* Assign a start value to riseavg */
		riseavg += MIN((riseData[indxr+1]-riseData[indxr]), (riseData[indxr+2]-riseData[indxr+1]));
		indxr++;
		countr++;

		/* start adding the values to the riseavg */
		while (indxr < (numval))
		{
			riseavg += MIN((riseData[indxr+1]-riseData[indxr]), riseavg/countr);
			countr++;
			indxr++;
		}

		/* Find the average riseavg, the average time between 2 RISE */
		riseavg = riseavg/countr;

		indxr = 0;

		/* The calculation for the Falling pulse on second channel */

		/* If the fall time is lower than rise time,
		 * Then there must be some error and we will increment
		 * both, until the error is gone
		 */
		if (fallData[indxf] < riseData[indxr])
		{
			indxf+=2; indxr+=2;
			while (fallData[indxf] < riseData[indxr]) indxf++;
		}

		else if (fallData[indxf] > riseData[indxr])
		{
			indxf+=2; indxr+=2;
			while (fallData[indxf] > riseData[indxr+1]) indxr++;
		}


		/* The method used for the calculation below is as follows:
		 * If Fall time < Rise Time, increment Fall counter
		 * If Fall time - Rise Time is in between 0 and (difference between 2 Rise times), then its a success
		 * If fall time > Rise time, but is also > (difference between 2 Rise times), then increment Rise Counter
		 */
		while ((indxf < (numval)) && (indxr < (numval)))
		{
			/* If the Fall time is lower than rise time, increment the fall indx */
			while ((int16_t)(fallData[indxf]-riseData[indxr]) < 0)
			{
				indxf++;
			}

			/* If the Difference in fall time and rise time is >0 and less than rise average,
			 * Then we will register it as a success and increment the countrf (the number of successes)
			 */
			if (((int16_t)(fallData[indxf]-riseData[indxr]) >= 0) && (((int16_t)(fallData[indxf]-riseData[indxr]) <= riseavg)))
			{
				rfavg += MIN((fallData[indxf]-riseData[indxr]), (fallData[indxf+1]-riseData[indxr+1]));
				indxf++;
				indxr++;
				countrf++;
			}

			else
			{
				indxr++;
			}
		}

		/* Calculate the Average time between 2 Rise */
		rfavg = rfavg/countrf;

		/* Calculate Frequency
		 * Freq = Clock/(time taken between 2 Rise)
		 */
		promFr=(uint8_t)riseavg;
		frequency = (refClock/(float)riseavg);;

		/* Width of the pulse
		 *  = (Time between Rise and fall) / clock
		 */
		promAn=(uint8_t)rfavg;
		width = ((rfavg)/((float)(refClock/1000000)))*1000;   // width in ns

		riseCaptured = 0;
		fallCaptured = 0;

		isMeasured = 1;

	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */


  HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_1, riseData, numval);
  HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_2, fallData, numval);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	  HAL_Delay (250);

	  if (isMeasured)
	  	  {
	  		  TIM2->CNT = 0;

	  		  HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_1, riseData, numval);

	  		  HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_2, fallData, numval);

	  		  isMeasured = 0;
	  	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 84-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : PD12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
