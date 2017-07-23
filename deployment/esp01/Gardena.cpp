/*
 * Gardena.cpp
 *
 *  Created on: 23.07.2017
 *      Author: pionet
 */

#include "Gardena.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <myWebServer.h>
//#include <TimeLib.h>

namespace GARDENA_ESP {

Gardena::Gardena() {
	// TODO Auto-generated constructor stub

}

Gardena::~Gardena() {
	// TODO Auto-generated destructor stub
}

void Gardena::begin(){
	Serial.begin(115200);

	MyWebServer.begin();
	server.on("/myfunction",std::bind(&Gardena::myfunction, this));
}


void Gardena::handle(){
	MyWebServer.handle();

	if(Serial.available() > 0)
	{
		String dataFromTeensy = "";
		dataFromTeensy = Serial.readStringUntil('\n');
		//Serial.println("data: "+inData);
		server.send(200, "text/plain", dataFromTeensy );
		dataFromTeensy="";
	}
}

void Gardena::myfunction(){
	int i = server.args();
	DynamicJsonBuffer  jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();

	if (i )
	{
		String args = "";
		String argName = "";
		for (int j = 0; j<i;j++)
		{
			//argName = server.argName(j);
			//args = args + argName + "="+ server.arg(j) + " ";
			root[server.argName(j)] = server.arg(j);
		}
		//Serial.println(args);
		root.printTo(Serial);
		Serial.println();
	}
}

} /* namespace GARDENA_ESP */
