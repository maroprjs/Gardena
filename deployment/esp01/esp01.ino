/**
* ESP01 settings : 	Generic ESP8266;upload protocol: default;CPU Frequ.: 80Mhz;
*   					Debug port: serial;debug level: none; flash frequ: 40Mhz;
*   					Flash mode: DIO; flash size: 1M(128k SPIFFS); reset method:ck
*
*	ArduinoJson Version v5.8.0 (above causes compiler error in "fuzzer.cpp"!?)
*/
//#include <Arduino.h>

#include "Gardena.h"

GARDENA_ESP::Gardena webUI;


void setup() {
	webUI.begin();
}

void loop() {
	webUI.handle();
}



