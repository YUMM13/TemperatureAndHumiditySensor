#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "secrets.h"

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"

// sensor info
#define DHT_TYPE DHT11
const int DHT_PIN = 23;

DHT_Unified dht(DHT_PIN, DHT_TYPE);

float farenheit = 0.0f;
float humid = 0.0f;
int status = 0;

// wifi
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void pubSubErr(int8_t MQTTErr)
{
  if (MQTTErr == MQTT_CONNECTION_TIMEOUT)
    Serial.print("Connection tiemout");
  else if (MQTTErr == MQTT_CONNECTION_LOST)
    Serial.print("Connection lost");
  else if (MQTTErr == MQTT_CONNECT_FAILED)
    Serial.print("Connect failed");
  else if (MQTTErr == MQTT_DISCONNECTED)
    Serial.print("Disconnected");
  else if (MQTTErr == MQTT_CONNECTED)
    Serial.print("Connected");
  else if (MQTTErr == MQTT_CONNECT_BAD_PROTOCOL)
    Serial.print("Connect bad protocol");
  else if (MQTTErr == MQTT_CONNECT_BAD_CLIENT_ID)
    Serial.print("Connect bad Client-ID");
  else if (MQTTErr == MQTT_CONNECT_UNAVAILABLE)
    Serial.print("Connect unavailable");
  else if (MQTTErr == MQTT_CONNECT_BAD_CREDENTIALS)
    Serial.print("Connect bad credentials");
  else if (MQTTErr == MQTT_CONNECT_UNAUTHORIZED)
    Serial.print("Connect unauthorized");
}

void connectToWiFi()
{
  // establish wifi connection
  WiFi.setHostname(THINGNAME);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Done!");
}

void connectAWS()
{
  // Sync time
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for time sync");
  time_t now = time(nullptr);
  while (now < 1600000000) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println(" done");

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  client.setServer(AWS_IOT_ENDPOINT, 8883);

  Serial.print("Connecting to AWS IOT...");

  while (!client.connected()) {
    if (!client.connect(THINGNAME)) {
      Serial.print("AWS IoT Timeout! Reason: ");
      pubSubErr(client.state());
      Serial.println(". Try again in 5 seconds");
      delay(5000);
    }
  }
  
  Serial.println("AWS IoT Connected!");
}

void publishMessage(float temp, float humidity, int ok)
{
  StaticJsonDocument<200> doc;
  if (!ok)
  {
    doc["time"] = millis();
    doc["device"] = "esp32-01";
    doc["temperature"] = temp;
    doc["humidity"] = humidity;
    doc["status"] = "ok";
  }
  else {
    doc["time"] = millis();
    doc["device"] = "esp32-01";
    doc["temperature"] = NULL;
    doc["humidity"] = NULL;
    doc["status"] = "sensor_error";
  }
  char jsonBuffer[512];
  // serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);

  // Serial.println("Sent Data!");
  // Serial.println(jsonBuffer);
}

void setup() {
  Serial.begin(115200);

  // Initialize device.
  dht.begin();

  // connect to netowrk
  connectToWiFi();

  // connect to AWS
  connectAWS();
}

void loop() {
  status = 0;
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
    status = 1;
  }
  else {
    farenheit = ((9/5) * event.temperature) + 32;
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
    status = 1;
  }
  else {
    humid = event.relative_humidity;
  }

  // send info off to AWS
  publishMessage(farenheit, humid, status);

  // serial print for debug
  // Serial.println("Sent Data!");
  // Serial.printf("Temp: %f,  Humidity: %f\n", farenheit, humid);

  client.loop();

  // Delay 1 min between measurements.
  delay(60000);
}
