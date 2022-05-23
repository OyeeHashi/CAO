#include <Arduino.h>
#ifdef ESP32
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define UP_LEFT 5
#define UP_RIGHT 6
#define DOWN_LEFT 7
#define DOWN_RIGHT 8
#define TURN_LEFT 9
#define TURN_RIGHT 10
#define STOP 0

const char* ssid     = "MyWiFiCar";
const char* password = "12345678";
