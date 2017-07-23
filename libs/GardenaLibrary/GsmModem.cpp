/*
 * GsmModem.cpp
 *
 *  Created on: 20.10.2016
 *      Author: maro
 */



//namespace Gardena {

#include "GsmModem.h"
#include "Debug.h"
#include "State.h"

GsmModem::GsmModem() {
	_rstpin = NONE;
	_mySerial = NULL;
	_baudrate = 9600;
	_ok_reply = F("OK");
//	 _subscriber = NULL;
	 _smsNOKCounter = 0;
	 sms = new SMS();
};



GsmModem::GsmModem(HardwareSerial *port, int8_t rst, uint32_t baud)
{
  _rstpin = rst;
  _mySerial = port;
  _baudrate = baud;

  _ok_reply = F("OK");
//  _subscriber = NULL;
  _smsNOKCounter = 0;
  sms = new SMS();
}



GsmModem::~GsmModem() {
	// TODO Auto-generated destructor stub

};

void GsmModem::begin(){
	_mySerial->begin(9600);
  //_buffer.reserve(255); //reserve memory to prevent intern fragmention
	PRINTLN("GsmModem::begin() called");
};

/*
void GsmModem::subscribe(Subscriber * subscriber){
	_subscriber = subscriber;
	PRINTLN("GsmModem::subscribe(..) called");

}
*/
void GsmModem::handle(void){
	//PRINTLN("handleGSMModemCalled");
	if (event()){
		PRINT(" Modem Event occured! ");
		if(smsArrived()){
			yield();
			if (getSMS(sms)){
				_smsNOKCounter = 0;
				PRINT("In INO: ");
				PRINT("Sender: ");
				PRINTLN(sms->sender);
				PRINT("Message: ");
				PRINTLN(sms->msg);
				sms->newMsgArrived = true;//subscriber.handle thread will check if new message has arrived
				//obsolete:
				//cmd->execute(smsToJson(_sms));->the GSM modem shouldn't know if it's a gardena command...so this is done by GsmSubscriber
				//if (_subscriber) _subscriber->msgArrived(_sms);//subscriber should know...so (s)he stores arrived message
				//deleteSMS(_sms->smsIdx);//from device buffer
			}else {
				_smsNOKCounter++;
			}
			if (_smsNOKCounter > MAX_ARRIVED_SMS_NOK_DEL){
				delAllSms();
				_smsNOKCounter = 0;//TODO: this needs  to be changed
			}
		}else{
			PRINT(" Modem Event Unhandled : ");
		}
	}
}



bool GsmModem::heartBeat(){
	int16_t timeout = 7000;
	PRINTLN("========== heartBeat() check begin ========>");
	while (timeout > 0) {
		PRINTLN("w1 ");
	    while (_mySerial->available()) _mySerial->read();
	    PRINTLN("w2 ");
	    if (sendCheckReply(F("AT"), _ok_reply)) break;
	    while (_mySerial->available()) _mySerial->read();
	    PRINTLN("w3 ");
	    if (sendCheckReply(F("AT"), F("AT"))) break;
	    delay(500);
	    timeout-=500;
	}
	sendCheckReply(F("AT"), _ok_reply);
	delay(100);
	sendCheckReply(F("AT"), _ok_reply);
	delay(100);
	sendCheckReply(F("AT"), _ok_reply);
	delay(100);
	PRINTLN("turning echo off! ");
    // turn off Echo!
    sendCheckReply(F("ATE0"), _ok_reply);
    delay(100);

    if (! sendCheckReply(F("ATE0"), _ok_reply)) {
    	PRINTLN("<============ heartBeat() check end ========");
    	return false;
    }
    PRINTLN("<============ heartBeat() check end ===========");
    return true;
}

void GsmModem::reset(){
	PRINTLN("resetting modem!");
	pinMode(_rstpin, OUTPUT);
	digitalWrite(_rstpin, HIGH);
	delay(10);
	digitalWrite(_rstpin, LOW);
	delay(100);
	digitalWrite(_rstpin, HIGH);
}

void GsmModem::roam(){//TODO: parse for proper operator
	PRINTLN("try roaming");
	_mySerial->print(F("AT+COPS=4,2,26203"));
	delay(20000);
}

bool GsmModem::event(){

	return _mySerial->available();

}

