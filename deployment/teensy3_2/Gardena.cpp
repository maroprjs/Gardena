/*
 * Gardena.cpp
 *
 *  Created on: 23.07.2017
 *      Author: pionet
 */

#include "Gardena.h"

//namespace GARDENA_TEENSY {

Gardena::Gardena() {
	/**
	 * 1. Each Object shall have a set of commands. The command list holds a reference to each one of these
	 */
	//interesting, look at this: https://stackoverflow.com/questions/4688055/create-menus-in-command-line
	_cmdList = new CommandList();


	/**
	 * 2. Setup Communication
	 * 2.1 Gardena needs a gsm modem to communicate
	 */
	_gsm = new GsmModem(&SIM800L,SIM800L_RST);

	/**
	 * 2.2 Gardena can also be controled through a "Wifi-Modem"= Web Interface
	 */
	_wifi = new WifiModem(&ESP01, ESP01_RST,115200);

	/**
	 * 2.3 Gardena  controled through serial port
	 */
	_debug = new DebugModem(115200);

	/**
	 * 3. subscriber gets a:
	 *  -gsm modem (->so she can take care of watchdog functionality;e.g. trigger modem reset in case of modem stall)
	 *  -and a list/instruction set of Commands (->to control Gardena setup)
	 */
	_subscriber = new Subscriber(_gsm, _wifi,_debug, _cmdList);

	/**
	 * 4. Instantiate and register all Gardena Objects
	 * (cmdList as reference to be able to register the possible commands)
	 */

	/**
	 * The Valves
	 */
	_relay1 = new Relay(K1);
	_relay2 = new Relay(K2);
	_relay3 = new Relay(K3);
	_relay4 = new Relay(K4);
	_relay5 = new Relay(K5);
	_relay6 = new Relay(K6);
	_relay7 = new Relay(K7);
	_relay8 = new Relay(K8);
	_hbridge1 = new HBridge(_relay2,_relay4,_relay1,_relay3);//look at drawing!
	_hbridge2 = new HBridge(_relay2,_relay4,_relay5,_relay6);//look at drawing!
	_hbridge3 = new HBridge(_relay2,_relay4,_relay7,_relay8);//look at drawing!
	//Valve valve[] = {Valve(&hbridge1,ON,"valve1",&cmdList)};//,Valve(&hbridge2,ON,"valve2",&cmdList),Valve(&hbridge3,ON,"valve3",&cmdList)
	_valve1 = new Valve(_hbridge1,OFF,"valve1",_cmdList);
	_valve2 = new Valve(_hbridge2,OFF,"valve2",_cmdList);
	_valve3 = new Valve(_hbridge3,OFF,"valve3",_cmdList);


	/**
	 * The Rainsensor
	 */
	_rain = new RainSensor("rain",A8);

	/**
	 * The moist sensor
	 */
	_moist = new MoistureSensor("moist",A9);

	/**
	 * The water flow sensor
	 */
	_waterflow = new WaterFlowSensor("flow",WATER_FLOW_SENSOR_PIN);

	/**
	 * The Temperature Sensor
	 */
	_temperature = new TemperatureSensor("temp",TEMPERATURE_SENSOR_PIN);


	/**
	 * The Humidity Sensor
	 */
	_humidity = new HumiditySensor("humidity",_temperature);

	/**
	 * Time
	 */
	_clock = new Clock("clock");

	/**
	 * Timer
	 *
	 * e.g. one per valve
	 *
	 */
	_timer1 = new Timer("timer1", _clock,_cmdList);
	_timer2 = new Timer("timer2", _clock,_cmdList);
	_timer3 = new Timer("timer3", _clock,_cmdList);

}

Gardena::~Gardena() {
	// TODO Auto-generated destructor stub
}

void Gardena::begin(){
	_debug->begin();//PRINT_INIT(115200);
	Serial.println("::::::::::::::::::::::::::::::::::");
	//delay(20000);
	PRINTLN("::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");
	_gsm->begin();
	_wifi->begin();
	_subscriber->begin();
	_valve1->begin();
	_valve2->begin();
	_valve3->begin();
	_rain->begin();
	_moist->begin();
	_waterflow->begin();
	_temperature->begin();
	_humidity->begin();
	_clock->begin();
	_timer1->begin();
	_timer2->begin();
	_timer3->begin();
	distributeCmdList();
	createDependencies();
}

void Gardena::handle(){
	_debug->handle();
	_gsm->handle();//checks for incomming events -> SMSs
	_wifi->handle();//checks for web requests
	_subscriber->handle();//watchdog functionality and processing of arrived SMSs
	_rain->handle();
	_moist->handle();
	_waterflow->handle();
	_temperature->handle();
	_humidity->handle();
	_clock->handle();
	_valve1->handle();
	_valve2->handle();
	_valve3->handle();
	_timer1->handle();
	_timer2->handle();
	_timer3->handle();
}

//also measured objects should register to cmdList
//in order be able to request their values or set conditions via sms or other UI
void Gardena::distributeCmdList(){
	_rain->setCommandList(_cmdList);
	_moist->setCommandList(_cmdList);
	_waterflow->setCommandList(_cmdList);
	_temperature->setCommandList(_cmdList);
	_humidity->setCommandList(_cmdList);

}

//reference to valve given, for threshold based switching of valves. Eg if command is "switch valve on"
//shall be if moist is below a certain threshold, the valve object needs to have a pointer to the measured moist value
void Gardena::createDependencies(){
	//subscriber.subscribe(&clock);<-not needed, time comes from now()
	//valves switched on/off based on measured values:
	_valve1->subscribe(_rain);
	_valve2->subscribe(_rain);
	_valve3->subscribe(_rain);
	//-----------------------------
	_valve1->subscribe(_moist);
	_valve2->subscribe(_moist);
	_valve3->subscribe(_moist);
	//-----------------------------
	//TODO: Is there conditional swtching necessary for amount of water flow?
	//valve[0].addDependency(&waterflow);
	//valve[1].addDependency(&waterflow);
	//valve[2].addDependency(&waterflow);
	//-----------------------------
	_valve1->subscribe(_temperature);
	_valve2->subscribe(_temperature);
	_valve3->subscribe(_temperature);
	//-----------------------------
	_valve1->subscribe(_humidity);
	_valve2->subscribe(_humidity);
	_valve3->subscribe(_humidity);
	//-----------------------------
	_valve1->subscribe(_timer1);
	_valve1->subscribe(_timer2);
	_valve1->subscribe(_timer3);
	//-----------------------------
	_valve2->subscribe(_timer1);
	_valve2->subscribe(_timer2);
	_valve2->subscribe(_timer3);
	//-----------------------------
	_valve3->subscribe(_timer1);
	_valve3->subscribe(_timer2);
	_valve3->subscribe(_timer3);
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


//} /* namespace GARDENA_TEENSY */
