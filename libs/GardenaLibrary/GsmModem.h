/*
 * GsmModem.h
 *
 *  Created on: 20.10.2016
 *      Author: maro
 */

#ifndef GSMMODEM_H_
#define GSMMODEM_H_

#include <Arduino.h>
#include "SMS.h"
//#include "Subscriber.h"

//namespace Gardena {

#define DEFAULT_TIMEOUT_MS 500

class GsmModem {

public:

	enum NetworkStatus
	{
		NotRegistered = 0,
	    RegisteredHome = 1,
	    NotRegisteredSearching = 2,
		Denied = 3,
		Unknown = 4,
		RegisteredRoaming = 5
	};

	GsmModem();
	GsmModem(HardwareSerial *port, int8_t rsetpin, uint32_t bdrate = 9600);
	virtual ~GsmModem();
	void begin();
	//void subscribe(Subscriber * subscriber);
	void handle(void);
	void reset();
	void roam();
	bool event();
	bool heartBeat(void);
	uint16_t readRaw(uint16_t b);
	bool smsArrived(void);
	int getSMSIndex(void);
	int8_t getNumSMS(void);
	boolean readSMS(uint8_t i, char *smsbuff, uint16_t max, uint16_t *readsize);
	boolean sendSMS(SMS * sms);
	boolean sendSMS(char *smsaddr, char *smsmsg);
	boolean deleteSMS(SMS *msg);
	bool delAllSms();
	boolean getSMSSender(uint8_t i, char *sender, int senderlen);
	boolean sendUSSD(char *ussdmsg, char *ussdbuff, uint16_t maxlen, uint16_t *readlen);
	String dateNet(); //return date,time, of the network
	boolean enableNetworkTimeSync(boolean onoff);
	boolean getTime(char *buff, uint16_t maxlen);
	NetworkStatus getNetworkStatus(void);
	bool getListOfOperators(void);
	uint8_t getRSSI(void);
	bool getSMS(SMS *sms);

	boolean sendCheckReply(char *send, char *reply, uint16_t timeout = DEFAULT_TIMEOUT_MS);
	boolean sendCheckReply(const __FlashStringHelper * send, const __FlashStringHelper * reply, uint16_t timeout = DEFAULT_TIMEOUT_MS);
	boolean sendParseReply(const __FlashStringHelper * tosend,const __FlashStringHelper * toreply,uint16_t *v, char divider = ',', uint8_t index=0);
	boolean sendCheckReply(char* send, const __FlashStringHelper * reply, uint16_t timeout = DEFAULT_TIMEOUT_MS);

	SMS* sms;

protected:
 int8_t _rstpin;
 HardwareSerial *_mySerial;
 uint32_t _baudrate;
 char _replybuffer[255];
 const __FlashStringHelper * _ok_reply;
 uint8_t getReply(char *send, uint16_t timeout = DEFAULT_TIMEOUT_MS);
 uint8_t getReply(const __FlashStringHelper * send, uint16_t timeout = DEFAULT_TIMEOUT_MS);
 void flushInput();
 uint8_t readline(uint16_t timeout = DEFAULT_TIMEOUT_MS, boolean multiline = false);
 boolean parseReply(const __FlashStringHelper * toreply,
           uint16_t *v, char divider  = ',', uint8_t index=0);
 boolean parseReplyQuoted(const __FlashStringHelper * toreply,
           char *v, int maxlen, char divider, uint8_t index);
 int _smsNOKCounter;
 const int MAX_ARRIVED_SMS_NOK_DEL = 4;
 //SMS *_sms;obsolete
 //Subscriber * _subscriber;



};

//} /* namespace Gardena */

#endif /* GSMMODEM_H_ */
