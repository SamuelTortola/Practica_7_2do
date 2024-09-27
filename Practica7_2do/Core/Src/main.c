/* USER CODE BEGIN Header */
/**
   ******************************************************************************
 Universidad Del Valle De Guatemala
 IE2023: Electrónica digital 2
 Autor: Samuel Tortola - 22094
 Proyecto: Laboratorio 7
 Hardware: STM32
 Creado: 25/09/2024
Última modificación: 27/09/2024
******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fatfs_sd.h"
#include "string.h"
#include "stdio.h"
//#include <stdlib.h>


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

SPI_HandleTypeDef hspi1;
FATFS fs;
FATFS *pfs;
FIL fil;
FRESULT fres;
DWORD fre_clust;
uint32_t totalSpace,freeSpace;
char buffer[100];

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

uint8_t rx_data[1]; // Solo para recibir byte por byte
uint8_t activa = 0, activa1 = 0;

#define MAX_FILES 10
#define FILENAME_MAX_LEN 50

char file_list[MAX_FILES][FILENAME_MAX_LEN];  // Lista de archivos
int file_count = 0;  // Número de archivos

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void transmit_uart(char *string);
void read_file_by_index(int index);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void transmit_uart(char *string);

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
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_FATFS_Init();


  /* USER CODE BEGIN 2 */

  HAL_UART_Receive_IT(&huart2,rx_data, 1);  // Empezar la recepción por UART1 en modo interrupción
  HAL_Delay(500);

  // Montar SD
  fres = f_mount(&fs,"/" , 0);
  if(fres == FR_OK){
      transmit_uart("Micro SD is mounted successfully\n\n\n\n");
  } else {
      transmit_uart("Micro SD is mounted bad\n\n\n\n");
  }


  transmit_uart("******************Bienvenido/a*************************\n\n");
  transmit_uart("Coloque A para leer la lista de archivos, B para desmostar unidad, D para montar unidad\n");
  transmit_uart("Coloque C y el identificador del archivo para leer\n\n");


/*
  // Abrir archivo
  fres = f_open(&fil, "Prueba.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
  if(fres == FR_OK){
      transmit_uart("File opened for reading and writing\n");
  } else {
      transmit_uart("File was not opened\n");
  }

  // Escribir en archivo
  for(uint8_t i = 0; i < 1; i++){
      if (f_puts("Hola Mundo\n", &fil) < 0) {
          transmit_uart("Error writing to file\n");
      } else {
          transmit_uart("Successfully wrote to file\n");
      }
  }


  // Cerrar archivo
  fres = f_close(&fil);
  if(fres == FR_OK){
      transmit_uart("File was closed\n");
  } else {
      transmit_uart("File was not closed\n");
  }
    */




  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  if(activa == 1){      //Si se desea leer la lista de archivos que tiene el archivo

		  DIR dir;
		     FILINFO fno;
		     file_count = 0;  // Reiniciar el contador de archivos

		     FRESULT fres = f_opendir(&dir, "/");
		     if (fres == FR_OK) {
		         while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0] && file_count < MAX_FILES) {
		             // Almacenar el nombre del archivo en la lista
		             strcpy(file_list[file_count], fno.fname);
		             file_count++;
		         }
		         f_closedir(&dir);
		     }

		     // Enviar la lista de archivos por UART a Python
		     for (int i = 0; i < file_count; i++) {
		         transmit_uart(file_list[i]);
		         transmit_uart("\n");
		     }

			 HAL_Delay(500);
			 activa = 0;
	  }

	  if(activa == 2){         //Si se desea desmontar el archivo
		  // Desmontar SD
				  f_mount(NULL, "", 0);
				  if(fres == FR_OK){
					  transmit_uart("Micro SD was unmounted\n");
				  } else {
					  transmit_uart("Micro SD was not unmounted\n");
				  }

				 HAL_Delay(500);
				 activa = 0;
	  }

	   if(activa == 3 && activa1 != 0){         //Si se desea abrir un archivo


		   read_file_by_index(activa1);  // Leer el archivo correspondiente

/*
		  char buffer[1000];
		  fres = f_open(&fil, "Prueba.txt", FA_READ);
		  if (fres == FR_OK) {
		      while (f_gets(buffer, sizeof(buffer), &fil)) {
		          transmit_uart(buffer);
		      }
		      f_close(&fil);
		  } else {
		      transmit_uart("Error opening file\n");
		  }*/

		  HAL_Delay(500);
		  activa = 0;
		  activa1 = 0;

	  }

	   if(activa == 4){
		   // Montar SD
		     fres = f_mount(&fs,"/" , 0);
		     if(fres == FR_OK){
		         transmit_uart("Micro SD is mounted successfully\n\n\n\n");
		     } else {
		         transmit_uart("Micro SD is mounted bad\n\n\n\n");
		     }
		     activa = 0;
		    activa1 = 0;

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  huart2.Init.BaudRate = 9600;
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
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SD_SS_GPIO_Port, SD_SS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : SD_SS_Pin */
  GPIO_InitStruct.Pin = SD_SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(SD_SS_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void transmit_uart(char *data) {
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), HAL_MAX_DELAY);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	 if (huart->Instance == USART2) // Verificar si la interrupción es de UART2
	  {
		 if (rx_data[0] == 'A'){
			 activa = 1;
		 }

		 else if (rx_data[0] == 'B'){
			 activa = 2;

		 }

		 else if (rx_data[0] == 'C'){
			 activa = 3;

				}

		 else if (rx_data[0] == 'D'){
					 activa = 4;

				}
		 else{
			 activa1 = atoi((char*)rx_data);  // Usa un cast a char*

		 }
	  }

	 // Volver a habilitar la recepción por UART2
	    HAL_UART_Receive_IT(&huart2, rx_data, 1);

}


// Función para leer el archivo basado en el índice
void read_file_by_index(int index) {
    if (index < file_count && index >= 0) {
        FIL fil;
        FRESULT fres = f_open(&fil, file_list[index], FA_READ);
        if (fres == FR_OK) {
            char line[100];
            while (f_gets(line, sizeof(line), &fil)) {
                transmit_uart(line);
            }
            f_close(&fil);
        } else {
            transmit_uart("Error opening file\n");
        }
    } else {
        transmit_uart("Invalid file index\n");
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
