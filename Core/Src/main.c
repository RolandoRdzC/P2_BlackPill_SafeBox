#include "main.h"
#include "portmacro.h"
#include "stm32f4xx_hal_def.h"
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
const uint8_t correctPassword[4] = {1, 2, 3, 4};

const uint8_t letterPatterns[] = {
    ~0x77, // A  (0b01110111: a b c e f g)
    ~0x7C, // b  (0b01111100: b c d e f)
    ~0x39, // C  (0b00111001: a d e f)
    ~0x5E, // d  (0b01011110: b c d e g)
    ~0x79, // E  (0b01111001: a d e f g)
    ~0x71, // F  (0b01110001: a e f g)
    ~0x7D, // G  (0b01111101: a c d e f g)
    ~0x74, // H  (0b01110100: b c e f g)
    ~0x06, // I  (0b00000110: b c)
    ~0x1E, // J  (0b00011110: b c d e)
    ~0x70, // k  (0b01110000: e f g)
    ~0x38, // L  (0b00111000: d e f)
    ~0x37, // M  (0b00110111: a b c e f)
    ~0x54, // n  (0b01010100: c e g)
    ~0x3F, // O  (0b00111111: a b c d e f)
    ~0x73, // P  (0b01110011: a b e f g)
    ~0x67, // q  (0b01100111: a b c f g)
    ~0x50, // r  (0b01010000: e g)
    ~0x6D, // S  (0b01101101: a c d f g)
    ~0x78, // t  (0b01111000: d e f g)
    ~0x3E, // U  (0b00111110: b c d e f)
    ~0x3E, // V  (0b00111110: b c d e f)
    ~0x7E, // W  (0b01111110: b c d e f g)
    ~0x76, // X  (0b01110110: b c e f g)
    ~0x6E, // Y  (0b01101110: b c d f g)
    ~0x5B  // Z  (0b01011011: a d e g)
};

volatile uint32_t displayNumber = 0; 
volatile uint8_t digits[4] = {0};
volatile uint8_t column = 0;
volatile uint8_t dataBuffer[20] = {0};  
volatile uint8_t dataIndex = 0;      
SemaphoreHandle_t xSem;

int main(void) {
  System_Init();
  xSem = xSemaphoreCreateBinary();


  xTaskCreate(DisplayTask, "Display", 128, NULL, 2, NULL);
  //xTaskCreate(CounterTask, "Counter", 128, NULL, 1, NULL);
  xTaskCreate(KeyboardTask, "Keyboard", 128, NULL, 1, NULL);
  xTaskCreate(OpenTask, "Door", 128, NULL, 1, NULL);
  vTaskStartScheduler();
}

