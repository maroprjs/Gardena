/*
 * DebugModem.cpp
 *
 *  Created on: 21.07.2017
 *      Author: pionet
 */

#include "DebugModem.h"
#include "Debug.h"

DebugModem::DebugModem(uint32_t baud) {
	  _baudrate = baud;
	  dbgService = new SMS();

}

DebugModem::~DebugModem() {
	// TODO Auto-generated destructor stub
}

void DebugModem::begin(){
	//PRINT_INIT(_baudrate);
	Serial.begin(_baudrate);
	//PRINTLN("DebugModem::begin() called");
}

void DebugModem::handle(){
	if(Serial.available() > 0)
	{
		String dataFromConsole = "";
		dataFromConsole = Serial.readStringUntil('\n');
		//Serial.println("data: "+inData);
		Serial.println(dataFromConsole);
		//TODO: check if semaphore needed:
		dbgService->msg = dataFromConsole;
		dbgService->newMsgArrived = true;//subscriber.handle thread will check if new message has arrived
		dataFromConsole="";
	}
}

void DebugModem::sendDbgJsonMsg(JsonObject& jsonMsg){
	String strg = jsonMsg["scope"];
	PRINT("scope is: ");PRINTLN(strg);
	String s = "";
	jsonMsg.printTo(s);
	Serial.println(s);


}

void DebugModem::sendDbgStrgMsg(String msg){

	PRINTLN(msg);


}
