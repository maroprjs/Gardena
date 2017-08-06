/*
 * Subscriber.cpp
 *
 *  Created on: 04.06.2017
 *      Author: maro
 */

#include "Subscriber.h"
#include "Clock.h"
#include "Debug.h"
#include <ArduinoJson.h>

//////////////////////////////////////////////////////
//TODO: make this watchdog more elegant:
//---------------------------------------------------
volatile int gsmSerialCheckedNokCount = 0;
volatile int gsmNetworkNokCount = 0;
volatile int watchdogWakeUpCount = 0;
volatile bool gsmResetFlag = false;
volatile bool roamFlag = false;

void gsmAliveCheck(){//counters increased here, reseted in healthcheck function if healthcheck ok
	watchdogWakeUpCount++;
	if (watchdogWakeUpCount % ALIVE_CHECK_INTERVAL != 0) return;//because watchdog interval needs to be quite small(due to "int"-type), but healthcheck not needed THAT often
	PRINT("Subscriber::gsmAliveCheck() called... ");
	watchdogWakeUpCount = 0;
	gsmSerialCheckedNokCount++;
	if (gsmSerialCheckedNokCount >= MAX_GSM_SERIAL_NOKS){
		//_gsm->reset();
		gsmResetFlag = true;//This flag evaluated in looped handle()-function
	}
	gsmNetworkNokCount ++;
	if (gsmNetworkNokCount % MAX_GSM_NWS_NOKS_FOR_ROAMING == 0) roamFlag = true;
	if (gsmNetworkNokCount % MAX_GSM_NWS_NOKS == 0){
		//_gsm->reset();
		gsmResetFlag = true;//This flag evaluated in looped handle()-function

	}

	//ToDo: network status count

	PRINT("Result-> serialNok: ");PRINT(gsmSerialCheckedNokCount - 1 );
	PRINT("   networkNok: ");PRINTLN(gsmNetworkNokCount - 1 );
}
////////////////////////////////////////////////////////

Subscriber::Subscriber() {
	_gsm = NULL;
	_cmdList = NULL;
	_wifi = NULL;
	//_activeClock = NULL;
	_debug = NULL;
}

Subscriber::Subscriber(GsmModem * gsm, WifiModem* wifi, CommandList * cmdList) {
	_gsm = gsm;
	_cmdList = cmdList;
	_wifi = wifi;
	//_activeClock = NULL;
	_debug = NULL;
}

Subscriber::Subscriber(GsmModem * gsm, WifiModem* wifi, DebugModem* debug, CommandList * cmdList) {
	_gsm = gsm;
	_cmdList = cmdList;
	_wifi = wifi;
	_debug = debug;
	//_activeClock = NULL;
}

Subscriber::~Subscriber() {
	// TODO Auto-generated destructor stub
}

//subscribe for a certain measurement
void Subscriber::subscribe(Measurement * measurement){
	Measurement::measurementDependency_t md;
	md.measurement = measurement;
	md.hysteresis.gradient = Measurement::gradient_t::NOT_MEASURED;
	_measurementDependencyVector.push_back(md);//not really needed, cause I don't want automate anything here
	// now these dependencies are added, so I can extend the command list,
	//switching valves based on measured thresholds
	//if(md.measurement->name == "clock"){
	//	_activeClock = (Clock*)measurement;
	//}

}

void Subscriber::begin(){
	PRINTLN("Subscriber::begin() called");
	//_gsmWatchDog.begin(gsmAliveCheck,ALIVE_CHECK_INTERVAL);<-first iteration test
	_gsmWatchDog.begin(gsmAliveCheck,WAKE_UP_WATCHDOG_INTERVAL);
	//if (_gsm->heartBeat()) gsmSerialCheckedNokCount = 0;
	//else PRINTLN("_gsm->heartBeat() NOK");
}


