/* Примечания к проекту:
*  1. Библиотека FastLed генерирует кучу предупреждений. 
*     Чтобы их убрать, надо в файл c_cpp_properties.json в раздел "compilerArgs" 
*     добавить три следующих флага: -Wmisleading-indentation", -Wdeprecated-declarations, -Wregister или
*     надо изменить версию c++ на c++11 в c_pp_properties.json
*/

// Развернуть всё: ctrl + k + j
// Свернуть всё: ctrl + k + 0
// Сдвинуть блок кода: выделить код, зажать ctrl+shift+alt + стрелку влево или вправо, 
// затем отпустить и стрелками двигать код

/*=========================== Подключение библиотек =================================*/
#include "wifiAndWebServer.h" // Библиотека для настройки Wifi и web сервера

/*=========================== Переменные ===========================================*/

/*=========================== Функции ===========================================*/

/** @brief  Сохранение всех настроек в EEPROM
  * @param  None
  * @return None */
void EEPROM_SaveAllConfig() {
    // Сохранение значения в память
    EEPROM.begin(EEPROM_SIZE);      // Инициализируем память
    EEPROM.put(0, ledMatrix.state); // Записываем состояние в буфер (1 байт)
    EEPROM.put(1, ledMatrix.baseBrightness);  // Записываем яркость в буфер (1 байт)
    uint32_t colorCode = ((uint32_t)ledMatrix.baseColor.r << 16) + 
                         ((uint32_t)ledMatrix.baseColor.g << 8) + 
                         (uint32_t)ledMatrix.baseColor.b;
    Serial.printf("Save color code: %x\n", colorCode);

    EEPROM.put(2, colorCode);       // Записываем цвет в буфер (4 байта)
    EEPROM.commit();                // Записываем данные в память
    EEPROM.end();
}

/** @brief  Получение всех настроек из EEPROM
  * @param  None
  * @return None */
void EPROM_GetAllConfig() {
    // Сохранение значения в память
    EEPROM.begin(EEPROM_SIZE);                // Инициализируем память
    EEPROM.get(0, ledMatrix.state);           // Считываем состояние (1 байт)
    EEPROM.get(1, ledMatrix.baseBrightness);  // Считываем яркость (1 байт)

    uint32_t colorCode;
    EEPROM.get(2, colorCode);                  // Считываем цвет (4 байта)
    Serial.printf("Get color code: %x\n", colorCode);
    ledMatrix.baseColor = CRGB(colorCode);

    EEPROM.end();
}

// Функция инициаилизации
void setup() {
    //----- Инициализация пинов МК ------
    delay( 3000 );                // Защитная задержка в 3 секунды
    random16_set_seed(millis());  // Начальное значение для случайных чисел
    Serial.begin(115200);         // Инициализация терминала (для дебага)
    EPROM_GetAllConfig();         // Считываем данные из EEPROM

    pinMode(LED_BUILTIN, OUTPUT); // Инициализация пина, отвечающего за светодиод на плате WemosD1  
    digitalWrite(LED_BUILTIN, LOW); // Зажигаем его (его управление инвертировано, LOW - светодиод горит)

    ledMatrix.matrixinit();  // Инициализация матрицы из адресных светодиодов
    wifiInit();              // Инициализация WiFi
    httpServerInit();        // Инициализация HTTP (Web) сервера
}

void loop() {
    server.handleClient();
    delay(1);

    // Выполнение очередного эффекта ленты
    (ledMatrix.*curLedEffect)();
    FastLED.show();
}

