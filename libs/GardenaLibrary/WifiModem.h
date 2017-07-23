/*
 * WifiModem.h
 *
 *  Created on: 17.07.2017
 *      Author: pionet
 */

#ifndef WIFIMODEM_H_
#define WIFIMODEM_H_
#include <Arduino.h>
#include <ArduinoJson.h>
#include "SMS.h"
class WifiModem {
public:
	WifiModem(HardwareSerial *port, int8_t rsetpin, uint32_t bdrate = 9600);
	virtual ~WifiModem();
	void begin();
	void handle(void);
	void sendWebJsonMsg(JsonObject& jsonMsg);
	void sendWebStrgMsg(String msg);
	SMS * webService;
protected:
 int8_t _rstpin;
 HardwareSerial *_mySerial;
 uint32_t _baudrate;
};

#endif /* WIFIMODEM_H_ */