void Subscriber::handle(){
	String swInfo = ZVERSION;
	if(gsmResetFlag)_gsm->reset(); gsmResetFlag = false;//TODO: check if gsmhealthcheck should be disabled for some time after reset!
    if(roamFlag)_gsm->roam();roamFlag = false;
    if (_gsmHealthCheckTime >= GSM_HEALTH_CHECK_INTERVAL) gsmHealthCheck();
	if (_gsm->sms->newMsgArrived){/*defined in base class "subscriber"*/
		processSMS(swInfo);
	}
	if (_wifi->webService->newMsgArrived){/*defined in base class "subscriber"*/
		processWebRequest(swInfo);
	}
	if (_debug->dbgService->newMsgArrived){/*defined in base class "subscriber"*/
		processDbgRequest(swInfo);
	}
	if (_elapsedTime >= SENSOR_CHECK_INTERVAL) sensorCheck();

}


void Subscriber::processSMS(String info){
	PRINTLN("in Subscriber::handle() msg arrived");
	String reply = "";
	String cmd = "";
	_gsm->sms->newMsgArrived = false;//message read
	_gsm->deleteSMS(_gsm->sms);//from device buffer
	//call cmd->execute:
	cmd = _gsm->sms->msg;
	_gsm->sms->msg = "";
	reply = _cmdList->iterate(_gsm->sms, cmd);
	//TODO: reply in json format: do U need it here?
	//if (reply == "") reply = "cmd not recognized!";//TODO: offer help menu
	if (_gsm->sms->msg == "") _gsm->sms->msg = "cmd not recognized!";
	//reply to sender:
	_gsm->sms->receiver = _gsm->sms->sender;
	PRINT("reply sms: ");PRINTLN(_gsm->sms->msg);
	_gsm->sendSMS(_gsm->sms);

}

void Subscriber::processWebRequest(String info){
	DynamicJsonBuffer jsonRecBuffer;
	DynamicJsonBuffer jsonSendBuffer;
	String tmpStrg = "";
	String webRequestStatus = "OK";
	JsonObject& jsonRequest = jsonRecBuffer.parseObject(_wifi->webService->msg);
	_wifi->webService->jsonMsg = &jsonSendBuffer.createObject();
	_wifi->webService->jsonData = &_wifi->webService->jsonMsg->createNestedArray("data");
	_wifi->webService->newMsgArrived = false;//message read
	 if (jsonRequest.success()){
		 //String str = "";
		 if (jsonRequest.containsKey("cmd")){
		     String cmd = jsonRequest["cmd"];
		     PRINT(" command found: ");PRINTLN(cmd);
		     _wifi->webService->msg = "";
		     tmpStrg = _cmdList->iterate(_wifi->webService,cmd);
		     if (_wifi->webService->msg == "") _wifi->webService->msg = "cmd not recognized!";//TODO: offer help menu
		     PRINT("sms reply: ");PRINTLN(_wifi->webService->msg);

		 }
	 }
	 //else _wifi->webService->msg = "no json format in web request recognized!";webRequestStatus = "NOK";
	 //reply to sender:
	 //{"scope":"valves", "data":{"valve1":{"status":"on","info":"rain and moist"},"valve2":{"status":"off","info":"man"}}}
	 //jsonReply["status"] = webRequestStatus;
	 //jsonReply["scope"] = jsonRequest["scope"];
	 //jsonReply["data"] = tmpStrg;
	 //tmpStrg = "{\"scope\":\"valves\",\"data\":{" + tmpStrg ;
	 String s = jsonRequest["scope"];
	 String st = String(now());
	 tmpStrg = "{\"status\":\""+ webRequestStatus + "\",\"scope\":\""+ s + "\",\"data\":{" + tmpStrg  + "},\"timestamp\":\"" + st + "\",\"swversion\":\"" + info + "\"}";
     //"scope"+ "\"" + ":" +"\"" + "valves" + "\"" + "," + "\"" + "data" + "\"" + ":{" + tmpStrg;
	 PRINT("reply web: ");PRINTLN(tmpStrg);

	// jsonReply["msg"] = _wifi->webService->msg;
	// jsonReply["receiver"] = _wifi->webService->sender;
	// jsonReply["sender"] = _wifi->webService->receiver;
	 //jsonReply["valve1"] = "on";
	 //jsonReply["valve2"] = "off";
	 //jsonReply["valve3"] = "on";
	 _wifi->sendWebJsonMsg(_wifi->webService->jsonMsg);
	 _wifi->sendWebStrgMsg(tmpStrg);

}

