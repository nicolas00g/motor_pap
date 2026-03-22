/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ENABLE_PIN GPIO_PIN_0
#define DIR_PIN GPIO_PIN_1
#define STEP_PIN GPIO_PIN_2
#define ENABLE_PORT GPIOA
#define DIR_PORT GPIOA
#define STEP_PORT GPIOA

// PARÁMETROS DEL MOTOR - AJUSTA ESTOS VALORES
#define PASOS_POR_REVOLUCION   200  // 200 pasos = 360°
#define VELOCIDAD_MS            2   // Milisegundos entre pulsos (menor = más rápido)

// Cálculo automático: (200 pasos / 360°) * 180° = 100 pasos
#define PASOS_180   (PASOS_POR_REVOLUCION / 4)  // 100 pasos para 180°/* USER CODE END PD */
// Función para calcular pasos según ángulo
#define ANGULO_A_PASOS(angulo)  ((int)((angulo * PASOS_POR_REVOLUCION) / 360.0f))



// Prototipos
void Motor_Init(void);
void Motor_Enable(void);
void Motor_Disable(void);
void Motor_GirarPasos(int pasos, uint8_t direccion);
void Motor_Girar180(uint8_t direccion);

void Motor_GirarAngulo(float angulo, uint8_t direccion);
void Motor_MoverA(float angulo);  // Función principal para mover a un ángulo
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  /* USER CODE BEGIN 2 */
Motor_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // Mover suavemente de 0° a 55°
    Motor_MoverA(55);
    
    // Pausa de 1 segundo en la posición de 55°
    HAL_Delay(1000);
    
    // Mover suavemente de 55° a 0°
    Motor_MoverA(0);
    
    // Pausa de 1 segundo antes de repetir (opcional)
    HAL_Delay(1000);
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

/* USER CODE BEGIN 4 */

void Motor_Init(void){
	  HAL_GPIO_WritePin(STEP_PORT, STEP_PIN, GPIO_PIN_RESET);
	    HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_SET);
	    HAL_GPIO_WritePin(ENABLE_PORT, ENABLE_PIN, GPIO_PIN_SET);  // Motor deshabilitado
}

void Motor_Enable(void)
{
    HAL_GPIO_WritePin(ENABLE_PORT, ENABLE_PIN, GPIO_PIN_RESET);  // LOW = habilitado
}

void Motor_Disable(void)
{
    HAL_GPIO_WritePin(ENABLE_PORT, ENABLE_PIN, GPIO_PIN_SET);  // HIGH = deshabilitado
}

void Motor_GirarPasos(int pasos, uint8_t direccion)
{
    // Configurar dirección
    if(direccion)
        HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_RESET);

    // Habilitar motor
    Motor_Enable();

    // Generar pulsos
    for(int i = 0; i < pasos; i++)
    {
        // Pulso STEP
        HAL_GPIO_WritePin(STEP_PORT, STEP_PIN, GPIO_PIN_SET);
        HAL_Delay(1);  // 1ms de pulso
        HAL_GPIO_WritePin(STEP_PORT, STEP_PIN, GPIO_PIN_RESET);
        HAL_Delay(VELOCIDAD_MS);  // Espera entre pulsos
    }

    // Deshabilitar motor (opcional - ahorra energía)
    Motor_Disable();
}

void Motor_Girar180(uint8_t direccion)
{
    Motor_GirarPasos(PASOS_180, direccion);
}


void Motor_GirarAngulo(float angulo, uint8_t direccion)
{
    int pasos = ANGULO_A_PASOS(angulo);
    Motor_GirarPasos(pasos, direccion);
}

// Función principal: mueve el motor al ángulo especificado
// asumiendo que parte desde la posición actual
void Motor_MoverA(float angulo_nuevo)
{
    static float angulo_actual = 0.0f;  // Guarda la posición actual

    float diferencia = angulo_nuevo - angulo_actual;

    if(diferencia > 0)
    {
        // Mover en dirección positiva
        Motor_GirarAngulo(diferencia, 1);
    }
    else if(diferencia < 0)
    {
        // Mover en dirección negativa
        Motor_GirarAngulo(-diferencia, 0);
    }
    else
    {
        // Ya estamos en el ángulo, no hacer nada
        return;
    }

    // Actualizar ángulo actual
    angulo_actual = angulo_nuevo;
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
