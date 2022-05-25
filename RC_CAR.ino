#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

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

#define FRONT_RIGHT_MOTOR 0
#define FRONT_LEFT_MOTOR 1
#define BACK_RIGHT_MOTOR 2
#define BACK_LEFT_MOTOR 3

#define FORWARD 1
#define BACKWARD -1


const char* ssid     = "MyWiFiCar";
const char* password = "12345678";

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP32CORE_WIFI_POINT

#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377

#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 19 bytes
  { 255,2,0,0,0,12,0,16,31,1,5,0,16,40,30,30,2,26,31 };


//   struct {

//     // input variables
//   int8_t joystick_1_x; // from -100 to 100  
//   int8_t joystick_1_y; // from -100 to 100  

//     // other variable
//   uint8_t connect_flag;  // =1 if wire connected, else =0 

// } RemoteXY;



void setup() 
{
  RemoteXY_Init (); 
  
}

void loop() 
{ 
  RemoteXY_Handler ();

}
struct MOTOR_PINS
{
  int pinIN1;
  int pinIN2;    
};
