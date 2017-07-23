/**
 *  @file		GardenaLib.ino
 *
 *  @author		maro
 *  @date		mm/dd/yyyy
 *  @version	x.y
 *
 *  @brief 		Main
 *
 *  @section DESCRIPTION
 *
 *  control gardena watering valves through gsm sms
 *
 */
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

/**
 * 1. Each Object shall have a set of commands. The command list holds a reference to each one of these
 */
//interesting, look at this: https://stackoverflow.com/questions/4688055/create-menus-in-command-line
CommandList cmdList;


/**
 * 2. Setup Communication
 * 2.1 Gardena needs a gsm modem to communicate
 */
#define SIM800L 	Serial1
#define SIM800L_RST	2
GsmModem gsm(&SIM800L,SIM800L_RST);

/**
 * 2.2 Gardena can also be controled through a "Wifi-Modem"= Web Interface
 */
#define ESP01		Serial2
#define ESP01_RST NONE //defined in state.h
WifiModem wifi(&ESP01, ESP01_RST,115200);
//wifiUser

/**
 * 2.3 Gardena  controled through serial port
 */
DebugModem debug(115200);

/**
 * 3. subscriber gets a:
 *  -gsm modem (->so she can take care of watchdog functionality;e.g. trigger modem reset in case of modem stall)
 *  -and a list/instruction set of Commands (->to control Gardena setup)
 */
Subscriber subscriber(&gsm, &wifi,&debug, &cmdList);



/**
 * 4. Instantiate and register all Gardena Objects
 * (cmdList as reference to be able to register the possible commands)
 */

/**
 * The Valves
 */
//Relay contact assignment/h-bridge "mechanics":
const int K1 = 19; //forward 2 (ON)
const int K2 = 18; //= H_BRIDGE_FORWARD_CONTACT1 (ON)
const int K3 = 17; //reverse 2 (OFF)
const int K4 = 16; //= H_BRIDGE_REVERSE_CONTACT1 (OFF)
const int K5 = 15; //forward 2 (ON)
const int K6 = 14; //reverse 2 (OFF)
const int K7 = 12; //forward 2 (ON)
const int K8 = 11; //reverse 2 (OFF)

Relay relay1(K1);
Relay relay2(K2);
Relay relay3(K3);
Relay relay4(K4);
Relay relay5(K5);
Relay relay6(K6);
Relay relay7(K7);
Relay relay8(K8);
HBridge hbridge1(&relay2,&relay4,&relay1,&relay3);//look at drawing!
HBridge hbridge2(&relay2,&relay4,&relay5,&relay6);//look at drawing!
HBridge hbridge3(&relay2,&relay4,&relay7,&relay8);//look at drawing!
//Valve valve[] = {Valve(&hbridge1,ON,"valve1",&cmdList)};//,Valve(&hbridge2,ON,"valve2",&cmdList),Valve(&hbridge3,ON,"valve3",&cmdList)
Valve valve1(&hbridge1,OFF,"valve1",&cmdList);
Valve valve2(&hbridge2,OFF,"valve2",&cmdList);
Valve valve3(&hbridge3,OFF,"valve3",&cmdList);


/**
 * The Rainsensor
 */
RainSensor rain("rain",A8);

/**
 * The moist sensor
 */
MoistureSensor moist("moist",A9);

/**
 * The water flow sensor
 */
#define WATER_FLOW_SENSOR_PIN 3
WaterFlowSensor waterflow("flow",WATER_FLOW_SENSOR_PIN);

/**
 * The Temperature Sensor
 */
#define TEMPERATURE_SENSOR_PIN 4
TemperatureSensor temperature("temp",TEMPERATURE_SENSOR_PIN);


/**
 * The Humidity Sensor
 */
HumiditySensor humidity("humidity",&temperature);

/**
 * Time
 */
Clock clock("clock");

/**
 * Timer
 *
 * e.g. one per valve
 *
 */
Timer timer1("timer1", &clock,&cmdList);
Timer timer2("timer2", &clock,&cmdList);
Timer timer3("timer3", &clock,&cmdList);

void setup()
{
	//initialization code here

	debug.begin();//PRINT_INIT(115200);
	Serial.println("::::::::::::::::::::::::::::::::::");
	//delay(20000);
	PRINTLN("::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");
	gsm.begin();
	wifi.begin();
	subscriber.begin();
	valve1.begin();
	valve2.begin();
	valve3.begin();
	rain.begin();
	moist.begin();
	waterflow.begin();
	temperature.begin();
	humidity.begin();
	clock.begin();
	timer1.begin();
	timer2.begin();
	timer3.begin();
	distributeCmdList();
	createDependencies();


}


void loop()
{

	debug.handle();
	gsm.handle();//checks for incomming events -> SMSs
	wifi.handle();//checks for web requests
	subscriber.handle();//watchdog functionality and processing of arrived SMSs
	rain.handle();
	moist.handle();
	waterflow.handle();
	temperature.handle();
	humidity.handle();
	clock.handle();
	valve1.handle();
	valve2.handle();
	valve3.handle();
	timer1.handle();
	timer2.handle();
	timer3.handle();

}

//also measured objects should register to cmdList
//in order be able to request their values or set conditions via sms or other UI
void distributeCmdList(){
	rain.setCommandList(&cmdList);
	moist.setCommandList(&cmdList);
	waterflow.setCommandList(&cmdList);
	temperature.setCommandList(&cmdList);
	humidity.setCommandList(&cmdList);

}

//reference to valve given, for threshold based switching of valves. Eg if command is "switch valve on"
//shall be if moist is below a certain threshold, the valve object needs to have a pointer to the measured moist value
void createDependencies(){
	//subscriber.subscribe(&clock);<-not needed, time comes from now()
	//valves switched on/off based on measured values:
	valve1.subscribe(&rain);
	valve2.subscribe(&rain);
	valve3.subscribe(&rain);
	//-----------------------------
	valve1.subscribe(&moist);
	valve2.subscribe(&moist);
	valve3.subscribe(&moist);
	//-----------------------------
	//TODO: Is there conditional swtching necessary for amount of water flow?
	//valve[0].addDependency(&waterflow);
	//valve[1].addDependency(&waterflow);
	//valve[2].addDependency(&waterflow);
	//-----------------------------
	valve1.subscribe(&temperature);
	valve2.subscribe(&temperature);
	valve3.subscribe(&temperature);
	//-----------------------------
	valve1.subscribe(&humidity);
	valve2.subscribe(&humidity);
	valve3.subscribe(&humidity);
	//-----------------------------
	valve1.subscribe(&timer1);
	valve1.subscribe(&timer2);
	valve1.subscribe(&timer3);
	//-----------------------------
	valve2.subscribe(&timer1);
	valve2.subscribe(&timer2);
	valve2.subscribe(&timer3);
	//-----------------------------
	valve3.subscribe(&timer1);
	valve3.subscribe(&timer2);
	valve3.subscribe(&timer3);
	//-----------------------------
	//-----------------------------
	//for periodically or time based reporting:(this is obsolete!!! - better to use e.g "subriber timerX" command)
	//rain.addDependency(&time);
	//-----------------------------
	//moist.addDependency(&time);
	//-----------------------------
	//waterflow.addDependency(&time);
	//-----------------------------
	//temperature.addDependency(&time);

}