bool GsmModem::smsArrived(void){
	readline(1000);
	PRINTLN(strstr(_replybuffer, "CMTI:") );
	if ((strstr(_replybuffer, "CMTI:") != NULL) && (strstr(_replybuffer, "MMS") == NULL)){
		PRINT(" SMS arrived: ");
		PRINTLN(_replybuffer);
	}else{

		PRINTLN(_replybuffer);
		return false;
		//flushInput();
	}
	return true;
}


bool GsmModem::getSMS(SMS *sms){
	sms->smsIdx  = getSMSIndex();
	if (getSMSSender(sms->smsIdx, sms->senderNumber, sms->smsMaxLen)){
		PRINT("Sender: ");
		PRINTLN(sms->senderNumber);
		//sms->sender;
		sms->sender = sms->senderNumber;
	}else{
		PRINT("Sender unknown: ");
		PRINTLN(sms->senderNumber);
		sms->sender = "unknown";
	}

	if (readSMS(sms->smsIdx,sms->smsMessage,sms->smsMaxLen,&sms->smslen)){
		PRINT("Message: ");
		PRINTLN(sms->smsMessage);
		sms->msg = sms->smsMessage;
		return true;
	}
	return false;
}

int GsmModem::getSMSIndex(void){
	String idx = strstr(_replybuffer, ",");//e.g. +CMTI: "SM",1
	idx = idx.substring(idx.indexOf(',') + 1);
	return idx.toInt();
}

int8_t GsmModem::getNumSMS(void) {
  uint16_t numsms;

  // get into text mode
  if (! sendCheckReply(F("AT+CMGF=1"), _ok_reply)) return -1;

  // ask how many sms are stored
  if (sendParseReply(F("AT+CPMS?"), F("\"SM\","), &numsms))
	  PRINTLN(numsms);
   // return numsms;//MARO: chnage 11.10.2016
  if (numsms < 1){//MARO: chnage 11.10.2016
	if (sendParseReply(F("AT+CPMS?"), F("\"SM_P\","), &numsms))
    return numsms;
  }
  return -1;
}

boolean GsmModem::readSMS(uint8_t i, char *smsbuff,
			       uint16_t maxlen, uint16_t *readlen) {
  // text mode
  if (! sendCheckReply(F("AT+CMGF=1"), _ok_reply)) return false;

  // show all text mode parameters
  if (! sendCheckReply(F("AT+CSDH=1"), _ok_reply)) return false;

  // parse out the SMS len
  uint16_t thesmslen = 0;

  PRINT(F("AT+CMGR="));
  PRINTLN(i);


  //getReply(F("AT+CMGR="), i, 1000);  //  do not print debug!
  _mySerial->print(F("AT+CMGR="));
  _mySerial->println(i);
  readline(1000); // timeout

  //PRINT(F("Reply: ")); PRINTLN(_replybuffer);
  // parse it out...


  PRINTLN(_replybuffer);


  if (! parseReply(F("+CMGR:"), &thesmslen, ',', 11)) {
    *readlen = 0;
    return false;
  }

  readRaw(thesmslen);

  flushInput();

  uint16_t thelen = min(maxlen, strlen(_replybuffer));
  strncpy(smsbuff, _replybuffer, thelen);
  smsbuff[thelen] = 0; // end the string


  PRINTLN(_replybuffer);

  *readlen = thelen;
  return true;
}

boolean GsmModem::sendSMS(SMS * sms) {

	//string overwrite chars (easier API for subscriber for message and receiver)
	//https://stackoverflow.com/questions/7383606/converting-an-int-or-string-to-a-char-array-on-arduino
	int str_len = sms->receiver.length() + 1;
	sms->receiver.toCharArray(sms->receiverNumber, str_len);
	str_len = sms->msg.length() + 1;
	sms->msg.toCharArray(sms->smsMessage, str_len);
	char *smsaddr= sms->receiverNumber;
	char *smsmsg = sms->smsMessage;

	if (! sendCheckReply(F("AT+CMGF=1"), _ok_reply)) return -1;

  char sendcmd[30] = "AT+CMGS=\"";
  strncpy(sendcmd+9, smsaddr, 30-9-2);  // 9 bytes beginning, 2 bytes for close quote + null
  sendcmd[strlen(sendcmd)] = '\"';

  if (! sendCheckReply(sendcmd, F("> "))) return false;

  PRINT(F("> ")); PRINTLN(smsmsg);

  _mySerial->println(smsmsg);
  _mySerial->println();
  _mySerial->write(0x1A);

  PRINTLN("^Z");

  readline(10000); // read the +CMGS reply, wait up to 10 seconds!!!
  //PRINT("Line 3: "); PRINTLN(strlen(_replybuffer));
  if (strstr(_replybuffer, "+CMGS") == 0) {
    return false;
  }
  readline(1000); // read OK
  //PRINT("* "); PRINTLN(_replybuffer);

  if (strcmp(_replybuffer, "OK") != 0) {
    return false;
  }

  return true;
}

