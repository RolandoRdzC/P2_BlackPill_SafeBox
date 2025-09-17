#include "stm32f411xe.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal_gpio.h"
#include <main.h>
// extern UART_HandleTypeDef huart1;

void System_Init(void) {
  HAL_Init();
  Clock_Init();
  GPIO_Init();
  // UART1_Init();
}
void Clock_Init(void) {
  // FUNCION PARA CONFIGURAR EL RELOJ a 100MHZ
  RCC_OscInitTypeDef osc_config = {0};
  osc_config.OscillatorType =
      RCC_OSCILLATORTYPE_HSE;           // ACTIVAMOS EL OSCILADOR EXTERNO 25MHz
  osc_config.HSEState = RCC_HSE_ON;     // ENCENDEMOS EL RELOJ
  osc_config.PLL.PLLState = RCC_PLL_ON; // ENCENDEMOS EL MULTIPLEXOR PLL
  osc_config.PLL.PLLSource =
      RCC_PLLSOURCE_HSE;     // SELECCIONAMOS EL INTERNO DE 16MHz
  osc_config.PLL.PLLM = 25;  // Dividimos el PLL M por 25
  osc_config.PLL.PLLN = 200; // Multiplicamos por 100
  osc_config.PLL.PLLP = 2;   // DIVIDIMOS POR 2
  if (HAL_RCC_OscConfig(&osc_config) != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitTypeDef clk_config = {0}; // ESTRUCTURA PARA EL MANEJADOR DEL CLOCK
  clk_config.ClockType =
      RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 |
      RCC_CLOCKTYPE_PCLK2; // SELECCIONAMOS TODOS LOS TIPOS DE RELOJ
  clk_config.SYSCLKSource =
      RCC_SYSCLKSOURCE_PLLCLK; // SELECCIONAMOS PLLCLK DEL MULTIPLEXOR
  clk_config.AHBCLKDivider =
      RCC_SYSCLK_DIV1; // DIVIDIMOS ENTRE 1 EL PRESCALADOR AHB
  clk_config.APB1CLKDivider =
      RCC_HCLK_DIV2; // DIVIDIMOS ENTRE 2 EL PRESCALADOR APB1
  clk_config.APB2CLKDivider =
      RCC_HCLK_DIV1; // DIVIDIMOS ENTRE 1 EL PRESCALER APB2
  if (HAL_RCC_ClockConfig(&clk_config, FLASH_LATENCY_4) != HAL_OK) {
    Error_Handler();
  }
}

void GPIO_Init(void) {
  // Habilitar relojes para GPIOA, GPIOB y GPIOC
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  // Estructura para configuración de pines
  GPIO_InitTypeDef pin = {0};
  
  // Configurar LED de usuario (PC13)
  pin.Pin = GPIO_PIN_13;
  pin.Mode = GPIO_MODE_OUTPUT_PP;
  pin.Pull = GPIO_NOPULL;
  pin.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &pin);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, DIGIT_OFF);
  
  // Configurar pines para segmentos (PA0-PA7)
  pin.Pin = SEG_A_PIN | SEG_B_PIN | SEG_C_PIN | SEG_D_PIN |
            SEG_E_PIN | SEG_F_PIN | SEG_G_PIN | SEG_DP_PIN;
  pin.Mode = GPIO_MODE_OUTPUT_PP;  // Push-pull para mejor control de corriente
  pin.Pull = GPIO_NOPULL;         // No necesitamos pull-up/down internos
  pin.Speed = GPIO_SPEED_FREQ_MEDIUM;  // Velocidad media es suficiente para multiplexado
  HAL_GPIO_Init(GPIOA, &pin);
  
  // Configurar pines para dígitos (PA8-PA11)
  pin.Pin = DIGIT_1_PIN | DIGIT_2_PIN | DIGIT_3_PIN | DIGIT_4_PIN;
  pin.Mode = GPIO_MODE_OUTPUT_PP;
  pin.Pull = GPIO_NOPULL;
  pin.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &pin);

  // Configurar pines de columnas del teclado (PA12-PA15)
  pin.Pin = COLUMN_1_PIN | COLUMN_2_PIN | COLUMN_3_PIN | COLUMN_4_PIN;
  pin.Mode = GPIO_MODE_OUTPUT_PP;
  pin.Pull = GPIO_NOPULL;
  pin.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(COLUMN_PORT, &pin);

  // Configurar pines de filas del teclado (PB0-PB3) como entradas con pull-down
  pin.Pin = ROW_1_PIN | ROW_2_PIN | ROW_3_PIN | ROW_4_PIN;
  pin.Mode = GPIO_MODE_INPUT;
  pin.Pull = GPIO_PULLDOWN;  // Pull-down para evitar lecturas falsas
  pin.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(ROW_PORT, &pin);

  // Configurar pin de bloqueo (PB12)
  pin.Pin = LOCK_PIN;
  pin.Mode = GPIO_MODE_OUTPUT_PP;
  pin.Pull = GPIO_NOPULL;
  pin.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LOCK_PORT, &pin);
  
  // Inicializar todos los segmentos y dígitos en estado apagado
  HAL_GPIO_WritePin(SEG_PORT, SEG_A_PIN | SEG_B_PIN | SEG_C_PIN | SEG_D_PIN |
                              SEG_E_PIN | SEG_F_PIN | SEG_G_PIN | SEG_DP_PIN, SEGMENT_OFF);
  HAL_GPIO_WritePin(DIGIT_PORT, DIGIT_1_PIN | DIGIT_2_PIN | DIGIT_3_PIN | DIGIT_4_PIN, DIGIT_OFF);
  
  // Inicializar columnas del teclado y pin de bloqueo en estado inicial
  HAL_GPIO_WritePin(COLUMN_PORT, COLUMN_1_PIN | COLUMN_2_PIN | COLUMN_3_PIN | COLUMN_4_PIN, DIGIT_OFF);
  HAL_GPIO_WritePin(LOCK_PORT, LOCK_PIN, GPIO_PIN_SET);
}

/*
void UART1_Init(void) {
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_USART1_CLK_ENABLE();

  GPIO_InitTypeDef uart_pins = {0};
  uart_pins.Pin = GPIO_PIN_9 | GPIO_PIN_10;
  uart_pins.Mode = GPIO_MODE_AF_PP;
  // uart_pins.Pull      = GPIO_PULLUP;
  uart_pins.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOA, &uart_pins);

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.Mode = UART_MODE_TX;
  if (HAL_UART_Init(&huart1) != HAL_OK)
    Error_Handler();
}*/

void Error_Handler(void) {
  while (1) {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(100);
  }
}
