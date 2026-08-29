/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
CAN_HandleTypeDef hcan;

I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
CAN_FilterTypeDef sFilterConfig;

uint8_t flag=0;
uint8_t Data[8];
uint8_t r[8];
uint8_t result[8];
uint32_t TxMailbox;
uint8_t TxData[8];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  TxHeader.StdId=0xc3;
  TxHeader.DLC=8;
  TxData[0]=0;
  TxData[1]=0;
  TxData[2]=0;
  TxData[3]=0;
  TxData[4]=0;
  TxData[5]=0;
  TxData[6]=0;
  TxData[7]=0;
  TxHeader.ExtId=0;
  TxHeader.IDE=CAN_ID_STD;
  TxHeader.RTR=CAN_RTR_DATA;
  TxHeader.TransmitGlobalTime=DISABLE;
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
  MX_CAN_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  sFilterConfig.FilterFIFOAssignment=CAN_FILTER_FIFO0;
  sFilterConfig.FilterBank=0;
  sFilterConfig.FilterIdHigh=0xc1<<5;
  sFilterConfig.FilterIdLow=0;
  sFilterConfig.FilterScale=CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
  sFilterConfig.FilterActivation=CAN_FILTER_ENABLE;

  HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);
  HAL_CAN_Start(&hcan);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(flag==1)
	  	  {
	  		  HAL_I2C_Mem_Write(&hi2c1, 0xA1, 0x02, 1, &Data[0], sizeof(Data[0]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Write(&hi2c1, 0xA1, 0x03, 1, &Data[1], sizeof(Data[1]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Write(&hi2c1, 0xA1, 0x04, 1, &Data[2], sizeof(Data[2]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Write(&hi2c1, 0xA1, 0x05, 1, &Data[3], sizeof(Data[3]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Write(&hi2c1, 0xA1, 0x06, 1, &Data[4], sizeof(Data[4]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Write(&hi2c1, 0xA1, 0x07, 1, &Data[5], sizeof(Data[5]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Write(&hi2c1, 0xA1, 0x08, 1, &Data[6], sizeof(Data[6]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Write(&hi2c1, 0xA1, 0x09, 1, &Data[7], sizeof(Data[7]), HAL_MAX_DELAY);
	  		  HAL_Delay(100);
	  		  HAL_I2C_Mem_Read(&hi2c1, 0xA1, 0x02, 1, &result[0], sizeof(result[0]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Read(&hi2c1, 0xA1, 0x03, 1, &result[1], sizeof(result[1]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Read(&hi2c1, 0xA1, 0x04, 1, &result[2], sizeof(result[2]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Read(&hi2c1, 0xA1, 0x05, 1, &result[3], sizeof(result[3]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Read(&hi2c1, 0xA1, 0x06, 1, &result[4], sizeof(result[4]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Read(&hi2c1, 0xA1, 0x07, 1, &result[5], sizeof(result[5]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Read(&hi2c1, 0xA1, 0x08, 1, &result[6], sizeof(result[6]), HAL_MAX_DELAY);
	  		  HAL_I2C_Mem_Read(&hi2c1, 0xA1, 0x09, 1, &result[7], sizeof(result[7]), HAL_MAX_DELAY);
	  		  TxData[0]=result[0];
	  		  TxData[1]=result[1];
	  		  TxData[2]=result[2];
	  		  TxData[3]=result[3];
	  		  TxData[4]=result[4];
	  		  TxData[5]=result[5];
	  		  TxData[6]=result[6];
	  		  TxData[7]=result[7];
	  		  HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
	  		  flag=0;
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 9;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_4TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