boolean GsmModem::sendSMS(char *smsaddr, char *smsmsg) {
  if (! sendCheckReply(F("AT+CMGF=1"), _ok_reply)) return -1;

  char sendcmd[30] = "AT+CMGS=\"";
  strncpy(sendcmd+9, smsaddr, 30-9-2);  // 9 bytes beginning, 2 bytes for close quote + null
  sendcmd[strlen(sendcmd)] = '\"';

  if (! sendCheckReply(sendcmd, F("> "))) return false;

  PRINT(F("> ")); PRINTLN(smsmsg);

  _mySerial->println(smsmsg);
  _mySerial->println();
  _mySerial->write(0x1A);

  PRINTLN("^Z");

  readline(10000); // read the +CMGS reply, wait up to 10 seconds!!!
  //PRINT("Line 3: "); PRINTLN(strlen(_replybuffer));
  if (strstr(_replybuffer, "+CMGS") == 0) {
    return false;
  }
  readline(1000); // read OK
  //PRINT("* "); PRINTLN(_replybuffer);

  if (strcmp(_replybuffer, "OK") != 0) {
    return false;
  }

  return true;
}

boolean GsmModem::deleteSMS(SMS *msg) {
	uint8_t i = msg->smsIdx;
    if (! sendCheckReply(F("AT+CMGF=1"), _ok_reply)) return -1;
  // read an sms
  char sendbuff[12] = "AT+CMGD=000";
  sendbuff[8] = (i / 100) + '0';
  i %= 100;
  sendbuff[9] = (i / 10) + '0';
  i %= 10;
  sendbuff[10] = i + '0';

  return sendCheckReply(sendbuff, _ok_reply, 2000);
}



bool GsmModem::delAllSms(){
	/*orig:
	_mySerial->print(F("at+cmgda=\"del all\"\n\r"));
  _buffer=_readSerial();
  if (_buffer.indexOf("OK")!=-1) {return true;}else {return false;}
  */
	getReply(F("at+cmgda=\"del all\"\n\r"), (uint16_t) 10000);

	for (uint8_t i=0; i<strlen(_replybuffer); i++) {
		PRINT(_replybuffer[i]); //PRINT(" ");
	}
	PRINTLN("");
	return "to be done!";

}

// Retrieve the sender of the specified SMS message and copy it as a string to
// the sender buffer.  Up to senderlen characters of the sender will be copied
// and a null terminator will be added if less than senderlen charactesr are
// copied to the result.  Returns true if a result was successfully retrieved,
// otherwise false.
boolean GsmModem::getSMSSender(uint8_t i, char *sender, int senderlen) {
  // Ensure text mode and all text mode parameters are sent.
  if (! sendCheckReply(F("AT+CMGF=1"), _ok_reply)) return false;
  if (! sendCheckReply(F("AT+CSDH=1"), _ok_reply)) return false;


  PRINT(F("AT+CMGR="));
  PRINTLN(i);


  // Send command to retrieve SMS message and parse a line of response.
  _mySerial->print(F("AT+CMGR="));
  _mySerial->println(i);
  readline(1000);


  PRINTLN(_replybuffer);

  // Parse the second field in the response.
  boolean result = parseReplyQuoted(F("+CMGR:"), sender, senderlen, ',', 1);
  // Drop any remaining data from the response.
  flushInput();
  return result;
}


boolean GsmModem::sendUSSD(char *ussdmsg, char *ussdbuff, uint16_t maxlen, uint16_t *readlen) {
  if (! sendCheckReply(F("AT+CUSD=1"), _ok_reply)) return -1;

  char sendcmd[30] = "AT+CUSD=1,\"";
  strncpy(sendcmd+11, ussdmsg, 30-11-2);  // 11 bytes beginning, 2 bytes for close quote + null
  sendcmd[strlen(sendcmd)] = '\"';

  if (! sendCheckReply(sendcmd, _ok_reply)) {
    *readlen = 0;
    return false;
  } else {
      readline(10000); // read the +CUSD reply, wait up to 10 seconds!!!
      //PRINT("* "); PRINTLN(_replybuffer);
      char *p = strstr_P(_replybuffer, PSTR("+CUSD: "));
      if (p == 0) {
        *readlen = 0;
        return false;
      }
      p+=7; //+CUSD
      // Find " to get start of ussd message.
      p = strchr(p, '\"');
      if (p == 0) {
        *readlen = 0;
        return false;
      }
      p+=1; //"
      // Find " to get end of ussd message.
      char *strend = strchr(p, '\"');

      uint16_t lentocopy = min(maxlen-1, strend - p);
      strncpy(ussdbuff, p, lentocopy+1);
      ussdbuff[lentocopy] = 0;
      *readlen = lentocopy;
  }
  return true;
}