void DisplayTask(void *pvParameters) {
  UNUSED(pvParameters);
  uint8_t currentDigit = 0;

  while (1) {
    HAL_GPIO_WritePin(DIGIT_PORT,
                      DIGIT_1_PIN | DIGIT_2_PIN | DIGIT_3_PIN | DIGIT_4_PIN,
                      DIGIT_OFF);
    displayDigit(digits[currentDigit], digits[currentDigit] > 15);  // isLetter es true si el valor > 15
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
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void KeyboardTask(void *pvParameters __attribute__((unused))) {
  uint16_t columnPin;
  uint8_t currentColumn = 0;
  uint16_t rowPins[] = {ROW_1_PIN, ROW_2_PIN, ROW_3_PIN, ROW_4_PIN};
  
  while (1) {
    HAL_GPIO_WritePin(COLUMN_PORT,
                      COLUMN_1_PIN | COLUMN_2_PIN | COLUMN_3_PIN | COLUMN_4_PIN,
                      DIGIT_OFF);
    switch (currentColumn) {
    case 0:
      columnPin = COLUMN_1_PIN;
      break;
    case 1:
      columnPin = COLUMN_2_PIN;
      break;
    case 2:
      columnPin = COLUMN_3_PIN;
      break;
    case 3:
      columnPin = COLUMN_4_PIN;
      break;
    }
    HAL_GPIO_WritePin(COLUMN_PORT, columnPin, DIGIT_ON);
    vTaskDelay(pdMS_TO_TICKS(1));
    for (int row = 0; row < 4; row++) {
      if (HAL_GPIO_ReadPin(ROW_PORT, rowPins[row]) == GPIO_PIN_SET) {
        char pressedKey = keymap[row][currentColumn];
        if (pressedKey >= '0' && pressedKey <= '9') {
          if (dataIndex < 20) {
            dataBuffer[dataIndex] = pressedKey - '0';
            dataIndex++;
            displayNumber = 0;
            int start = (dataIndex > 4) ? (dataIndex - 4) : 0;
            for (int i = start; i < dataIndex; i++) {
              displayNumber = displayNumber * 10 + dataBuffer[i];
            }
            updateDigits(displayNumber);
          }
        }
        else if (pressedKey == 'A') {
          if (dataIndex > 0) {
            dataIndex--;
            dataBuffer[dataIndex] = 0;
            displayNumber = 0;
            int start = (dataIndex > 4) ? (dataIndex - 4) : 0;
            for (int i = start; i < dataIndex; i++) {
              displayNumber = displayNumber * 10 + dataBuffer[i];
            }
            updateDigits(displayNumber);
          }
        }
        else if (pressedKey == 'B') {
          //checkPassword();
          xSemaphoreGive(xSem);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
        while (HAL_GPIO_ReadPin(ROW_PORT, rowPins[row]) == GPIO_PIN_SET) {
          vTaskDelay(pdMS_TO_TICKS(10));
        }
      }
    }
    currentColumn = (currentColumn + 1) % 4;
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void OpenTask(void *pvParameters) {
    UNUSED(pvParameters);
    while(1){
      xSemaphoreTake(xSem, portMAX_DELAY);
      uint8_t isCorrect = 1;
      if (dataIndex >= 4) {
          int start = dataIndex - 4;
          for(int i = 0; i < 4; i++) {
              if(dataBuffer[start + i] != correctPassword[i]) {
                  isCorrect = 0;
                  break;
              }
          }
      } else {
          isCorrect = 0; 
      }
      
      if(isCorrect) {
        displayText("OPEN");
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);  
        vTaskDelay(pdMS_TO_TICKS(10000)); 
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); 
      } else {
          displayText("FAIL");
          vTaskDelay(pdMS_TO_TICKS(10000)); 
      }
      dataIndex = 0;
      displayNumber = 0;
      for(int i = 0; i < 20; i++) {
          dataBuffer[i] = 0;
      }
      updateDigits(displayNumber);
    }
}

void updateDigits(uint32_t number) {
  digits[0] = number % 10;
  digits[1] = (number / 10) % 10;
  digits[2] = (number / 100) % 10;
  digits[3] = (number / 1000) % 10;
}

void displayText(const char* text) {
    int len = 0;
    while(text[len] != '\0' && len < 4) len++;
    for(int i = 0; i < 4; i++) {
        int displayPos = 3 - i; 
        if (i < len && text[i] >= 'A' && text[i] <= 'Z') {
            digits[displayPos] = (text[i] - 'A') + 16;
        } else {
            digits[displayPos] = 255; 
        }
    }
}

void displayDigit(uint8_t value, uint8_t isLetter) {
    uint8_t pattern;
    if (isLetter) {
        // Si es una letra, restamos 16 para obtener el índice correcto en letterPatterns
        uint8_t letterIndex = value - 16;
        if (letterIndex < 26) {
            pattern = letterPatterns[letterIndex];
        } else {
            pattern = 0xFF; // Segmentos apagados si el índice es inválido
        }
    } else if (value < 10) {
        pattern = numberPatterns[value];
    } else {
        pattern = 0xFF; // Segmentos apagados para valores fuera de rango
    }

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
