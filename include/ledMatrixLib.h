#ifndef __LED_MATRIX_LIB_H
#define __LED_MATRIX_LIB_H

#include "FastLED.h"

/*=========================== Макросы ===========================================*/
//----- Настройки Ленты ------
#define NUM_LEDS     48      // Кол-во светодиодов в ленте 
#define LED_TYPE     WS2812  // Тип светодиодной ленты
#define COLOR_ORDER  GRB     // 
#define DATA_PIN     D5      // Пин данных
#define VOLTS        5       // Напряжение питания
#define MAX_MA       4000    // Максимальный ток в мА

//----- Настройки матрицы ------
#define MATRIX_WIDTH 4      // Длина матрицы (кол-во пикселей по горизонтали)
#define MATRIX_HEIGHT 12     // Ширина матрицы (кол-во пикселей по вертикали)

/*=========================== Переменные ===========================================*/
// Объявление класса для матрицы из светодиодов
class LedMatrix {
public:
    CRGB leds[NUM_LEDS];            // Массив, содержащий все пиксели матрицы

    void setPixelXY(uint8_t x, uint8_t y, uint8_t hue);
    void setPixelXY(uint8_t x, uint8_t y, const CRGB& color);
    void setAllOneColor(uint8_t hue);
    void setAllOneColor(const CRGB::HTMLColorCode& color);
};

#endif