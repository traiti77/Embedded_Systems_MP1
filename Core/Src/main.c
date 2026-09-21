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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RCC_BASE        0x40023800UL // Reset and clock control
#define GPIOA_BASE      0x40020000UL // controls pin PA5 (LED)
#define GPIOC_BASE      0x40020800UL // controls pin PC13 (button)
#define SYSCFG_BASE     0x40013800UL
#define EXTI_BASE       0x40013C00UL
#define TIM6_BASE       0x40001000UL
#define NVIC_ISER_BASE  0xE000E100UL

#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))  // enable GPIO port bus clocks
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x40)) // enables bus clock for timer 6
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x44)) // enables bus clock for SYSCFG

#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x14)) // Output Data: writes 1 (ON) or 0 (OFF) to PA5

#define GPIOC_MODER     (*(volatile uint32_t *)(GPIOC_BASE + 0x00))
#define GPIOC_PUPDR     (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))

#define SYSCFG_EXTICR4  (*(volatile uint32_t *)(SYSCFG_BASE + 0x14))

#define EXTI_IMR        (*(volatile uint32_t *)(EXTI_BASE + 0x00))
#define EXTI_FTSR       (*(volatile uint32_t *)(EXTI_BASE + 0x0C))
#define EXTI_PR         (*(volatile uint32_t *)(EXTI_BASE + 0x14))

#define TIM6_CR1        (*(volatile uint32_t *)(TIM6_BASE + 0x00)) // start/stops counter
#define TIM6_DIER       (*(volatile uint32_t *)(TIM6_BASE + 0x0C))
#define TIM6_SR         (*(volatile uint32_t *)(TIM6_BASE + 0x10))
#define TIM6_PSC        (*(volatile uint32_t *)(TIM6_BASE + 0x28))
#define TIM6_ARR        (*(volatile uint32_t *)(TIM6_BASE + 0x2C))

#define NVIC_ISER1      (*(volatile uint32_t *)(NVIC_ISER_BASE + 0x04))
#define NVIC_ISER1_TIM6 (1U << (54 - 32))
#define NVIC_ISER1_EXTI (1U << (40 - 32))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t system_mode = 0; // 0 = Off, 1 = Solid, 2 = Blink
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void init_hardware(void);
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
  /* USER CODE BEGIN 2 */
  init_hardware();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    __asm__ volatile ("wfi"); // Wait For Interrupt
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
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  // Initializes the CPU and AHB and APB buses clocks
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

/* USER CODE BEGIN 4 */
void init_hardware(void) {
    // enable clocks
    RCC_AHB1ENR |= (1U << 0) | (1U << 2); // GPIOA & GPIOC
    RCC_APB2ENR |= (1U << 14); // SYSCFG
    RCC_APB1ENR |= (1U << 4); // TIM6

    // config
    GPIOA_MODER &= ~(3U << (5 * 2));
    GPIOA_MODER |=  (1U << (5 * 2)); // PA5 Output

    GPIOC_MODER &= ~(3U << (13 * 2)); // PC13 Input
    GPIOC_PUPDR &= ~(3U << (13 * 2));
    GPIOC_PUPDR |=  (1U << (13 * 2)); // PC13 Pull-up

    // EXTI Config
    SYSCFG_EXTICR4 &= ~(0xF << 4);
    SYSCFG_EXTICR4 |=  (0x2 << 4); // Map PC13 to EXTI13
    EXTI_IMR  |= (1U << 13); // Unmask Line 13
    EXTI_FTSR |= (1U << 13); // Falling edge trigger

    // timer Config (APB1 Timer Clock is 90 MHz with 180 MHz System Clock)
    TIM6_PSC = 90000 - 1;  // 1 kHz tick (90 MHz / 90000)
    TIM6_ARR = 125 - 1;    // 125 ms tick for 8 Hz overflow (4 Hz blink)
    TIM6_DIER |= (1U << 0); // update interrupt enable
    TIM6_CR1  |= (1U << 0); // start counter

    // NVIC config
    NVIC_ISER1 |= NVIC_ISER1_EXTI;
    NVIC_ISER1 |= NVIC_ISER1_TIM6;
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI_PR & (1U << 13)) {
        EXTI_PR = (1U << 13); // Clear pending bit

        system_mode = (system_mode + 1) % 3;

        if (system_mode == 0) {
            GPIOA_ODR &= ~(1U << 5); // Mode 0: Off
        } else if (system_mode == 1) {
            GPIOA_ODR |= (1U << 5);  // Mode 1: Solid ON
        }
    }
}

void TIM6_DAC_IRQHandler(void) {
    if (TIM6_SR & (1U << 0)) {
        TIM6_SR &= ~(1U << 0); // clear UIF flag

        if (system_mode == 2) {
            GPIOA_ODR ^= (1U << 5); // Mode 2: flashing LED
        }
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
