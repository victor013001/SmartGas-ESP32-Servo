#include "config.h"

void setupSerial();
void setupRGBLED();
void blueLedOn();
void setupServo();
void attachServo();
void detachServo();
void connectWiFi();
void createMQTTClient();
void clientServoCallback(char *topic, byte *payload, unsigned int length);
void publishServoStatus(const char *status);
void doServoDeactivation();
void greenLedOn();
void doServoActivation();
void redLedOn();
void reconnectMQTTClient();
void persistMQTTConnection();

void setup()
{
  setupSerial();
  setupRGBLED();
  blueLedOn();
  setupServo();
  connectWiFi();
  createMQTTClient();
  greenLedOn();
  Serial.println("SmartGas >> Setup completed");
}

void loop()
{
  persistMQTTConnection();
}

void setupSerial()
{
  Serial.begin(MONITOR_SPEED);
  while (!Serial)
    ;
  delay(SERIAL_DELAY);
  Serial.println();
  Serial.println("SmartGas >> Serial setup");
}

void setupRGBLED()
{
  Serial.println("SmartGas >> Setting up the RGB LED");
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);
}

void blueLedOn()
{
  Serial.println("SmartGas >> Turning on the blue LED");
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_BLUE, HIGH);
}

void setupServo()
{
  Serial.println("SmartGas >> Setting up the servo motor");
  attachServo();
  servoMotor.write(0);
  delay(INITIAL_SERVO_DELAY);
  detachServo();
}

void attachServo()
{
  servoMotor.attach(PIN_SERVO);
}

void detachServo()
{
  servoMotor.detach();
}

void connectWiFi()
{
  Serial.print("SmartGas >> Connecting to ");
  Serial.print(SSID);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print(ID.c_str());
  Serial.println(" connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void createMQTTClient()
{
  Serial.println("SmartGas >> Creating MQTT client");
  client.setServer(BROKER.c_str(), PORT);
  client.setCallback(clientServoCallback);
  reconnectMQTTClient();
}

void clientServoCallback(char *topic, byte *payload, unsigned int length)
{
  String response;
  for (int i = 0; i < length; i++)
  {
    response += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(SERVO_TOPIC.c_str());
  Serial.print("]: ");
  if (response == SERVO_OFF_MESSAGE)
  {
    publishServoStatus(response.c_str());
    Serial.println(response);
    doServoDeactivation();
  }
  else if (response == SERVO_ON_MESSAGE)
  {
    publishServoStatus(response.c_str());
    Serial.println(response);
    doServoActivation();
  }
  else
  {
    Serial.println("Invalid message received. Expected: ON or OFF");
  }
}

void publishServoStatus(const char *status)
{
  client.publish(SERVO_STATUS_TOPIC.c_str(), status);
}

void doServoDeactivation()
{
  if (currentServoPosition == SERVO_MAX_POSITION)
  {
    Serial.println("SmartGas >> Deactivating the servo motor");
    attachServo();
    for (int pos = SERVO_MAX_POSITION; pos >= 0; pos -= 1)
    {
      servoMotor.write(pos);
      delay(SERVO_DELAY);
    }
    detachServo();
    greenLedOn();
    currentServoPosition = 0;
  }
}

void greenLedOn()
{
  analogWrite(PIN_LED_RED, 0);
  analogWrite(PIN_LED_GREEN, 255);
  analogWrite(PIN_LED_BLUE, 0);
}

void doServoActivation()
{
  if (currentServoPosition == 0)
  {
    Serial.println("SmartGas >> Activating the servo motor");
    attachServo();
    for (int pos = 0; pos <= SERVO_MAX_POSITION; pos += 1)
    {
      servoMotor.write(pos);
      delay(SERVO_DELAY);
    }
    detachServo();
    redLedOn();
    currentServoPosition = SERVO_MAX_POSITION;
  }
}

void redLedOn()
{
  analogWrite(PIN_LED_RED, 255);
  analogWrite(PIN_LED_GREEN, 0);
  analogWrite(PIN_LED_BLUE, 0);
}

void reconnectMQTTClient()
{
  while (!client.connected())
  {
    Serial.println("SmartGas >> Attempting MQTT connection...");
    if (client.connect(CLIENT_NAME.c_str()))
    {
      Serial.print("Connected to Broker: ");
      Serial.println(BROKER.c_str());
      client.subscribe(SERVO_TOPIC.c_str(), 1);
    }
    else
    {
      Serial.print("Retying in 5 seconds - failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void persistMQTTConnection()
{
  reconnectMQTTClient();
  client.loop();
  delay(PERSIST_MQTT_CONNECTION_DELAY);
}