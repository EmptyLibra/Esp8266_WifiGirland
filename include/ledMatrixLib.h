#ifndef __LED_MATRIX_LIB_H
#define __LED_MATRIX_LIB_H

#include <Arduino.h>           // Ардуино библиотека для управления платой
#include "FastLED.h"  // Библиотека для обращения к ленте

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
private:
public:
    bool state = false;             // Состояние гирлянды (включена, выключена)
    uint8_t baseBrightness = 128;   // Базовая яркость гирлянды
    CRGB baseColor = CRGB::Yellow;  // Базовый цвет гирлянды
    CRGB leds[NUM_LEDS];            // Массив, содержащий все пиксели матрицы

    void matrixinit();

    void setPixelXY(uint8_t x, uint8_t y, uint8_t hue);
    void setPixelXY(uint8_t x, uint8_t y, const CRGB::HTMLColorCode& color);
    uint32_t getPixXYColor(uint8_t x, uint8_t y);
    void setAllOneColor(const CRGB::HTMLColorCode& color);
    void setAllOneColor(CRGB color);

    void standartFlashing();
    void confetti2();
    void cylonfill();
    void emptyEffect();
};
extern LedMatrix ledMatrix;

// Указатель на текущий выбранный эффект ленты (указатель на метод класса)
extern void(LedMatrix::*curLedEffect)();
#endif