void Subscriber::processDbgRequest(String info){
	DynamicJsonBuffer jsonRecBuffer;
	DynamicJsonBuffer jsonSendBuffer;
	//str = _wifi->webService->msg;
	String tmpStrg = "";
	String cmd = _debug->dbgService->msg;
	//JsonObject& jsonRequest = jsonRecBuffer.parseObject(_wifi->webService->msg);
	// JsonObject& jsonReply = jsonSendBuffer.createObject();
	 String dbgRequestStatus = "OK";
	_debug->dbgService->newMsgArrived = false;//message read
	 //if (jsonRequest.success()){
		 //String str = "";
	//	 if (jsonRequest.containsKey("cmd")){
	//	     String cmd = jsonRequest["cmd"];
	//	     PRINT(" command found: ");PRINTLN(cmd);
		     //_wifi->webService->msg = _cmdList->iterate(_wifi->webService,cmd);
			 _debug->dbgService->msg = "";
		     tmpStrg = _cmdList->iterate(_debug->dbgService,cmd);
		     if (_debug->dbgService->msg == "") _debug->dbgService->msg = "cmd not recognized!";
		     PRINT("SMS message: ");PRINTLN(_debug->dbgService->msg);
		 	//if (_wifi->webService->msg == "") _wifi->webService->msg = "cmd not recognized!";//TODO: offer help menu

	//	 }
	// }

	 String s = "dummy";//jsonRequest["scope"];
	 String st = String(now());
	 tmpStrg = "{\"status\":\""+ dbgRequestStatus + "\",\"scope\":\""+ s + "\",\"data\":{" + tmpStrg  + "},\"timestamp\":\"" + st + "\"}";
     //"scope"+ "\"" + ":" +"\"" + "valves" + "\"" + "," + "\"" + "data" + "\"" + ":{" + tmpStrg;
	 PRINT("web reply: ");PRINTLN(tmpStrg);

	 _debug->sendDbgStrgMsg(tmpStrg);

}

void Subscriber::gsmHealthCheck(){
		_gsmHealthCheckTime = _gsmHealthCheckTime - GSM_HEALTH_CHECK_INTERVAL;
		if (_gsm->heartBeat()) gsmSerialCheckedNokCount = 0;
		PRINTLN("========== GsmModem::getNetworkStatus() check begin ========>");
		GsmModem::NetworkStatus nws = _gsm->getNetworkStatus();
		switch (nws){//if serial "ok", but network status not, modem shall be rested after some counts
			case GsmModem::NetworkStatus::RegisteredHome: gsmNetworkNokCount = 0; break;
			case GsmModem::NetworkStatus::RegisteredRoaming: gsmNetworkNokCount = 0;  break;
			default: {
				PRINT("Modem not registered! NWStatus: ");PRINTLN(nws);
				if (_gsm->getListOfOperators()){//scan might help to re-attach
					PRINTLN("found operators");
					_gsmOperatorsNokCount = 0;
				}else{
					PRINTLN("Operator Scanning not succesfull....increasing negative counter: ");
					_gsmOperatorsNokCount++;//just informational
					PRINT(_gsmOperatorsNokCount); PRINTLN(" x consecutive attempts unsuccessful!");
				}
			}
		}
		PRINTLN("<========== GsmModem::getNetworkStatus() check end ========");

}

//TODO: implement here the conditions to report a measurement to gsm subscriber when certain thresholds are
//reached.imiliar like in Valve-class
void Subscriber::sensorCheck(){
		_elapsedTime = _elapsedTime - SENSOR_CHECK_INTERVAL;


}