//Get the time  of the base of GSM
String GsmModem::dateNet() {
		/*orig:
  	  _mySerial->print(F("AT+CIPGSMLOC=2,1\r\n "));
  	  _replybuffer = readSerial();

  	  if (_buffer.indexOf("OK")!=-1 ){
    	return _replybuffer.substring(_replybuffer.indexOf(":")+2,(_buffer.indexOf("OK")-4));
  	  } else
  	  return "0";*/
	getReply(F("AT+CIPGSMLOC=2,1\r\n "), (uint16_t) 10000);

	for (uint8_t i=0; i<strlen(_replybuffer); i++) {
		PRINT(_replybuffer[i]); //PRINT(" ");
	}
	PRINTLN("");
	return "to be done!";

}


boolean GsmModem::enableNetworkTimeSync(boolean onoff) {
  if (onoff) {
    if (! sendCheckReply(F("AT+CLTS=1"), _ok_reply))
      return false;
  } else {
    if (! sendCheckReply(F("AT+CLTS=0"), _ok_reply))
      return false;
  }

  flushInput(); // eat any 'Unsolicted Result Code'

  return true;
}


boolean GsmModem::getTime(char *buff, uint16_t maxlen) {
  getReply(F("AT+CCLK?"), (uint16_t) 10000);
  if (strncmp(_replybuffer, "+CCLK: ", 7) != 0)
    return false;

  char *p = _replybuffer+7;
  uint16_t lentocopy = min(maxlen-1, (uint16_t)strlen(p));
  strncpy(buff, p, lentocopy+1);
  buff[lentocopy] = 0;

  readline(); // eat OK

  return true;
}

GsmModem::NetworkStatus GsmModem::getNetworkStatus(void) {
	uint16_t status;

  //sendParseReply(F("AT+COPS=2"), F("+COPS: "), &status, ',', 1);

  if (! sendParseReply(F("AT+CREG?"), F("+CREG: "), &status, ',', 1)){
	  status = 0;
	  return (GsmModem::NetworkStatus)status;
  }


  return  (GsmModem::NetworkStatus)status;
}

bool GsmModem::getListOfOperators(void) {

  //sendParseReply(F("AT+COPS=2"), F("+COPS: "), &status, ',', 1);
	  getReply(F("AT+COPS=?"), (uint16_t) 15000);
	  if (strncmp(_replybuffer, "+COPS: ", 7) != 0)
	    return false;


	  readline();

	  return true;
}




uint8_t GsmModem::getRSSI(void) {
  uint16_t reply;

  if (! sendParseReply(F("AT+CSQ"), F("+CSQ: "), &reply) ) return 0;

  return reply;
}

boolean GsmModem::sendParseReply(const __FlashStringHelper * tosend,
		const __FlashStringHelper * toreply,
				      uint16_t *v, char divider, uint8_t index) {
  getReply(tosend);

  if (! parseReply(toreply, v, divider, index)) return false;

  readline(); // eat 'OK'

  return true;
}

boolean GsmModem::sendCheckReply(char *send, char *reply, uint16_t timeout) {
  if (! getReply(send, timeout) )
	  return false;
/*
  for (uint8_t i=0; i<strlen(_replybuffer); i++) {
  PRINT(_replybuffer[i], HEX); DEBUG_PRINT(" ");
  }
  PRINTLN();
  for (uint8_t i=0; i<strlen(reply); i++) {
    PRINT(reply[i], HEX); DEBUG_PRINT(" ");
  }
  PRINTLN();
  */
  return (strcmp(_replybuffer, reply) == 0);
}

boolean GsmModem::sendCheckReply(const __FlashStringHelper * send, const __FlashStringHelper * reply, uint16_t timeout) {
	if (! getReply(send, timeout) )
		return false;

  //return (prog_char_strcmp(replybuffer, (prog_char*)reply) == 0);
	return (strcmp_P(_replybuffer, (char*)reply) == 0);
}

