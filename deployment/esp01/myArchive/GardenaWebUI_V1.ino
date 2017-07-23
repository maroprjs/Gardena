//V1
/**
 *
 *V1 - 12.07.2017
*   - compiled with https://github.com/nailbuster/myWebServer
*   - and v5.6.7 https://github.com/bblanchon/ArduinoJson
*   - and https://github.com/PaulStoffregen/Time
*   - WORKING!!!
*   ESP01 settings : 	Generic ESP8266;upload protocol: default;CPU Frequ.: 80Mhz;
*   					Debug port: serial;debug level: none; flash frequ: 40Mhz;
*   					Flash mode: DIO; flash size: 1M(128k SPIFFS); reset method:ck
*/

#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <myWebServer.h>
//#include <TimeLib.h>



void setup() {
  // put your setup code here, to run once:
	Serial.begin(115200);

	MyWebServer.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
	MyWebServer.handle();

}
