/*
 * SMS.h
 *
 *  Created on: 24.10.2016
 *      Author: maro
 */

#ifndef SMS_H_
#define SMS_H_

#include <Arduino.h>
#include <ArduinoJson.h>

//namespace Gardena {

class SMS {
	friend class GsmModem;
public:
	SMS();
	virtual ~SMS();
	String sender;
	String receiver;
	String msg;
	JsonObject* jsonMsg;
	JsonArray* jsonData;
	bool newMsgArrived;
private:
	char smsMessage[255];
	char senderNumber[20];
	char receiverNumber[20];
	uint16_t smslen;
	uint16_t smsMaxLen;
	uint16_t smsIdx;

};

//} /* namespace Gardena */

#endif /* SMS_H_ */
