#include <Arduino.h>       // Ардуино библиотека для управления платой
#define register  
#include "FastLED.h"       // Библиотека для обращения к ленте

#include "ledMatrixLib.h"  // Моя библиотека для управления матрицей

/* Примечания к проекту:
*  1. Библиотека FastLed генерирует кучу предупреждений. 
*     Чтобы их убрать, надо в файл c_cpp_properties.json в раздел "compilerArgs" 
*     добавить три следующих флага: -Wmisleading-indentation", -Wdeprecated-declarations, -Wregister.
*/

/*=========================== Переменные ===========================================*/
LedMatrix ledMatrix;

/*=========================== Функции ===========================================*/
// Функция инициаилизации
void setup() {
  //----- Инициализация пинов МК ------
  // Защитная задержка в 3 секунды
  delay( 3000 );

  // Инициализация пина, отвечающего за светодиод на плате WemosD1
  pinMode(LED_BUILTIN, OUTPUT);

  // Зажигаем его (его управление инвертировано, LOW - светодиод горит)
  digitalWrite(LED_BUILTIN, LOW);

  //----- Инициализация матрицы из адресных светодиодов -----
  // Установка максимального напряжения и тока для ленты
  FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);

  /* Добавление указателя на массив светодиодов и настроек ленты
   * WS2812 - тип ленты, 
   * DATA_PIN - пин отправки данных, 
   * GRB - порядок следования цветов,
   * ledMatrix.leds - указатель на массив светодиодов, 
   * NUM_LEDS - всего светодиодов */
  FastLED.addLeds< WS2812, DATA_PIN, GRB>(ledMatrix.leds, NUM_LEDS);

  // Установка яркости и цвета всей ленты при включении
  FastLED.setBrightness(200); // Яркость: [0, 255]
  ledMatrix.setAllOneColor(CRGB::Bisque); // Цвет всей ленты
  FastLED.show(); // Отобразить все светодиоды
}

// Развернуть всё: ctrl + k + j
// Свернуть всё: ctrl + k + 0
// Доработать функцию setPixelXY() !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

byte counter;
void loop() {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    ledMatrix.setAllOneColor(counter + i * 255 / NUM_LEDS);
  }
  counter+=10;        // counter меняется от 0 до 255 (тип данных byte)
  FastLED.show();
  delay(500);        // скорость движения радуги
}
