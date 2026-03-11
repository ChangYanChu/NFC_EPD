/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "epd.h"
#include <string.h>
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
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */





/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define NT3H_I2C_ADDR 0xaa
#define SRAM_SIZE 64
uint32_t total_writen = 0;
uint32_t have_writen = 0;
uint32_t all_count = 0;
bool stopFlag = false;
bool writeDone = false;
bool refreshPending = false;
bool invertByte = false;

static uint32_t EPD_FrameBytes(void)
{
#if defined(NO_RED)
  return (uint32_t)EPD_WIDTH * (uint32_t)EPD_HEIGHT / 8;
#else
  return (uint32_t)EPD_WIDTH * (uint32_t)EPD_HEIGHT / 4;
#endif
}

static void EPD_FillRemainingFrame(uint32_t written_bytes)
{
  uint32_t frame_bytes = EPD_FrameBytes();
  uint8_t white_byte = 0xFF;

  while (written_bytes < frame_bytes) {
    EPD_SendData(white_byte);
    written_bytes++;
  }
}

void enableMirror(void){
// add sram mirror
	uint8_t data[4] = {0xfe, 0x00, 0x2, 0x2};
	HAL_I2C_Master_Transmit(&hi2c1, NT3H_I2C_ADDR, data, sizeof(data), 100);
}

void startPassthrough(void){
  uint8_t data[4] = {0xfe, 0x00, 0x40, 0x40};
  HAL_I2C_Master_Transmit(&hi2c1, NT3H_I2C_ADDR, data, sizeof(data), 100);
}

void stopPassthrough() {
  uint8_t data[4] = {0xfe, 0x00, 0x40, 0x0};
  HAL_I2C_Master_Transmit(&hi2c1, NT3H_I2C_ADDR, data, sizeof(data), 100);
}

void setI2CtoNFC(){
  stopPassthrough();
  uint8_t data[4] = {0xfe, 0x00, 0x1, 0x0};
  HAL_I2C_Master_Transmit(&hi2c1, NT3H_I2C_ADDR, data, sizeof(data), 100);
  startPassthrough();
}


void setNFCtoI2C(){
  stopPassthrough();
  uint8_t data[4] = {0xfe, 0x00, 0x1, 0x1};
  HAL_I2C_Master_Transmit(&hi2c1, NT3H_I2C_ADDR, data, sizeof(data), 100);
  startPassthrough();
}



bool checkReady()
{
    uint8_t rxData[1] = {0};
    uint8_t txData[2] = {0xFE, 0x06};
    HAL_I2C_Master_Transmit(&hi2c1, NT3H_I2C_ADDR, txData, 2, 100);
    HAL_I2C_Master_Receive(&hi2c1, NT3H_I2C_ADDR, rxData, 1, 100);
	return (rxData[0] & 16) != 0;
		//									0b10000
}

void WriteACK(uint8_t *dataBuffer)
{
    setI2CtoNFC();
    dataBuffer[SRAM_SIZE-4] = 'A';
    dataBuffer[SRAM_SIZE-3] = 'C';
    dataBuffer[SRAM_SIZE-2] = 'K';
    HAL_I2C_Mem_Write(&hi2c1,             
                                  NT3H_I2C_ADDR,     
                                  0xfb,                   
                                  I2C_MEMADD_SIZE_8BIT,  
                                  dataBuffer + 3*16,     
                                  16,                  
                                  HAL_MAX_DELAY);        
    setNFCtoI2C();
}

bool checkFP(uint8_t *data){
    uint8_t pg_data[48] = {0};
   if (memcmp(data, pg_data, 48) == 0 && data[SRAM_SIZE-4] == 'F' && data[SRAM_SIZE-3] == 'P')
   {
      total_writen = data[SRAM_SIZE-5] + (data[SRAM_SIZE-6] << 8);
      have_writen = 0;
      return true;

  }else if (memcmp(data, pg_data, 48) == 0 && data[SRAM_SIZE-4] == 'F' && data[SRAM_SIZE-3] == 'S')
   {
    if (!writeDone && all_count > 0) {
      EPD_FillRemainingFrame(all_count);
      all_count = EPD_FrameBytes();
      writeDone = true;
      refreshPending = true;
    }
      HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_SET);
      stopFlag = true;
      return true;
   }
   return false;
   

}

void ReadDataBlock(uint8_t block_address, uint8_t* out_buffer, int out_buffer_length)
{
      HAL_I2C_Master_Transmit(&hi2c1, NT3H_I2C_ADDR, &block_address, 1, 100);
      HAL_I2C_Master_Receive(&hi2c1, NT3H_I2C_ADDR, out_buffer, out_buffer_length, 100);
}


