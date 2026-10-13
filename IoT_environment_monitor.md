# IoT environment monitor

This project is an ESP8266-based indoor environment monitoring system that measures environmental parameters including temperature, humidity, CO2 and volatile organic compounds. 

### Hardware components
The project is based on **WeMos D1 Mini**, a miniature Wi-Fi development board build around the Espressif ESP8266 ESP-12F module. Sensor modules to measure environmental parameters:
1. DHT22 - digital Temperature/Humidity sensor with calibrated signal output. It uses custom single-wire digital protocol for communication.
2. CCS811 - digital gas sensor for monitoring indoor air quality. It measures levels of Total Volatile Organic Compounds (TVOCs) and equivalent CO<sub>2</sub> (eCO2).

### Sensors' support libraries
These libraries hide complexities of underlying communication protocols. They provide a high-level interface for performing basic sensor operations, such as initialization, mode configuration and reading values.
The most well-known library for the DHT series of temperatur/humidity sensors is the Adafruit DHT library. Unfortunatelly it wouldn't work for me - the functions to read temperature/humidity values returned NaN (which means error) constantly. I looked into source code, enabled debugging output via serial port. The problem was that reading of 40-bit packet from sensor couldn't start because waiting of low-level on data-line finished with timeout. I was already planning to try connecting the sensor to different GPIO (I had connected it to D0/GPIO16 which has some quirks) - just to verify that the sensor is working properly. But before to do it I made one more attempt to find the reason with Google. Result is I found one more example of usage DHT sensor in project based on ESP8266 [link](https://github.com/dzindra/dhtesp) and it include slightly modified version of original code of Arduino DHT library. I included the code of DHT-driver I found into my project and the problem gone away. The dependency on the original Adafruit DHT library I excluded from project.

The interaction with CCS811 sensor can be implemented using the Adafruit CCS811 Library, however I chose an alternative - Maarten-Pennings/CCS811 as it is considered to implement the communication protocol for CCS811 more effectively.
