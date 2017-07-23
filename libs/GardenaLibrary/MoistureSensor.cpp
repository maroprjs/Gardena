/**
 *  @file		MoistureSensor.cpp
 *
 *  @author		maro
 *  @date 		03.07.2017
 *  @version	x.y
 *
 *  @brief 		"Driver" for Groove Moisture Sensor
 *
 *  @section DESCRIPTION
 *
 *  resistance changes depending on amount of moist
 *
 */

#include "MoistureSensor.h"
#include "Debug.h"

MoistureSensor::MoistureSensor(String alias, int sensorPin, CommandList * cmdList, uint32_t interval) {
	name = alias;
	_sensorPin = sensorPin;
	_measurementInterval = interval;
	initializeSensorThresholds();
	_cmd = new Command();
	_cmdList = cmdList;
	_cmdList->registerCmd(_cmd);
	//TODO: initialize moist specific commands
}
MoistureSensor::MoistureSensor(String alias, int sensorPin, uint32_t interval) {
	name = alias;
	_sensorPin = sensorPin;
	_measurementInterval = interval;
	initializeSensorThresholds();

}

MoistureSensor::~MoistureSensor() {
	// TODO Auto-generated destructor stub
}

void MoistureSensor::initializeSensorThresholds(){//used by common measurement class
	_defaultConditions.gradient = MOIST_DEFAULT_CONDITIONAL_GRAD;
	_defaultConditions.threshold_high = MOIST_DEFAULT_CONDITIONAL_THR_HIGH;
	_defaultConditions.threshold_low = MOIST_DEFAULT_CONDITIONAL_THR_LOW;
	_defaultConditions.requiredActivity = MOIST_DEFAULT_REQUIRED_ACTIVITY;

	_sensorDetectConditions.hysteresis.threshold_high = MOIST_SENSOR_ATTACHED_THRESHOLD_HIGH;
	_sensorDetectConditions.hysteresis.threshold_low = MOIST_SENSOR_ATTACHED_THRESHOLD_LOW;
	_sensorDetectConditions.hysteresis.gradient = MOIST_SENSOR_ATTACHED_GRADIENT;
}





/*
void MoistureSensor::begin(){
	PRINTLN("MoistureSensor::begin() called");
	_measurementPeriode = 0;
}

void MoistureSensor::handle() {
	if (_measurementPeriode >= DEFAULT_MEASUREMENT_INTERVAL) {
		_measurementPeriode = _measurementPeriode - DEFAULT_MEASUREMENT_INTERVAL;
		PRINT("Moisty: ");
		PRINTLN(analogRead(_sensorPin));
	}

}
*/

