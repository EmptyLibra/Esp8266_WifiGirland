#include <Arduino.h>
#include "FastLED.h"

/*=========================== Макросы ===========================================*/
#define NUM_LEDS     50      // Кол-во светодиодов в ленте 
#define LED_TYPE     WS2812  // Тип светодиодной ленты
#define COLOR_ORDER  GRB     // 
#define DATA_PIN     D5      // Пин данных
#define VOLTS        5       // Напряжение питания
#define MAX_MA       4000    // Максимальный ток в мА

/*=========================== Переменные ===========================================*/
CRGB leds[NUM_LEDS];

/*=========================== Функции ===========================================*/
void setup() {
  delay( 3000 ); //safety startup delay
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
  FastLED.addLeds< WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(20);

  // for (int i = 0; i < NUM_LEDS; i++) {
  //   leds[i].setHue(i * 255 / NUM_LEDS);
  // }
  // FastLED.show();
}

byte counter;
void loop() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setHue(counter + i * 255 / NUM_LEDS);
  }
  counter++;        // counter меняется от 0 до 255 (тип данных byte)
  FastLED.show();
  delay(30);        // скорость движения радуги
}
