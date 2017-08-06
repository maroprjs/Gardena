/*
 * Gardena.h
 *
 *  Created on: 23.07.2017
 *      Author: pionet
 */

#ifndef GARDENA_H_
#define GARDENA_H_

//namespace GARDENA_TEENSY {

//#include <Arduino.h>
#include "Relay.h"
#include "HBridge.h"
#include "Valve.h"
#include "GsmModem.h"
#include "WifiModem.h"
#include "DebugModem.h"
#include "Subscriber.h"
#include "RainSensor.h"
#include "MoistureSensor.h"
#include "WaterFlowSensor.h"
#include "TemperatureSensor.h"
#include "HumiditySensor.h"
#include "Clock.h"
#include "Timer.h"
#include "Debug.h"

//GSM modem:
#define SIM800L 	Serial1
#define SIM800L_RST	2

//ESP01 (wifi modem)
#define ESP01		Serial2
#define ESP01_RST NONE //defined in state.h

//Valves:
//Relay contact assignment/h-bridge "mechanics":
const int K1 = 19; //forward 2 (ON)
const int K2 = 18; //= H_BRIDGE_FORWARD_CONTACT1 (ON)
const int K3 = 17; //reverse 2 (OFF)
const int K4 = 16; //= H_BRIDGE_REVERSE_CONTACT1 (OFF)
const int K5 = 15; //forward 2 (ON)
const int K6 = 14; //reverse 2 (OFF)
const int K7 = 12; //forward 2 (ON)
const int K8 = 11; //reverse 2 (OFF)

//water flow sensor:
#define WATER_FLOW_SENSOR_PIN 3

//temperature
#define TEMPERATURE_SENSOR_PIN 4




class Gardena {
public:
	Gardena();
	virtual ~Gardena();
	void begin();
	void handle();

private:
	void distributeCmdList();
	void createDependencies();
	CommandList * _cmdList;
	GsmModem* _gsm;
	WifiModem* _wifi;
	DebugModem* _debug;
	Subscriber* _subscriber;
	Relay* _relay1;
	Relay* _relay2;
	Relay* _relay3;
	Relay* _relay4;
	Relay* _relay5;
	Relay* _relay6;
	Relay* _relay7;
	Relay* _relay8;
	HBridge* _hbridge1;
	HBridge* _hbridge2;
	HBridge* _hbridge3;
	Valve* _valve1;
	Valve* _valve2;
	Valve* _valve3;
	RainSensor* _rain;
	MoistureSensor* _moist;
	WaterFlowSensor* _waterflow;
	TemperatureSensor* _temperature;
	HumiditySensor* _humidity;
	Clock* _clock;
	Timer* _timer1;
	Timer* _timer2;
	Timer* _timer3;
};

//} /* namespace GARDENA_TEENSY */

#endif /* GARDENA_H_ */
