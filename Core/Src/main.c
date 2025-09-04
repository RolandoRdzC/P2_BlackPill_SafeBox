#include "main.h"
#include "stm32f4xx_hal_gpio.h"

const uint8_t numberPatterns[] = {
    ~0x3F, // 0
    ~0x06, // 1
    ~0x5B, // 2
    ~0x4F, // 3
    ~0x66, // 4
    ~0x6D, // 5
    ~0x7D, // 6
    ~0x07, // 7
    ~0x7F, // 8
    ~0x6F  // 9
};

volatile uint16_t displayNumber = 0;
volatile uint8_t digits[4] = {0};

int main(void) {
  System_Init();
  xTaskCreate(DisplayTask, "Display", 128, NULL, 2, NULL);
  xTaskCreate(CounterTask, "Counter", 128, NULL, 1, NULL);
  vTaskStartScheduler();
}

void DisplayTask(void *pvParameters) {
  UNUSED(pvParameters);
  uint8_t currentDigit = 0;

  while (1) {
    HAL_GPIO_WritePin(DIGIT_PORT,
                      DIGIT_1_PIN | DIGIT_2_PIN | DIGIT_3_PIN | DIGIT_4_PIN,
                      DIGIT_OFF);
    displayDigit(digits[currentDigit]);
    uint16_t digitPin;
    switch (currentDigit) {
    case 0:
      digitPin = DIGIT_1_PIN;
      break;
    case 1:
      digitPin = DIGIT_2_PIN;
      break;
    case 2:
      digitPin = DIGIT_3_PIN;
      break;
    case 3:
      digitPin = DIGIT_4_PIN;
      break;
    }
    HAL_GPIO_WritePin(DIGIT_PORT, digitPin, DIGIT_ON);
    currentDigit = (currentDigit + 1) % 4;
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void CounterTask(void *pvParameters __attribute__((unused))) {
  while (1) {
    displayNumber++;
    if (displayNumber > 9999)
      displayNumber = 0;
    updateDigits(displayNumber);
    vTaskDelay(pdMS_TO_TICKS(6600));
  }
}

void updateDigits(uint16_t number) {
  digits[0] = number % 10;
  digits[1] = (number / 10) % 10;
  digits[2] = (number / 100) % 10;
  digits[3] = (number / 1000) % 10;
}

void displayDigit(uint8_t number) {
  uint8_t pattern = numberPatterns[number];

  HAL_GPIO_WritePin(SEG_PORT, SEG_A_PIN,
                    (pattern & 0x01) ? SEGMENT_ON : SEGMENT_OFF);
  HAL_GPIO_WritePin(SEG_PORT, SEG_B_PIN,
                    (pattern & 0x02) ? SEGMENT_ON : SEGMENT_OFF);
  HAL_GPIO_WritePin(SEG_PORT, SEG_C_PIN,
                    (pattern & 0x04) ? SEGMENT_ON : SEGMENT_OFF);
  HAL_GPIO_WritePin(SEG_PORT, SEG_D_PIN,
                    (pattern & 0x08) ? SEGMENT_ON : SEGMENT_OFF);
  HAL_GPIO_WritePin(SEG_PORT, SEG_E_PIN,
                    (pattern & 0x10) ? SEGMENT_ON : SEGMENT_OFF);
  HAL_GPIO_WritePin(SEG_PORT, SEG_F_PIN,
                    (pattern & 0x20) ? SEGMENT_ON : SEGMENT_OFF);
  HAL_GPIO_WritePin(SEG_PORT, SEG_G_PIN,
                    (pattern & 0x40) ? SEGMENT_ON : SEGMENT_OFF);
  HAL_GPIO_WritePin(SEG_PORT, SEG_DP_PIN,
                    (pattern & 0x80) ? SEGMENT_ON : SEGMENT_OFF);
}
