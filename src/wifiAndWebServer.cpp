
/*=========================== Подключение библиотек =================================*/
#include "wifiAndWebServer.h"

/*=========================== Переменные ===========================================*/
// Установка статических ip, шлюза и маски, 
IPAddress ip(192, 168, 1 , 17);      // Ip устройства или точки доступа
IPAddress gateway(192, 168, 1, 254); // Шлюз подключения
IPAddress subnet(255, 255, 255, 0);  // Маска подсети
ESP8266WebServer server(80);           // Создание экземпляра web-сервера с портом для прослушивания

/*=========================== Функции ===========================================*/

extern void EEPROM_SaveAllConfig();

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

/** @brief  Обработчик несуществующих запросов 
  * @param  None
  * @return None */
static void handleNotFound() {
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
static void handleEffects() {
    String status = server.arg("ef");
    if (status == "mode1") {
        curLedEffect = &LedMatrix::emptyEffect;
        ledMatrix.setAllOneColor(CRGB::White);
        FastLED.setBrightness(255);
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

/** @brief  Обработчик запросов на утановку цвета: http://192.168.1.17/setColor?color={value}
  * @param  None
  * @return None */
static void handlerSetColor() {
    // Получаем присланный цветовой код
    String status = server.arg("color");
    uint32_t colorHex;
    sscanf(status.c_str(), "%x", &colorHex);

    // Установка базового цвета и яркости
    ledMatrix.baseColor = CRGB(colorHex);
    ledMatrix.baseBrightness = rgb2hsv_approximate(colorHex).value;

    if(curLedEffect == &LedMatrix::emptyEffect) {
        ledMatrix.setAllOneColor(CRGB(colorHex));
        FastLED.setBrightness(ledMatrix.baseBrightness);
        FastLED.show();
    }
    
    server.send(200, "text/plain", String(colorHex, HEX));
    Serial.printf("answer= %s\n", String(colorHex, HEX).c_str());
}

/** @brief  Обработчик запросов на отдельные команды (какой-то из команд может и не быть): 
  *         http://192.168.1.17/cmd?getState={1}&setState={0|1}&saveAllConfig={1}&getBaseColor={1}
  * @param  None
  * @return None */
static void handlerCmd() {
    String argStatus, argName;
    String answer = "";
    for(int i = 0; i < server.args(); i++) {
        argStatus = server.arg(i);
        argName = server.argName(i);

        // Проверка наличия команды на получение состояния
        if(argName == "getState" && argStatus == "1") {
            answer += "state=" + String(ledMatrix.state ? "1" : "0") + ";";
            continue;
        }

        // Проверка наличия команды на установку состояния
        if(argName == "setState" && (argStatus == "1" || argStatus == "0")) {
            if(argStatus == "0") FastLED.clear();

            ledMatrix.state = argStatus.toInt();
            answer += "Set state success. State = " + String(ledMatrix.state) + ";";
            continue;
        }

        // Проверка наличия команды сохранения всех настроек в EEPROM
        if(argName == "saveAllConfig" && argStatus == "1") {
            EEPROM_SaveAllConfig();
            answer += "Save All config...;";
            continue;
        }

        // Проверка наличия команды запроса базового цвета
        if(argName == "getBaseColor" && argStatus == "1") {
            uint32_t color = ((uint32_t)ledMatrix.baseColor.r << 16) + 
                             ((uint32_t)ledMatrix.baseColor.g <<  8) + 
                             (uint32_t)ledMatrix.baseColor.b;
            answer += "color=" + String(color, 16);
            continue;
        }
    }

    server.send(200, "text/plain", answer.c_str());
    Serial.printf("answer= %s\n", answer.c_str());
}

/** @brief  Инициализирует http (web) сервер и назначает обработчики запросов
  * @param  None
  * @return None */
void httpServerInit() {
    server.onNotFound(handleNotFound);           // Обработчик несуществующих запросов
    server.on("/effect", handleEffects);         // Обработчик запросов на выбор эффекта
    server.on("/setBaseColor", handlerSetColor); // Установка базового цвета
    server.on("/cmd", handlerCmd);               // Обработка кратких команд
    server.begin();                              // Запуск Web-сервера

    // Записываем IP адрес
    Serial.print("Server started\n"); Serial.println(WiFi.localIP());
}