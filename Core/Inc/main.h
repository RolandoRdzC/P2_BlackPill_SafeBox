#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <timers.h>

// Configuración del tipo de display
#define COMMON_ANODE 1 // Cambiar a 0 para display de cátodo común
#if COMMON_ANODE
#define SEGMENT_ON 0
#define SEGMENT_OFF 1
#define DIGIT_ON 1
#define DIGIT_OFF 0
#else
#define SEGMENT_ON 1
#define SEGMENT_OFF 0
#define DIGIT_ON 0
#define DIGIT_OFF 1
#endif

// Definiciones para los pines de los segmentos
#define SEG_A_PIN GPIO_PIN_0
#define SEG_B_PIN GPIO_PIN_1
#define SEG_C_PIN GPIO_PIN_2
#define SEG_D_PIN GPIO_PIN_3
#define SEG_E_PIN GPIO_PIN_4
#define SEG_F_PIN GPIO_PIN_5
#define SEG_G_PIN GPIO_PIN_6
#define SEG_DP_PIN GPIO_PIN_7
#define SEG_PORT GPIOA

// Definiciones para los pines de los dígitos
#define DIGIT_1_PIN GPIO_PIN_8
#define DIGIT_2_PIN GPIO_PIN_9
#define DIGIT_3_PIN GPIO_PIN_10
#define DIGIT_4_PIN GPIO_PIN_11
#define DIGIT_PORT GPIOA

// Definiciones para los pines de las columnas
#define COLUMN_1_PIN GPIO_PIN_12
#define COLUMN_2_PIN GPIO_PIN_13
#define COLUMN_3_PIN GPIO_PIN_14
#define COLUMN_4_PIN GPIO_PIN_15
#define COLUMN_PORT GPIOA

#define ROW_1_PIN GPIO_PIN_0
#define ROW_2_PIN GPIO_PIN_1
#define ROW_3_PIN GPIO_PIN_2
#define ROW_4_PIN GPIO_PIN_3
#define ROW_PORT GPIOB

void System_Init(void);
void Clock_Init(void);
void UART1_Init(void);
void GPIO_Init(void);
void Error_Handler();

void DisplayTask(void *pvParameters);
void CounterTask(void *pvParameters);
void KeyboardTask(void *pvParameters);
void updateDigits(uint16_t number);
void displayDigit(uint8_t number);

#endif /* __MAIN_H */
