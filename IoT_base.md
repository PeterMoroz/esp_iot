# IoT base

This project is base starting point to create IoT modules based on ESP family MCU.

#### WiFi connection
To establish connection with WiFi network the unit should know SSID of that network and password (in most cases connection is protected with WPA2 or WPA3). To avoid hardcoding these values user should be provided with feature 
 - choose WiFi network from available ones
 - input password 
 - store these values somewhere for the next time

WiFiManager is library for configuring ESP8266/ESP32 modules WiFi credentials at runtime with captive portal.

When device powered on it tries connect to known network with **Station** mode (**STA**). When failure (or no credentials saved yet) it switches to **Access Point** mode (**AP**) and provides web-interface to setup new connection. Web-interface is available at `192.168.4.1` .

#### WebServer and DNS
Web server running on IoT device provides two different services
- web user interface allowing users to interact with device via web-browser
- REST API (and WebSocket API) for access and update device state, get sensors values, etc 

A basic web-server could be implemented with library ESP8266WebServer, it provides class of the same name. The class allows setup listening port and HTTP-handlers:
- register routes, i.e. bind processing function to requested path
- send responses (HTTP status code, content and content type)
- handle errors

The **mDNS** (multicast DNS) resolves host names to IP addresses in small networks that lack a local name server. It uses the same packet formats and operating semantics as unicast DNS. Creating a simple mDNS is possible using ESP8266mDNS library.

Both web-server and DNS-resolver use synchronous model of handling requests. Their corresponding methods must be called inside the main `loop()` function.


#### OTA update
**OTA** is a way to update device via WiFi. The acronim OTA came from 'Over The Air'. This approach is very handy because wire connection is no needed and device to update can be located anywhere.

###### How it works
It requires two partitions on Flash memory. The active partition contains firmware which is running now (current version), another partition is used to write new version of firmware. When upload, write and verify of new firmware complete bootloader makes the second partition active and reboot device. 

###### Firmware delivery
There are few methods to delivery a new binary file to device. One of them runs small HTTP-server on device, when user opens a special web-page in browser he/she can choose binary file which contains new version of firmware.
