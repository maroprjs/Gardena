/*
 * HumiditySensor.cpp
 *
 *  Created on: 22.07.2017
 *      Author: pionet
 */

#include "HumiditySensor.h"
#include "Debug.h"



HumiditySensor::HumiditySensor(String alias,  TemperatureSensor* sensor, uint32_t interval) {
	name = alias;
	_sensor = sensor;
	_measurementInterval = interval;
	_humidity = 0;
	initializeSensorThresholds();
}

HumiditySensor::~HumiditySensor() {
	// TODO Auto-generated destructor stub
}

void HumiditySensor::initializeSensorThresholds(){
	_defaultConditions.gradient = HUMIDITY_DEFAULT_CONDITIONAL_GRAD;
	_defaultConditions.threshold_high = HUMIDITY_DEFAULT_CONDITIONAL_THR_HIGH;
	_defaultConditions.threshold_low = HUMIDITY_DEFAULT_CONDITIONAL_THR_LOW;
	_defaultConditions.requiredActivity = HUMIDITY_DEFAULT_REQUIRED_ACTIVITY;

	_sensorDetectConditions.hysteresis.threshold_high = HUMIDITY_SENSOR_ATTACHED_THRESHOLD_HIGH;
	_sensorDetectConditions.hysteresis.threshold_low = HUMIDITY_SENSOR_ATTACHED_THRESHOLD_LOW;
	_sensorDetectConditions.hysteresis.gradient = HUMIDITY_SENSOR_ATTACHED_GRADIENT;
}

void HumiditySensor::begin(){
	PRINTLN("HumiditySensorSensor::begin() called");
	_elapsedTime = 0;
	_objStatusTxt = "";
	_sensorDetected = sensorDetected();
}

void HumiditySensor::handle() {
	if (_elapsedTime >= _measurementInterval) {
		_elapsedTime = _elapsedTime - _measurementInterval;
		_humidity = _sensor->getHumidity();
		PRINT("Humidity: ");PRINTLN(_humidity);
		_measuredValNow = _humidity;
	}
}

bool HumiditySensor::sensorDetected(){
	PRINTLN("sensor dtect functzion humidity");
	bool retVal = false;
	String s = "";
	s = String(_sensor->getHumidity());
	if (s != "")retVal = true;
	retVal = true;//TODO: when in above solution sensor is not detected at start up, it wont measure at all, thats not good!
	return retVal;
}

