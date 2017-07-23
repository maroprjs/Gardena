/*
 * Subscriber.h
 *
 *  Created on: 04.06.2017
 *      Author: maro
 */

#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_
#include <Arduino.h>
#include "GsmModem.h"
#include "WifiModem.h"
#include "DebugModem.h"
#include "CommandList.h"
#include "Measurement.h"
#include "Clock.h"


/*first iteration test values:
	#define MAX_GSM_SERIAL_NOKS 60 //that high, because TimerInterval callback can't take that high numbers and healthcheck is only once per minute
	#define MAX_GSM_NWS_NOKS 60 //that, because TimerInterval callback can't take that high numbers and healthcheck is only once per minute
	#define MAX_GSM_NWS_NOKS_FOR_ROAMING 50//TODO: this functionality is incomplete, must be implemented properly; and find good value
	#define ALIVE_CHECK_INTERVAL 10000000 //[us] = 10s Can't be much bigger!!!
	#define GSM_HEALTH_CHECK_INTERVAL  60000 //[ms]
*/
	#define MAX_GSM_SERIAL_NOKS 30 //equals 30minutes if watchdog interval is 10s and alive check every 6th interval
	#define MAX_GSM_NWS_NOKS 30 ////equals 30minutes if watchdog interval is 10s and alive check every 6th interval
	#define MAX_GSM_NWS_NOKS_FOR_ROAMING 50//TODO: this functionality is incomplete, must be implemented properly; and find good value
	#define WAKE_UP_WATCHDOG_INTERVAL 10000000 //[us] = 10s Can't be much bigger!!!
	#define ALIVE_CHECK_INTERVAL 6 //equals GSM_HEALTH_CHECK_INTERVAL in [s]/WAKE_UP_WATCHDOG_INTERVAL in [s]
	#define GSM_HEALTH_CHECK_INTERVAL  60000 //[ms]
	#define SENSOR_CHECK_INTERVAL 10000 //[ms]


class Subscriber {
public:
	Subscriber();
	Subscriber(GsmModem * gsm, WifiModem* wifi, CommandList * cmdList);
	Subscriber(GsmModem * gsm, WifiModem* wifi, DebugModem* debug, CommandList * cmdList);
	virtual ~Subscriber();
	void subscribe(Measurement * measurement);
	void begin();
	void handle();

private://functions
	void gsmHealthCheck();
	void processSMS(String info);
	void processWebRequest(String info);
	void processDbgRequest(String info);
	void sensorCheck();
	//void gsmAliveCheck();
private://variables
	GsmModem * _gsm;
	WifiModem * _wifi;
	DebugModem * _debug;
    CommandList * _cmdList;
	IntervalTimer _gsmWatchDog;
	int _gsmOperatorsNokCount = 0;//just informational
	elapsedMillis _gsmHealthCheckTime = 0;
	elapsedMillis _elapsedTime = 0;
	Measurement::measurementDependencyVector_t _measurementDependencyVector;
	//Clock *_activeClock;

};

#endif /* SUBSCRIBER_H_ */
