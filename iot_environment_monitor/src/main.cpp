#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <ESP8266HTTPUpdateServer.h>

#include "DHT.h"

#include <Wire.h>
#include <ccs811.h>


#define HOSTNAME "myesp8266"
#define AP_NAME "My ESP8266 AP"

ESP8266WebServer webServer(80);
ESP8266HTTPUpdateServer updateServer;

#define UPDATE_USER "admin"
#define UPDATE_PASS "secret"


#define DHT_PIN D0
#define DHT_SENSOR_TYPE DHT22
#define DHT_READ_INTERVAL_MS 4000

DHT dht(DHT_PIN, DHT_SENSOR_TYPE);
unsigned long lastTimeReadDHT = 0;
float dhtTemperature = 0.f;
float dhtHumidity = 0.f;


#define AIQ_READ_INTERVAL_MS 4000
unsigned long lastTimeReadAIQ = 0;

CCS811 ccs811;

void handleRoot() {
  char text[96] = { '\0' };
  snprintf(text, sizeof(text), 
    "<b>Temperature:<b> %f <br/> <b>Humidity:</b> %f",
    dhtTemperature, dhtHumidity);
  webServer.send(200, text);
}


void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFiManager wifiManager;
  if (!wifiManager.autoConnect(AP_NAME)) {
    Serial.println("Couldn't connect to WiFi network, timeout. Reboot...");
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin(HOSTNAME)) {
    Serial.printf("mDNS - OK, http://%s.local\n", HOSTNAME);
    MDNS.addService("http", "tcp", 80);
  } else {
    Serial.println("mDNS failed to start");
  }

  updateServer.setup(&webServer, "/update", UPDATE_USER, UPDATE_PASS);

  webServer.on("/", handleRoot);
  webServer.begin();
  Serial.println("WebServer - OK");

  dht.begin();
  delay(2000);

  Wire.begin();
  ccs811.set_i2cdelay(50);
  if (!ccs811.begin()) {
    Serial.println("CCS811 - not found");
  }

  if (!ccs811.start(CCS811_MODE_1SEC)) {
    Serial.println("CCS811 - failed to start");
  }
}

void loop() {
  MDNS.update();
  webServer.handleClient();

  unsigned long ms = millis();
  if ((ms - lastTimeReadDHT) >= DHT_READ_INTERVAL_MS) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t) && !isnan(h)) {
      dhtTemperature = t;
      dhtHumidity = h;
      Serial.printf("temperature: %f, humidity: %f%%\n", t, h);
      if (!ccs811.set_envdata_Celsius_percRH(t, h)) {
        Serial.println("CCS811 - set environment data failed");
      }
    } else{
      Serial.println("DHT - read failed");
    }

    lastTimeReadDHT = ms;
  }

  if ((ms - lastTimeReadAIQ) >= AIQ_READ_INTERVAL_MS) {
    uint16_t eco2 = 0, etvoc = 0, stat = 0;
    ccs811.read(&eco2, &etvoc, &stat, NULL);
    if (stat == CCS811_ERRSTAT_OK) {
      Serial.printf("eCO2: %u, eTVOC: %u\n", eco2, etvoc);
    } else {
      if (stat == CCS811_ERRSTAT_OK_NODATA) {
        Serial.println("CCS811 - no data yet");
      } else {
        Serial.printf("CCS811 - error 0x%04X (%s)\n", stat, ccs811.errstat_str(stat));
      }
    }
    lastTimeReadAIQ = ms;
  }

}
