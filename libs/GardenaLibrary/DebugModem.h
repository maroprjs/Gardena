/*
 * DebugModem.h
 *
 *  Created on: 21.07.2017
 *      Author: pionet
 */

#ifndef DEBUGMODEM_H_
#define DEBUGMODEM_H_
#include <Arduino.h>
#include <ArduinoJson.h>
#include "SMS.h"

class DebugModem {
public:
	DebugModem(uint32_t bdrate = 115200);
	virtual ~DebugModem();
	void begin();
	void handle(void);
	void sendDbgJsonMsg(JsonObject& jsonMsg);
	void sendDbgStrgMsg(String msg);
	SMS * dbgService;
protected:
 //HardwareSerial *_mySerial;
 uint32_t _baudrate;
};

#endif /* DEBUGMODEM_H_ */
