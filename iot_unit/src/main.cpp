#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>


#define HOSTNAME "myesp8266"
#define AP_NAME "My ESP8266 AP"

ESP8266WebServer webServer(80);


void handleRoot() {
  webServer.send(200, "text/html", "<h1><Hello from ESP8266!</h1>");
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

  webServer.on("/", handleRoot);
  webServer.begin();
  Serial.println("WebServer - OK");
}

void loop() {
  MDNS.update();
  webServer.handleClient();
}
