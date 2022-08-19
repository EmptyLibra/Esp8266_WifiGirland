#ifndef __WIFI_AND_WEB_SERVER_H
#define __WIFI_AND_WEB_SERVER_H

/*=========================== Подключение библиотек =================================*/
#include <ESP8266WiFi.h>       // Библиотека для управления по Wi-fi
#include <ESP8266WebServer.h>  // Библиотека для Web сервера
#include <EEPROM.h>            // Библиотека для работы с EEPROM памятью
#include "ledMatrixLib.h"      // Моя библиотека для управления матрицей
#include <string>              // Для sscanf

/*=========================== Макросы ===========================================*/
// Настройки Wi-fi в режиме станции
#define SSID_STA "MGTS_GPON_7576" // Адрес Wi-fi сети
#define PASSWORD_STA "USHUJ83X"   // Пароль от Wi-fi сети

// Настройки Wi-fi  в режиме точки доступа
#define SSID_AP "SMART_LEDS_MATRIX" // Адрес Wi-fi сети
#define PASSWORD_AP "31415926"      // Пароль от Wi-fi сети

// Размер EEPROM памяти в байтах
#define EEPROM_SIZE 6
/*=========================== Переменные ===========================================*/
extern ESP8266WebServer server; // Web сервер

/*=========================== Функции ===========================================*/

void wifiInit();
void httpServerInit();

#endif