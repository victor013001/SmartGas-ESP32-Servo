#include <Arduino.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <PubSubClient.h>
#include <string>

#pragma once

using namespace std;

#define PIN_SERVO 18     // GPIO18
#define PIN_LED_RED 23   // GPI23
#define PIN_LED_GREEN 22 // GPI22
#define PIN_LED_BLUE 21  // GPI21
#define TIME_TO_ACTIVATE_ALERTS 5
#define SERVO_DELAY 15
#define INITIAL_SERVO_DELAY 2000
#define SERVO_MAX_POSITION 180
#define PERSIST_MQTT_CONNECTION_DELAY 500
#define WIFI_CONNECTION_DELAY 500
#define RECONNECTION_DELAY 5000
#define SERIAL_DELAY 1000
#define MONITOR_SPEED 9600

Servo servoMotor;
WiFiClient wifiClient;
PubSubClient client(wifiClient);

int currentServoPosition = 0;

const char *SSID = "";
const char *PASSWORD = "";

const string ID = "smargas_esp32_002";
const string CLIENT_NAME = ID + "SmartGas client";

const string BROKER = "192.168.204.206";
const int PORT = 1883;

const string SERVO_TOPIC = "UdeA/SmartGas/Servo/" + ID;
const string SERVO_STATUS_TOPIC = SERVO_TOPIC + "/Status";

const char *SERVO_ON_MESSAGE = "ON";
const char *SERVO_OFF_MESSAGE = "OFF";