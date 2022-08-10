
/*=========================== Подключение библиотек =================================*/
#include "wifiAndWebServer.h"

/*=========================== Переменные ===========================================*/
// Установка статических ip, шлюза и маски, 
IPAddress ip(192, 168, 1 , 17);      // Ip устройства или точки доступа
IPAddress gateway(192, 168, 1, 254); // Шлюз подключения
IPAddress subnet(255, 255, 255, 0);  // Маска подсети
ESP8266WebServer server(80);           // Создание экземпляра web-сервера с портом для прослушивания

/*=========================== Функции ===========================================*/

static void handleNotFound();
static void handleEffects();

/** @brief  Инициализирует wifi-подключение к домашнему WiFi в режиме станции (STA)
  *         или создает точку доступа
  * @param  None
  * @return None */
void wifiInit() {
    // Подключаемся к сети wi-fi
    Serial.printf("\n\nConnecting to %s\n", SSID_STA);

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
    WiFi.begin(SSID_STA, PASSWORD_STA);        // Запуск сетевого соединения
  
    // Зажечь первый пиксель жёлтым цветом
    ledMatrix.setPixelXY(0,0, 60);  
    FastLED.show();

    // Ждём, пока плата не подключится к wifi (20 попыток)
    int tries = 0, status = 0;
    while(status != WL_CONNECTED && tries < 20) {
        status = WiFi.status();
        Serial.printf("%u. %u\n", tries++, status);
        delay(1000);
    }

    // Если к Wi-fi так и не подключился, то запускаем в режиме точки доступа
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-fi NOT connecting");
        Serial.println("Run as access point...");

        WiFi.disconnect();                  // Отключаем WIFI
        WiFi.mode(WIFI_AP);                 // Меняем режим на режим точки доступа
        WiFi.softAPConfig(ip, ip, subnet);  // Задаем настройки сети
        // Включаем WIFI в режиме точки доступа с именем и паролем
        WiFi.softAP(SSID_AP, PASSWORD_AP);  

        Serial.println("Access point was started!");
        Serial.printf("Wi-fi: %s\n", SSID_AP);
        Serial.printf("Password: %s\n", PASSWORD_AP);

        ledMatrix.setPixelXY(0,0, CRGB::Orange);  
        FastLED.show();
    } else {
        // Сообщение об успешном подключении и зажигание зелёного светодиода
        Serial.println("WiFi connected !");
        
        // Зажигаем зелёный светодиод
        ledMatrix.setPixelXY(0,0, CRGB::Green);
        FastLED.show();
    }
    delay(1000);
}

/** @brief  Инициализирует http (web) сервер и назначает обработчики запросов
  * @param  None
  * @return None */
void httpServerInit() {
    server.onNotFound(handleNotFound);    // Обработчик несуществующих запросов
    server.on("/effect", handleEffects);  // Обработчик запросов на выбор эффекта
    server.begin();                       // Запуск Web-сервера

    // Записываем IP адрес
    Serial.print("Server started\n"); Serial.println(WiFi.localIP());
}

/** @brief  Обработчик несуществующих запросов 
  * @param  None
  * @return None */
void handleNotFound() {
    String message = "File Not Found\n\nURI: " + server.uri();
    message += "\nMethod: " + (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: " + server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

/** @brief  Обработчик запросов на выбор эффекта: http://192.168.1.17/effect?ef={переменная}
  * @param  None
  * @return None */
void handleEffects() {
    String status = server.arg("ef");
    if (status == "mode1") {
        curLedEffect = &LedMatrix::emptyEffect;
        ledMatrix.setAllOneColor(CRGB::Azure);
        server.send(200, "text/plain", "Effect Azure");
    }

    if (status == "mode2") {
        curLedEffect = &LedMatrix::cylonfill;
        server.send(200, "text/plain", "Effect clear");
    }

    if (status == "mode3") {
        curLedEffect = &LedMatrix::confetti2;
        server.send(200, "text/plain", "Effect confetti2");
    }
}
//