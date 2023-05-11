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
#include "stdio.h"
#include "string.h"
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
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
uint8_t RxBuffer[1];
uint8_t TxBuffer[40];

//Menu
uint8_t Menu[700] = 	"============================= \r\n"
						"      ****  Menu ****         \r\n"
						"  < Please Select 1 Choise >  \r\n"
						"            GoGO !!           \r\n"
						"       0 : LED CONTROL        \r\n"
						"       1 : BUTTON STATUS      \r\n"
						"============================= \r\n";
uint8_t Menu2[700] = 	"============================= \r\n"
						"  ** Menu of LED Control **   \r\n"
						"  < Please Select 1 Choise >  \r\n"
						"            GoGO !!           \r\n"
						"       a : SPEED UP      	   \r\n"
						"       s : SPEED DOWN     	   \r\n"
						"       d : ON/OFF      	   \r\n"
						"============================= \r\n"
						"     X : GO BACK TO MENU      \r\n"
						"============================= \r\n";
uint8_t Menu3[700] =    "============================= \r\n"
						"      **  Menu BUTTON **      \r\n"
						"  < Please Select 1 Choise >  \r\n"
						"            GoGO !!           \r\n"
						"     X : GO BACK TO MENU      \r\n"
						"============================= \r\n";

uint8_t state = 0;

// status
uint8_t LED_ON[10] = "ON\r\n";
uint8_t LED_OFF[10] = "OFF\r\n";
uint8_t wrong_op[20] = "You Are Wrong\r\n";

//Button
uint8_t Button =0;
uint8_t Check = 0;
uint8_t Press_Button[30] = "PRESSED\r\n" ;
uint8_t Undo[30] = "UNPRESSED\r\n"    ;
uint8_t once =0;

//LED
uint8_t HZ =0;
uint8_t HZ_2 =0;
uint8_t LED_st = 0;
uint8_t HZ_Check =0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void UARTPollingMethod();
void DummyTask();
void interrupt();
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  interrupt();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_GPIO_TogglePin(GPIOA, GPIO_Pin_0);
		static uint32_t timestamp = 0;
		if (HAL_GetTick() > timestamp) {

			if (HZ > 0) {
				HZ_Check = (1/HZ)*500;
			}
			else
			{
				HZ_Check = 0;
			}
			timestamp = HAL_GetTick() + HZ_Check;

			if(LED_st == 0 || HZ == 0){
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, RESET);
			}
			else{
				HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			}
		}

		switch (state)
		{
		case 0:
			HAL_Delay(5);
			HAL_UART_Transmit_IT(&huart2, Menu, strlen((char*) Menu));
			state = 1;
			break;
		case 1:
			if(RxBuffer[0] == '0'){
				RxBuffer[0] = 0;
				state = 2;
			}
			else if(RxBuffer[0] == '1'){
				RxBuffer[0] = 0;
				state = 4;
			}
			else if(RxBuffer[0] != '0' && '1' )
			{
				HAL_Delay(5);
				HAL_UART_Transmit_IT(&huart2, wrong_op, strlen((char*) wrong_op));
				RxBuffer[0] = 0;
				state = 1;
			}
			break;
		case 2:
			HAL_Delay(5);
			HAL_UART_Transmit_IT(&huart2, Menu2, strlen((char*) Menu2));
			state = 3;
			break;
		case 3:
			if (RxBuffer[0] == 'a')
			{
				HZ += 1;
				HZ_2 += 1;

				state = 3;
				RxBuffer[0] = 0;

			}
			else if (RxBuffer[0] == 's')
			{
				HZ -= 1;
				if(HZ <= 0){
					HZ = 0;
				}
				HZ_2 -= 1;
				if(HZ_2 <= 0){
					HZ_2 = 0;
				}

				state = 3;
				RxBuffer[0] = 0;
			}
			else if(RxBuffer[0] == 'd')
			{
				if(LED_st)
				{
					LED_st = 0;
					RxBuffer[0] = 0;

					HZ = 0;
					HAL_UART_Transmit_IT(&huart2, LED_OFF, strlen((char*) LED_OFF));
					HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, SET);
				}
				else
				{
					LED_st = 1;
					RxBuffer[0] = 0;

					HZ = HZ_2;
					HAL_UART_Transmit_IT(&huart2, LED_ON, strlen((char*) LED_ON));
				}
			}
			else if(RxBuffer[0] == 'x')
			{
				RxBuffer[0] = 0;
				state = 0;
			}
			break;

		case 4:
			HAL_Delay(5);
			HAL_UART_Transmit_IT(&huart2, Menu3, strlen((char*) Menu3));
			state = 5;
			break;
		case 5:
			Button = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
			if(Button == 1 && Check == 0)
			{
				HAL_UART_Transmit_IT(&huart2, Undo, strlen((char*) Undo));
				once = 0;
			}
			else if(Button == 0 && Check == 0)
			{
				if(!once)
				 {
					HAL_UART_Transmit_IT(&huart2, Press_Button, strlen((char*) Press_Button));
					once = 1;
				}
			}
			else if (RxBuffer[0] == 'x')
			{
				RxBuffer[0] = 0;
				state = 0;
			}
			Check = Button;
			break;
		}

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 57600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 LD2_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void interrupt()
{
	HAL_UART_Receive_IT(&huart2, RxBuffer , 1);

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart2)
	{
		RxBuffer[2]= '\0';
		//return
		sprintf((char*)TxBuffer,"CHOOSE: %s\r\n" ,RxBuffer);
		HAL_UART_Transmit_IT(&huart2, TxBuffer,strlen((char*)TxBuffer));
		// recall
		HAL_UART_Receive_IT(&huart2, RxBuffer, 1);
	}
}
void UARTPollingMethod()
{
	//10ตัว in 10000ms
	HAL_StatusTypeDef HAL_status = HAL_UART_Receive(&huart2, RxBuffer , 10 ,10000);
	if(HAL_status == HAL_OK)
	{
		RxBuffer[10] = '\0';
		sprintf((char*)TxBuffer,"Received : %s\r\n" ,RxBuffer);
		HAL_UART_Transmit(&huart2, TxBuffer,strlen((char*)TxBuffer),10);
	}
	else if (HAL_status == HAL_TIMEOUT)
	{
		uint32_t lastCharPos = huart2.RxXferSize - huart2.RxXferCount;
		RxBuffer[lastCharPos] = '\0';
		//return received
		sprintf((char*)TxBuffer,"Received Timeout : %s\r\n" ,RxBuffer);
		HAL_UART_Transmit(&huart2, TxBuffer,strlen((char*)TxBuffer),10);

	}
}
void DummyTask()
{
	static uint32_t timestamp=0;
	if(HAL_GetTick()>= timestamp)
	{
		timestamp = HAL_GetTick()+100;
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	}
}
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
