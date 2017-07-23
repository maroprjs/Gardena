/*
 * Timer.cpp
 *
 *  Created on: 16.07.2017
 *      Author: pionet
 */

#include "Timer.h"
#include "Debug.h"

Timer::Timer(String alias,Clock* clock, CommandList * cmdList, uint32_t interval) {
	name = alias;
	_measurementInterval = interval;
	initializeSensorThresholds();
	_cmd = new Command();
	_cmdList = cmdList;
	_cmdList->registerCmd(_cmd);
	_clock = clock;

}

Timer::~Timer() {
	// TODO Auto-generated destructor stub
}

void Timer::initializeSensorThresholds(){//used by common measurement class
	_defaultConditions.gradient = TIMER_DEFAULT_CONDITIONAL_GRAD;
	_defaultConditions.threshold_high = TIMER_DEFAULT_CONDITIONAL_THR_HIGH;
	_defaultConditions.threshold_low = TIMER_DEFAULT_CONDITIONAL_THR_LOW;
	_defaultConditions.requiredActivity = TIMER_DEFAULT_REQUIRED_ACTIVITY;

	_sensorDetectConditions.hysteresis.threshold_high = TIMER_SENSOR_ATTACHED_THRESHOLD_HIGH;
	_sensorDetectConditions.hysteresis.threshold_low = TIMER_SENSOR_ATTACHED_THRESHOLD_LOW;
	_sensorDetectConditions.hysteresis.gradient = TIMER_SENSOR_ATTACHED_GRADIENT;
}

bool Timer::sensorDetected(){
	PRINTLN("sensor dtect functzion timer");
	bool retVal = false;
	if (_sensorDetected == true)retVal = true;
	return retVal;
}


void Timer::begin(){
	PRINT(name);PRINTLN("::begin() called");
	_elapsedTime = 0;
	_objStatusTxt = "";
	_sensorDetected = _clock->clockInSync();
}

void Timer::handle(){

   if (_elapsedTime >= _measurementInterval) {
	   _elapsedTime = _elapsedTime - _measurementInterval;
	   //TODO: get time
   }
}

