#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
// Defining Constants
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define TURN_LEFT 9
#define TURN_RIGHT 10
#define STOP 0

#define RIGHT_MOTORS 0
#define LEFT_MOTORS 1
#define FORWARD 1
#define BACKWARD -1



// Creating Struct for Motor Pins thus handling objects
struct MOTOR_PINS
{
  int pinIN1;
  int pinIN2;
};


std::vector<MOTOR_PINS> motorPins =
{
  {26, 27},  //RIGHT_MOTORS  PIN1,Pin2
  {25, 33},  //LEFT_MOTORS   PIN1,Pin2
};


//Giving SOFTAP Credentials for Hotspot
const char* ssid     = "Hashi's Car";
const char* password = "12345678";

//Golbal Variables for Asyn Web Server and Socket
AsyncWebServer server(80);   //Port 80
AsyncWebSocket ws("/ws");



//Creating an HTML Page and storing that to the PROGMEM to send to the device

const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
    <style>
    .arrows {
      font-size:70px;
      color:white;
    }
    .circularArrows {
      font-size:80px;
      color:white;
    }
    td {
      /* background: rgb(0, 0, 252);; */
      background: #000080;
      border-radius:25%;
      box-shadow: 5px 5px #888888;
    }
    td:active {
      transform: translate(5px,5px);
      box-shadow: none;
    }
    .noselect {
      -webkit-touch-callout: none; /* iOS Safari */
        -webkit-user-select: none; /* Safari */
         -khtml-user-select: none; /* Konqueror HTML */
           -moz-user-select: none; /* Firefox */
            -ms-user-select: none; /* Internet Explorer/Edge */
                user-select: none; /* Non-prefixed version, currently
                                      supported by Chrome and Opera */
    }
    </style>
  </head>
  <body class="noselect" align="center" style="background-color:white">

    <h1 style="color: teal;text-align:center;">MOTOR CONTROLLER</h1>
    <h2 style="color: teal;text-align:center;">Wi-Fi CAR Control</h2>

    <table id="mainTable" style="width:400px;margin:auto;table-layout:fixed" CELLSPACING=10>
      <tr>
        <td style="background-color:white;box-shadow:none"></td>
        <td ontouchstart='onTouchStartAndEnd("1")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#8679;</span></td>
      </tr>

      <tr>
        <td ontouchstart='onTouchStartAndEnd("3")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#8678;</span></td>
        <td style="background-color:white;box-shadow:none"></td>
        <td ontouchstart='onTouchStartAndEnd("4")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#8680;</span></td>
      </tr>

      <tr>
        <td ontouchstart='onTouchStartAndEnd("9")' ontouchend='onTouchStartAndEnd("0")'><span class="circularArrows" >&#8634;</span></td>

        <td ontouchstart='onTouchStartAndEnd("2")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#8681;</span></td>
        <td ontouchstart='onTouchStartAndEnd("10")' ontouchend='onTouchStartAndEnd("0")'><span class="circularArrows" >&#8635;</span></td>

      </tr>

      <tr>
        <td style="background-color:white;box-shadow:none"></td>
      </tr>
    </table>
    <script>
      var webSocketUrl = "ws:\/\/" + window.location.hostname + "/ws";
      var websocket;


//      Initializing the WEBSOCKET
      function initWebSocket()
      {
        websocket = new WebSocket(webSocketUrl);
        websocket.onopen    = function(event){};
        websocket.onclose   = function(event){setTimeout(initWebSocket, 2000);};  //TImeout for reconnecting set to 2 Seconds
        websocket.onmessage = function(event){};
      }
      function onTouchStartAndEnd(value)    // Function to send value from device to ESP32
      {
        websocket.send(value);    // WEbsocket used to send the commands
      }

      window.onload = initWebSocket;    // WHen window is loaded on device it initialized websocket.
//
);
    </script>

  </body>
</html>
)HTMLHOMEPAGE";



//Rotate Motor

void rotateMotor(int motorNumber, int motorDirection)
{
  if (motorDirection == FORWARD) // SETS the Right Side of Car Pins to 5V and 0V for Forward Move
  {
    digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);
  }
  else if (motorDirection == BACKWARD)  // Same as Forward but Voltage Direction changed.
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, HIGH);
  }
  else   //No Potential Difference
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);
  }
}

void processCarMovement(String inputValue)    //
{
  Serial.printf("Got value as %s %d\n", inputValue.c_str(), inputValue.toInt());
  switch(inputValue.toInt())
  {

    case UP:
      rotateMotor(RIGHT_MOTORS, FORWARD);
      rotateMotor(LEFT_MOTORS, FORWARD);
      break;

    case DOWN:
      rotateMotor(RIGHT_MOTORS, BACKWARD);
      rotateMotor(LEFT_MOTORS, BACKWARD);
      break;

    case LEFT:
      rotateMotor(RIGHT_MOTORS, FORWARD);
      rotateMotor(LEFT_MOTORS, BACKWARD);
      break;

    case RIGHT:
      rotateMotor(RIGHT_MOTORS, BACKWARD);
      rotateMotor(LEFT_MOTORS, FORWARD);
      break;

    case TURN_LEFT:
      rotateMotor(RIGHT_MOTORS, FORWARD);
      rotateMotor(LEFT_MOTORS, BACKWARD);
      break;

    case TURN_RIGHT:
      rotateMotor(RIGHT_MOTORS, BACKWARD);
      rotateMotor(LEFT_MOTORS, FORWARD);
      break;

    case STOP:
      rotateMotor(RIGHT_MOTORS, STOP);
      rotateMotor(LEFT_MOTORS, STOP);
      break;

    default:
      rotateMotor(RIGHT_MOTORS, STOP);
      rotateMotor(LEFT_MOTORS, STOP);
      break;
  }
}


//Web Server on Request sends the HTML page to the DEVICE


void handleRoot(AsyncWebServerRequest *request)
{
  request->send_P(200, "text/html", htmlHomePage);
}


//IF not found sends the given error

void handleNotFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "File Not Found");
}

//Defining Web Socket Events.

void onWebSocketEvent(AsyncWebSocket *server,
                      AsyncWebSocketClient *client,
                      AwsEventType type,   //Web server Event type
                      void *arg,
                      uint8_t *data,
                      size_t len)
{
  switch (type)
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      processCarMovement("0");
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)   //Default Values
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        processCarMovement(myData.c_str());
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;
  }
}

// For Setting up Pin Modes to output

void setUpPinModes()
{
  for (int i = 0; i < motorPins.size(); i++)
  {
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);
    rotateMotor(i, STOP);
  }
}


void setup(void)
{
  setUpPinModes(); //Setting Pin Modes to the OUTPUT
  Serial.begin(115200);    // Starting Serial Channel for console messages

  WiFi.softAP(ssid, password);    //Starting the Hotspot/ Access Point
  IPAddress IP = WiFi.softAPIP();   //Storing IP of Hotspot to access on device
  Serial.print("AP IP address: ");  // Printing on console
  Serial.println(IP);


  // Assign callback of handleROOT to send the HTML WEBPAGE created to the device on request
  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);   //Handle error if not found

  server.begin();    // Starting Server

  ws.onEvent(onWebSocketEvent);  //Tackling different web socket events received from the device
  server.addHandler(&ws);        // Error Handler

  Serial.println("HTTP server started");
}

void loop()
{
  ws.cleanupClients(); // WEbsocket own function to handle previous unused connection
}
