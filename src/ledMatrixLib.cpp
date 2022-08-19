#include "ledMatrixLib.h"

LedMatrix ledMatrix;

void(LedMatrix::*curLedEffect)() = &LedMatrix::emptyEffect;

//Инициализация адресной ленты светодиодов (в виде матрицы)
void LedMatrix::matrixinit() {
    // Установка максимального напряжения и тока для ленты
    FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
    FastLED.setBrightness(baseBrightness); // Яркость: [0, 255]

    // Добавление указателя на массив светодиодов и настроек ленты
    // Лента типа WS2812, с пином данных DATA_PIN, порядком следования цветом GRB
    // Указатель на массив светодиодов ledMatrix.leds, кол-во светодиодов NUM_LEDS
    FastLED.addLeds< WS2812, DATA_PIN, GRB>(ledMatrix.leds, NUM_LEDS);

    // Гасим все светодиоды
    ledMatrix.setAllOneColor(CRGB::Black);
    FastLED.show();
    delay(500);
    FastLED.clear();
    FastLED.show();

    curLedEffect = &LedMatrix::emptyEffect;
    delay( 1000 );
}

/** Установка цвета (цветового тона по модели HSV) конкретному пикселю 
* @param x: координата пикселя по горизонтали [0, MATRIX_WIDTH]
* @param y: координата пикселя по вертикали [0, MATRIX_HEIGHT]
* @param hue: цвет (цветовой тон) - число в дипазано [0, 255]
* @return 
* @note   Значения некоторых цветов (параметр hue): 
*         hue = 0 - красный, 32 - оранжевый, 64 - жёлтый, 96 - зелёный, 128 - голубой,
*         160 - синий, 192 - фиолетовый, 222 - розовый, 255 - красный. 
*/
void LedMatrix::setPixelXY(uint8_t x, uint8_t y, uint8_t hue) {
    // Проверка допустимого диапазона
    x = (x >= MATRIX_WIDTH ? MATRIX_WIDTH-1 : x);
    y = (y >= MATRIX_HEIGHT ? MATRIX_HEIGHT-1 : y);

    // Установка цветового тона (цвета)
    leds[(x % 2 == 0 ? x*MATRIX_HEIGHT + y : (x+1)*MATRIX_HEIGHT-1-y)].setHue(hue);
}

/** Установка цвета (по модели RGB) конкретному пикселю 
* @param x: координата пикселя по горизонтали [0, MATRIX_WIDTH]
* @param y: координата пикселя по вертикали [0, MATRIX_HEIGHT]
* @param color: цвет - объект класса CRGB или перечисления HTMLColorCode.              
* @return 
* @note  Значения для color указывается либо как конкретный цвет из перечисления: CRGB::Red, 
*        либо вызывается один из конструкторов класса CRGB:
*        CRGB(red, green, blue), например: CRGB(106,153,85) - цвет комментариев в VScode,
*        CRGB(colorcode), например: CRGB(0x6A9955)
*/
void LedMatrix::setPixelXY(uint8_t x, uint8_t y, const CRGB::HTMLColorCode& color) {
    // Проверка допустимого диапазона
    x = (x >= MATRIX_WIDTH ? MATRIX_WIDTH-1 : x);
    y = (y >= MATRIX_HEIGHT ? MATRIX_HEIGHT-1 : y);

    // Установка цветового тона (цвета)
    leds[(x % 2 == 0 ? x*MATRIX_HEIGHT + y : (x+1)*MATRIX_HEIGHT-1-y)] = color;
}

/** Функция получения цвета пикселя по его номеру 
 * @param x: координата пикселя по горизонтали [0, MATRIX_WIDTH]
 * @param y: координата пикселя по вертикали [0, MATRIX_HEIGHT]
 * @return Цветовой код в шестнадцетеричном виде: #rrggbb
*/
uint32_t LedMatrix::getPixXYColor(uint8_t x, uint8_t y) {
    // Проверка допустимого диапазона
    if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return 0;

    // Получаем координату пикселя и цвет
    int coord = (x % 2 == 0 ? x*MATRIX_HEIGHT + y : (x+1)*MATRIX_HEIGHT-1-y);
    return (((uint32_t)leds[coord].r << 16) | ((long)leds[coord].g << 8 ) | (long)leds[coord].b);
}

/** Установка цвета (по модели RGB) всем пикселям
* @param  color: цвет - объект класса CRGB или перечисления HTMLColorCode.              
* @return 
* @note   Значения для color указывается либо как конкретный цвет из перечисления: CRGB::Red, 
*         либо вызывается один из конструкторов класса CRGB:
*         CRGB(red, green, blue), например: CRGB(106,153,85) - цвет комментариев в VScode,
*         CRGB(colorcode), например: CRGB(0x6A9955)
*/
void LedMatrix::setAllOneColor(const CRGB::HTMLColorCode& color) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        leds[i] = color;
    }
}

/** Установка цвета (по модели RGB) всем пикселям
* @param  color: цвет - объект класса CRGB или перечисления HTMLColorCode.              
* @return 
* @note   Значения для color указывается либо как конкретный цвет из перечисления: CRGB::Red, 
*         либо вызывается один из конструкторов класса CRGB:
*         CRGB(red, green, blue), например: CRGB(106,153,85) - цвет комментариев в VScode,
*         CRGB(colorcode), например: CRGB(0x6A9955)
*/
void LedMatrix::setAllOneColor(CRGB color) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        leds[i] = color;
    }
}

void LedMatrix::standartFlashing() {
    static uint8_t curBrightness = 0, direct = 0;
    if(curBrightness == 0) {
        uint8_t randColor = random8(1, 255);  // Устанавливаем случайный цвет
        std::fill(leds, leds+NUM_LEDS,randColor);    // Заполняем массив этим цветом
    }
    // Меняем направление изменения яркости при достижении границ
    direct = (curBrightness == 255 || curBrightness == 0 ? ~direct : direct);

    // Установка яркости
    FastLED.setBrightness(curBrightness);
    FastLED.show();
    curBrightness = (direct == 0 ? curBrightness+1 : curBrightness-1);
    delay(500);
}

void LedMatrix::confetti2() {
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy( leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV( rgb2hsv_approximate(baseColor).hue  + random8(64), 200, 255);
}

void LedMatrix::cylonfill(){
    static int x = 1;

    // First slide the led in one direction
    for(int i = 0; i < NUM_LEDS-x; i++) {
        // Set the i'th led to red 
        leds[i] = CRGB::Blue;
        // Show the leds
        FastLED.show();
        // now that we've shown the leds, reset the i'th led to black
        leds[i] = CRGB::Green;
        // Wait a little bit before we loop around and do it again
        delay(16);
    }
          
    leds[NUM_LEDS -x] = CRGB::Orange;
    FastLED.show();
    x++;
    if (x ==NUM_LEDS){
        x =1;
    }
    Serial.println(x);
    // Now go in the other direction.  
    for(int i = NUM_LEDS-x; i >= 2; i--) {
       // Set the i'th led to red 
       leds[i] = CRGB::Purple;
       // Show the leds
       FastLED.show();
       // now that we've shown the leds, reset the i'th led to black
       leds[i] = CRGB::Black;
       // Wait a little bit before we loop around and do it again
       //  delay(1);
    }
}

void LedMatrix::emptyEffect() { }