/**
 *  @file		TemperatureSensor.cpp
 *
 *  @author		maro
 *  @date 		04.07.2017
 *  @version	x.y
 *
 *  @brief 		"Driver" for DHT22 Sensor
 *
 *  @section DESCRIPTION
 *
 *  resistance changes depending on amount of moist
 *
 */

#include "TemperatureSensor.h"
#include "Debug.h"

//TemperatureSensor::TemperatureSensor() {
//	_sensorPin = 4;
//	_dht = new DHT(_sensorPin, DHTTYPE);
//	_temperature = 0;
//	_humidity = 0;
//}

TemperatureSensor::TemperatureSensor(String alias, int sensorPin, uint32_t interval) {
	name = alias;
	_sensorPin = sensorPin;
	_measurementInterval = interval;
	_dht = new DHT(_sensorPin, DHTTYPE);
	_temperature = 0;
	_humidity = 0;
	initializeSensorThresholds();
}


TemperatureSensor::~TemperatureSensor() {
	// TODO Auto-generated destructor stub
}

void TemperatureSensor::initializeSensorThresholds(){
	_defaultConditions.gradient = TEMP_DEFAULT_CONDITIONAL_GRAD;
	_defaultConditions.threshold_high = TEMP_DEFAULT_CONDITIONAL_THR_HIGH;
	_defaultConditions.threshold_low = TEMP_DEFAULT_CONDITIONAL_THR_LOW;
	_defaultConditions.requiredActivity = TEMP_DEFAULT_REQUIRED_ACTIVITY;

	_sensorDetectConditions.hysteresis.threshold_high = TEMP_SENSOR_ATTACHED_THRESHOLD_HIGH;
	_sensorDetectConditions.hysteresis.threshold_low = TEMP_SENSOR_ATTACHED_THRESHOLD_LOW;
	_sensorDetectConditions.hysteresis.gradient = TEMP_SENSOR_ATTACHED_GRADIENT;
}


void TemperatureSensor::begin(){
	PRINTLN("TemperatureSensor::begin() called");
	_elapsedTime = 0;
	_objStatusTxt = "";
	_dht->begin();
	_sensorDetected = sensorDetected();
}

void TemperatureSensor::handle() {
	if (_elapsedTime >= _measurementInterval) {
		_elapsedTime = _elapsedTime - _measurementInterval;
		_temperature = _dht->readTemperature();
		//_humidity = _dht->readHumidity();
		//if (String(_temperature) == "") --- unknown
		PRINT("Temperature: ");PRINTLN(_temperature);
		//PRINT("Humidity: ");PRINTLN(_humidity);
		_measuredValNow = _temperature;

	}

}

bool TemperatureSensor::sensorDetected(){
	PRINTLN("sensor dtect functzion temperature");
	bool retVal = false;
	String s = "";
	s = String(_dht->readTemperature());
	if (s != "")retVal = true;
	retVal = true;//TODO: when in above solution sensor is not detected at start up, it wont measure at all, thats not good!
	return retVal;
}

float TemperatureSensor::getHumidity(){
	return _dht->readHumidity();
}
