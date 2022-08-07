/* Примечания к проекту:
*  1. Библиотека FastLed генерирует кучу предупреждений. 
*     Чтобы их убрать, надо в файл c_cpp_properties.json в раздел "compilerArgs" 
*     добавить три следующих флага: -Wmisleading-indentation", -Wdeprecated-declarations, -Wregister.
*/

#include <Arduino.h>       // Ардуино библиотека для управления платой
#include <ESP8266WiFi.h>   // Библиотека для управления по Wi-fi
#include "FastLED.h"       // Библиотека для обращения к ленте
#include "ledMatrixLib.h"  // Моя библиотека для управления матрицей

// Wi-fi настройки
#define SSID "MGTS_GPON_7576" // Адрес Wi-fi сети
#define PASSWORD "USHUJ83X"   // Пароль от Wi-fi сети


/*=========================== Переменные ===========================================*/
// Создание экзмепляра класса матрицы из светодиодов
LedMatrix ledMatrix;

//---------- Настройки Wi-fi -----------
// Установка статических ip, шлюза и маски, 
IPAddress ip(192, 168, 1 , 17);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
WiFiServer server(80);               // Создание экземпляра web-сервера с портом для прослушивания

// Класс
WiFiClient client;

/*=========================== Функции ===========================================*/
//Инициализация адресной ленты светодиодов (в виде матрицы)
void ledMatrixInit() {
  // Установка максимального напряжения и тока для ленты
  FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
  FastLED.setBrightness(200); // Яркость: [0, 255]

  // Добавление указателя на массив светодиодов и настроек ленты
  // Лента типа WS2812, с пином данных DATA_PIN, порядком следования цветом GRB
  // Указатель на массив светодиодов ledMatrix.leds, кол-во светодиодов NUM_LEDS
  FastLED.addLeds< WS2812, DATA_PIN, GRB>(ledMatrix.leds, NUM_LEDS);

  // Гасим все светодиоды
  ledMatrix.setAllOneColor(CRGB::Black);
  FastLED.show();
  delay( 1000 );
}

// Инициализация wifi, подключение к домашнему WiFi в режиме станции (STA)
void wifiStaInit() {
  // Подключаемся к сети wi-fi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  /* Использовать следующие функции, если не работает Wi-Fi:
  *  WiFi.setOutputPower(0); // Задаёт максимальную мощность передачи (без неё одна из плат кидала Exception(0))
  *  WiFi.persistent(false); // Записывать ssid, password во Flash только, если они изменились
  *  WiFi.mode(WIFI_OFF);    // Отключает WiFi
  *  delay(2000);            // Задержка на 2 сек
  *  WiFi.mode(WIFI_STA);    // Устнавливаем режим станции (Station)
  *  WiFi.disconnect();      // Отключить соединение Wi-fi
  *  WiFi.config(ip, gateway, subnet);  // Установить статический ip, шлюз и маску
  *  ESP.eraseConfig();      // Сбрасывает насройки esp
  *  WiFi.begin(ssid, password);        // Запуск сетевого соединения
  */
  WiFi.persistent(false);            // Записывать ssid, password во Flash только, если они изменились
  WiFi.mode(WIFI_STA);               // Устнавливаем режим станции (Station)
  WiFi.config(ip, gateway, subnet);  // Установить статический ip, шлюз и маску
  WiFi.begin(SSID, PASSWORD);        // Запуск сетевого соединения
  
  // Зажечь первый пиксель жёлтым цветом
  ledMatrix.setPixelXY(0,0, 60);  
  FastLED.show();

  // Ждём, пока плата не подключится к wifi
  int tries = 0, status = 0;
  while(status != WL_CONNECTED) {
    status = WiFi.status();
    Serial.print(tries++); Serial.print(". "); Serial.println(status);
    delay(1000);
  }

  // Сообщение об успешном подключении и зажигание зелёного светодиода
  Serial.println( (WiFi.status() != WL_CONNECTED ? "\nWi-fi NOT connected" : "\nWiFi connected"));
  ledMatrix.setPixelXY(0,0, CRGB::Green);
  FastLED.show();
  delay(1000);
}

// Функция инициаилизации
void setup() {
  //----- Инициализация пинов МК ------
  delay( 3000 );        // Защитная задержка в 3 секунды
  Serial.begin(115200); // Инициализация терминала (для дебага)

  pinMode(LED_BUILTIN, OUTPUT); // Инициализация пина, отвечающего за светодиод на плате WemosD1  
  digitalWrite(LED_BUILTIN, LOW); // Зажигаем его (его управление инвертировано, LOW - светодиод горит)

  // Инициализация матрицы из адресных светодиодов
  ledMatrixInit();

  // Инициализация WiFi
  wifiStaInit(); 
  
  // Запуск Web-сервера
  server.begin();
  Serial.println("Server started");

  // Записываем IP адрес
  Serial.println(WiFi.localIP());
  delay(500);
}

// Развернуть всё: ctrl + k + j
// Свернуть всё: ctrl + k + 0

void controller(String req,  WiFiClient client) {
  if(req.indexOf("/mode1") != -1){
    ledMatrix.setAllOneColor(CRGB::Azure);
    FastLED.show();

  } else if(req.indexOf("/mode2") != -1){
    FastLED.clear();
    FastLED.show();

  } else if(req.indexOf("/mode3") != -1){
    ledMatrix.setAllOneColor(CRGB::Cyan);
    FastLED.show();

  } else {
    Serial.println("invalid request");
    client.stop();
    return;
  }
}


void loop() {
  // for(uint8_t row = 0; row < MATRIX_HEIGHT; row++) {
  //   for(uint8_t col = 0; col < MATRIX_WIDTH; col++) {
  //     ledMatrix.setPixelXY(col, row, color);
  //     FastLED.show();
  //     delay(30);
  //   }
  //   row++;
  //   for(int col = MATRIX_WIDTH-1; col >= 0; col--) {
  //     ledMatrix.setPixelXY(col, row, color);
  //     FastLED.show();
  //     delay(30);
  //   }
  // }
  // color += 30;
  // for(int row = MATRIX_HEIGHT-1; row >= 0; row--) {
  //   for(uint8_t col = 0; col < MATRIX_WIDTH; col++) {
  //     ledMatrix.setPixelXY(col, row, color);
  //     FastLED.show();
  //     delay(30);
  //   }
  //   row--;
  //   for(int col = MATRIX_WIDTH-1; col >= 0; col--) {
  //     ledMatrix.setPixelXY(col, row, color);
  //     FastLED.show();
  //     delay(30);
  //   }
  // }
  // color += 30;

  // Мигаем первым светодиодом
  // ledMatrix.setPixelXY(0, 1, (ledMatrix.getPixXYColor(0, 1) == CRGB::LightYellow ? CRGB::Black : CRGB::LightYellow));
  FastLED.show();

  // Подключение нового клиента
  client = server.available();
  if (!client) { return; }

  // Ждём, пока клиент что-нибудь не отправит
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  // Читаем первую строку запроса
  String req = client.readStringUntil('\r');
  Serial.println(req);

  // Совпадение с запросом
  controller(req, client);
  client.flush(); 

  // Подготавливаем ответ
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "anwer";

  // Отправка запроса клиенту
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
}
