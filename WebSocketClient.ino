/*
   WebSocketClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#include <Servo.h>

Servo servo;
Servo servo1;
Servo servo2;
Servo servo3;
//
//int servoPin = 9;
//int servoPin1 = 6;
//int servoPin2 = 5;
//int servoPin3 = 4;

char* x;
char* y;
char* z;
long x_map;

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
char *token;


#define USE_SERIAL Serial

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
        USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

        // send message to server when Connected
        webSocket.sendTXT("Connected");
      }
      break;
    case WStype_TEXT:
      USE_SERIAL.printf("[WSc] get text: %s\n", payload);
      x = strtok((char *)payload, ";");
      y = strtok(NULL, ";");
      z = strtok(NULL, ";");
      x_map = map(1000*(1-atof(x)), 0, 1000, 0, 180);
      servo.write(x_map);
      servo1.write(atoi(y));
      USE_SERIAL.println(1000*(1-atof(x)));
      USE_SERIAL.println(x_map);
      //USE_SERIAL.println(y);
            
      // send message to server
      // webSocket.sendTXT("message here");
      break;
    case WStype_BIN:
      USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
    case WStype_PING:
      // pong will be send automatically
      USE_SERIAL.printf("[WSc] get ping\n");
      break;
    case WStype_PONG:
      // answer to a ping we send
      USE_SERIAL.printf("[WSc] get pong\n");
      break;
  }

}

void setup() {

  servo.attach(2);
  servo1.attach(0);
  servo2.attach(4);
  servo3.attach(5);
  //  servo1.attach(servoPin1);
  //  servo2.attach(servoPin2);
  //  servo3.attach(servoPin3);

  // USE_SERIAL.begin(921600);
  USE_SERIAL.begin(115200);

  //Serial.setDebugOutput(true);
  USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFiMulti.addAP("monster", "kenys.11");

  //WiFi.disconnect();
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  // server address, port and URL
  webSocket.begin("192.168.1.142", 8080, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // use HTTP Basic Authorization this is optional remove if not needed
  //	webSocket.setAuthorization("user", "Password");

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.enableHeartbeat(15000, 3000, 2);

}

void loop() {

  webSocket.loop();
}