void readPages(uint8_t startPage, uint8_t endPage, uint8_t *data64) 
{   
    uint8_t i = 0;
    for (uint8_t page = startPage; page <= endPage; page++) 
    {
        ReadDataBlock(page, data64 + i*16, 16);
        i++;
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
  MX_I2C1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_SET);
  EPD_Init();
	
	EPD_Start_Black();
/*

    UWORD Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    UWORD Height = EPD_HEIGHT;

    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0x00);
					EPD_SendData(0xff);
        }
    }
		
		EPD_TurnOnDisplay();
 */
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	
	HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_RESET);
	

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			startPassthrough();
      if (checkReady()){
         uint8_t data64[64];
         readPages(0xf8, 0xfb, data64);
         if (checkFP(data64)) {
           if (stopFlag) break;
           continue;
         }
				
         if (!writeDone){
  						#if defined(NO_RED)
                if (all_count >= EPD_WIDTH*EPD_HEIGHT/8){
                    writeDone = true;
                    refreshPending = true;
  							}
							//EPD_write64(data64);
                  DEV_Digital_Write(EPD_DC_PIN, 1);
									DEV_Digital_Write(EPD_CS_PIN, 0);
									DEV_SPI_Write_nByte(data64, 64);
									DEV_Digital_Write(EPD_CS_PIN, 1);
							#else
               if (all_count >= EPD_WIDTH*EPD_HEIGHT/8*2){
                  writeDone = true;
                  refreshPending = true;
                }
							  if (invertByte){
									for(size_t i=0;i<64;i++){
										data64[i] ^= 0xff;
									}
								}
								// boundary red and black
								if (all_count < EPD_WIDTH*EPD_HEIGHT/8 && all_count+64 >= EPD_WIDTH*EPD_HEIGHT/8){
									uint8_t black_p = EPD_WIDTH*EPD_HEIGHT/8 - all_count;
									
									DEV_Digital_Write(EPD_DC_PIN, 1);
									DEV_Digital_Write(EPD_CS_PIN, 0);
									DEV_SPI_Write_nByte(data64, black_p);
									DEV_Digital_Write(EPD_CS_PIN, 1);
									
									EPD_Start_Red();
									// EPD_HINK need
									invertByte = true;
									
									uint8_t dataRed[64-black_p];
									for(size_t i=0;i<64;i++){
										data64[i] ^= 0xff;
									}
									memcpy(dataRed, &data64[black_p], sizeof(dataRed));
									DEV_Digital_Write(EPD_DC_PIN, 1);
									DEV_Digital_Write(EPD_CS_PIN, 0);
									DEV_SPI_Write_nByte(data64, 64-black_p);
									DEV_Digital_Write(EPD_CS_PIN, 1);
								}else{
									DEV_Digital_Write(EPD_DC_PIN, 1);
									DEV_Digital_Write(EPD_CS_PIN, 0);
									DEV_SPI_Write_nByte(data64, 64);
									DEV_Digital_Write(EPD_CS_PIN, 1);
								}
							#endif
	
					all_count += 64;
          #if defined(NO_RED)
          if (!writeDone && all_count >= EPD_WIDTH*EPD_HEIGHT/8){
            writeDone = true;
            refreshPending = true;
          }
          #else
          if (!writeDone && all_count >= EPD_WIDTH*EPD_HEIGHT/8*2){
            writeDone = true;
            refreshPending = true;
          }
          #endif
				 }							 
         have_writen += 64;
         if (have_writen >= total_writen && total_writen != 0){
            WriteACK(data64);
            have_writen = 0;
         }
				}
      
     if (stopFlag) break;
  }
  if (refreshPending){
    EPD_TurnOnDisplay();
    HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_SET);
  }
	EPD_ReadBusy();
	
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
  hi2c1.Init.Timing = 0x00000608;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14|LED_PIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, EPD_DC_Pin|EPD_CS_Pin|EPD_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC14 LED_PIN_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_14|LED_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : EPD_DC_Pin EPD_CS_Pin EPD_RST_Pin */
  GPIO_InitStruct.Pin = EPD_DC_Pin|EPD_CS_Pin|EPD_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : EPD_BUSY_Pin */
  GPIO_InitStruct.Pin = EPD_BUSY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(EPD_BUSY_GPIO_Port, &GPIO_InitStruct);

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
#ifdef USE_FULL_ASSERT
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
