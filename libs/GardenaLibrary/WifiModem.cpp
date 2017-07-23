/*
 * WifiModem.cpp
 *
 *  Created on: 17.07.2017
 *      Author: pionet
 */

#include "WifiModem.h"
#include "Debug.h"

WifiModem::WifiModem(HardwareSerial *port, int8_t rst, uint32_t baud)
{
  _rstpin = rst;
  _mySerial = port;
  _baudrate = baud;
  webService = new SMS();

}


WifiModem::~WifiModem() {
	// TODO Auto-generated destructor stub
}

void WifiModem::begin(){
	_mySerial->begin(_baudrate);
	PRINTLN("WifiModem::begin() called");
}

void WifiModem::handle(){
	if(_mySerial->available() > 0)
	{
		String dataFromEsp = "";
		dataFromEsp = _mySerial->readStringUntil('\n');
		//Serial.println("data: "+inData);
		PRINTLN(dataFromEsp);
		//TODO: check if semaphore needed:
		webService->msg = dataFromEsp;
		webService->newMsgArrived = true;//subscriber.handle thread will check if new message has arrived
		dataFromEsp="";
	}
}

void WifiModem::sendWebJsonMsg(JsonObject& jsonMsg){
	String strg = jsonMsg["scope"];
	PRINT("scope is: ");PRINTLN(strg);
	jsonMsg.printTo((Print&)*_mySerial);
	_mySerial->println();
	//for (JsonObject::iterator it = jsonMsg.begin(); it != jsonMsg.end(); ++it) {
	    // get the object in the array
	//	String strg = jsonMsg.;
	    //jsonMsg.remove(*it);
	//}
	//String strg2 = jsonMsg["scope"];
	//PRINT("does it clear?: ");PRINTLN(strg2);

}

void WifiModem::sendWebStrgMsg(String msg){
	//String strg = jsonMsg["scope"];
	//PRINT("scope is: ");PRINTLN(strg);
	//jsonMsg.printTo((Print&)*_mySerial);
	_mySerial->println(msg);
	//for (JsonObject::iterator it = jsonMsg.begin(); it != jsonMsg.end(); ++it) {
	    // get the object in the array
	//	String strg = jsonMsg.;
	    //jsonMsg.remove(*it);
	//}
	//String strg2 = jsonMsg["scope"];
	//PRINT("does it clear?: ");PRINTLN(strg2);

}
