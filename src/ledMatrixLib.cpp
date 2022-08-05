#include "ledMatrixLib.h"

/** Установка цвета (цветового тона по модели HSV) конкретному пикселю 
* @param:  x: координата пикселя по горизонтали [0, MATRIX_WIDTH]
* @param:  y: координата пикселя по вертикали [0, MATRIX_HEIGHT]
* @param:  hue: цвет (цветовой тон) - число в дипазано [0, 255]
* @retval: None
* @note:   Значения некоторых цветов (параметр hue): 
*          hue = 0 - красный, 32 - оранжевый, 64 - жёлтый, 96 - зелёный, 128 - голубой,
*          160 - синий, 192 - фиолетовый, 222 - розовый, 255 - красный. 
*/
void LedMatrix::setPixelXY(uint8_t x, uint8_t y, uint8_t hue) {
    // Проверка допустимого диапазона
    x %= MATRIX_WIDTH;
    y %= MATRIX_HEIGHT;

    // Установка цветового тона (цвета)
    leds[y + x*MATRIX_HEIGHT].setHue(hue);
}

/** Установка цвета (по модели RGB) конкретному пикселю 
* @param:  x: координата пикселя по горизонтали [0, MATRIX_WIDTH]
* @param:  y: координата пикселя по вертикали [0, MATRIX_HEIGHT]
* @param:  color: цвет - объект класса CRGB или перечисления HTMLColorCode.              
* @retval: None
* @note:   Значения для color указывается либо как конкретный цвет из перечисления: CRGB::Red, 
*          либо вызывается один из конструкторов класса CRGB:
*          CRGB(red, green, blue), например: CRGB(106,153,85) - цвет комментариев в VScode,
*          CRGB(colorcode), например: CRGB(0x6A9955)
*/
void LedMatrix::setPixelXY(uint8_t x, uint8_t y, const CRGB& color) {
    // Проверка допустимого диапазона
    x %= MATRIX_WIDTH;
    y %= MATRIX_HEIGHT;

    // Установка цветового тона (цвета)
    leds[y + x*MATRIX_HEIGHT] = color;
}

/** Установка цвета (цветового тона по модели HSV) всем пикселям 
* @param:  hue: цвет (цветовой тон) - число в дипазано [0, 255]
* @retval: None
* @note:   Значения некоторых цветов (параметр hue): 
*          hue = 0 - красный, 32 - оранжевый, 64 - жёлтый, 96 - зелёный, 128 - голубой,
*          160 -   синий, 192 - фиолетовый, 222 - розовый, 255 - красный. 
*/
void LedMatrix::setAllOneColor(uint8_t hue) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        leds[i].setHue(hue);
    }
}

/** Установка цвета (по модели RGB) всем пикселям
* @param:  color: цвет - объект класса CRGB или перечисления HTMLColorCode.              
* @retval: None
* @note:   Значения для color указывается либо как конкретный цвет из перечисления: CRGB::Red, 
*          либо вызывается один из конструкторов класса CRGB:
*          CRGB(red, green, blue), например: CRGB(106,153,85) - цвет комментариев в VScode,
*          CRGB(colorcode), например: CRGB(0x6A9955)
*/
void LedMatrix::setAllOneColor(const CRGB::HTMLColorCode& color) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        leds[i] = color;
    }
}