boolean GsmModem::sendCheckReply(char* send, const __FlashStringHelper *reply, uint16_t timeout) {
  if (! getReply(send, timeout) )
	  return false;
  return (strcmp_P(_replybuffer, (char*)reply) == 0);
}


uint8_t GsmModem::getReply(char *send, uint16_t timeout) {
  flushInput();


  PRINT(F("\t---> "));PRINTLN(send);


  _mySerial->println(send);

  uint8_t l = readline(timeout);

  PRINT(F("\t<--- ")); PRINTLN(_replybuffer);

  return l;
}

uint8_t GsmModem::getReply(const __FlashStringHelper * send, uint16_t timeout) {
  flushInput();


  PRINT(F("\t---> ")); PRINTLN(send);


  _mySerial->println(send);

  uint8_t l = readline(timeout);

  PRINT (F("\t<--- ")); PRINTLN(_replybuffer);

  return l;
}

boolean GsmModem::parseReply(const __FlashStringHelper * toreply,
          uint16_t *v, char divider, uint8_t index) {
  char *p = strstr_P(_replybuffer, (char*)toreply);  // get the pointer to the voltage
  if (p == 0) return false;
  p+=strlen_P((char*)toreply);
  //DEBUG_PRINTLN(p);
  for (uint8_t i=0; i<index;i++) {
    // increment dividers
    p = strchr(p, divider);
    if (!p) return false;
    p++;
    //DEBUG_PRINTLN(p);

  }
  *v = atoi(p);

  return true;
}

// Parse a quoted string in the response fields and copy its value (without quotes)
// to the specified character array (v).  Only up to maxlen characters are copied
// into the result buffer, so make sure to pass a large enough buffer to handle the
// response.
boolean GsmModem::parseReplyQuoted(const __FlashStringHelper * toreply,
          char *v, int maxlen, char divider, uint8_t index) {
  uint8_t i=0, j;
  // Verify response starts with toreply.
  char *p = strstr_P(_replybuffer, (char*)toreply);
  if (p == 0) return false;
  p+=strlen_P((char*)toreply);

  // Find location of desired response field.
  for (i=0; i<index;i++) {
    // increment dividers
    p = strchr(p, divider);
    if (!p) return false;
    p++;
  }

  // Copy characters from response field into result string.
  for(i=0, j=0; j<maxlen && i<strlen(p); ++i) {
    // Stop if a divier is found.
    if(p[i] == divider)
      break;
    // Skip any quotation marks.
    else if(p[i] == '"')
      continue;
    v[j++] = p[i];
  }

  // Add a null terminator if result string buffer was not filled.
  if (j < maxlen)
    v[j] = '\0';

  return true;
}

void GsmModem::flushInput() {
    // Read all available serial input to flush pending data.
    uint16_t timeoutloop = 0;
    while (timeoutloop++ < 40) {
        while(_mySerial->available()) {
        	_mySerial->read();
            timeoutloop = 0;  // If char was received reset the timer
        }
        delay(1);
    }
}

uint16_t GsmModem::readRaw(uint16_t b) {
  uint16_t idx = 0;

  while (b && (idx < sizeof(_replybuffer)-1)) {
    if (_mySerial->available()) {
      _replybuffer[idx] = _mySerial->read();
      idx++;
      b--;
    }
  }
  _replybuffer[idx] = 0;

  return idx;
}


uint8_t GsmModem::readline(uint16_t timeout, boolean multiline) {
  uint16_t replyidx = 0;

  while (timeout--) {
    if (replyidx >= 254) {
      //DEBUG_PRINTLN(F("SPACE"));
      break;
    }

    while(_mySerial->available()) {
      char c =  _mySerial->read();
      if (c == '\r') continue;
      if (c == 0xA) {
        if (replyidx == 0)   // the first 0x0A is ignored
          continue;

        if (!multiline) {
          timeout = 0;         // the second 0x0A is the end of the line
          break;
        }
      }
      _replybuffer[replyidx] = c;
      //PRINT(c, HEX); DEBUG_PRINT("#"); DEBUG_PRINTLN(c);
      replyidx++;
    }

    if (timeout == 0) {
      //PRINTLN(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  _replybuffer[replyidx] = 0;  // null term
  return replyidx;
}


//} /* namespace Gardena */
