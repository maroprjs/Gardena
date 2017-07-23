/**
 *  @file		RainSensor.cpp
 *
 *  @author		maro
 *  @date 		03.07.2017
 *  @version	x.y
 *
 *  @brief 		"Driver" for Gardena Rain Sensor 1189-20
 *
 *  @section DESCRIPTION
 *
 *  resistance changes depending on amount of rain
 *
 */

#include "RainSensor.h"
#include "Debug.h"
#include <TimeLib.h>



RainSensor::RainSensor(String alias, int sensorPin, CommandList * cmdList, uint32_t interval) {
	name = alias;
	_sensorPin = sensorPin;
	_measurementInterval = interval;
	initializeSensorThresholds();
	_cmd = new Command();
	_cmdList = cmdList;
	_cmdList->registerCmd(_cmd);
	//TODO: initialize rain specific commands
}

RainSensor::RainSensor(String alias, int sensorPin, uint32_t interval) {
	name = alias;
	_sensorPin = sensorPin;
	_measurementInterval = interval;
	initializeSensorThresholds();
}

RainSensor::~RainSensor() {
	// TODO Auto-generated destructor stub
}


void RainSensor::initializeSensorThresholds(){//used by common measurement class
	_defaultConditions.gradient = RAIN_DEFAULT_CONDITIONAL_GRAD;
	_defaultConditions.threshold_high = RAIN_DEFAULT_CONDITIONAL_THR_HIGH;
	_defaultConditions.threshold_low = RAIN_DEFAULT_CONDITIONAL_THR_LOW;
	_defaultConditions.requiredActivity = RAIN_DEFAULT_REQUIRED_ACTIVITY;

	_sensorDetectConditions.hysteresis.threshold_high = RAIN_SENSOR_ATTACHED_THRESHOLD_HIGH;
	_sensorDetectConditions.hysteresis.threshold_low = RAIN_SENSOR_ATTACHED_THRESHOLD_LOW;
	_sensorDetectConditions.hysteresis.gradient = RAIN_SENSOR_ATTACHED_GRADIENT;
}


/*implemented in common measurement class
void RainSensor::begin(){
	PRINTLN("RainSensor::begin() called");
	_measurementPeriode = 0;
}

void RainSensor::handle() {
	if (_measurementPeriode >= MEASUREMENT_INTERVAL) {
		_measurementPeriode = _measurementPeriode - MEASUREMENT_INTERVAL;
		PRINT("Rain: ");
		PRINTLN(analogRead(_sensorPin));
	}

}
*/
