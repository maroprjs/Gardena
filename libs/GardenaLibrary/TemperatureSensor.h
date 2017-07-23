/**
 *  @file		TemperatureSensor.h
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
#ifndef TEMPERATURESENSOR_H_
#define TEMPERATURESENSOR_H_
#include <Arduino.h>
#include "Measurement.h"
#include "Command.h"
#include "CommandList.h"
#include <DHT.h>

	#define DHTTYPE DHT22


//default condition to recognize if sensor is detected:
#define TEMP_SENSOR_ATTACHED_THRESHOLD_HIGH 0 // dummy
#define TEMP_SENSOR_ATTACHED_THRESHOLD_LOW 0 //dummx
#define TEMP_SENSOR_ATTACHED_GRADIENT gradient_t::UNDEFINED //sensorDetectConditions

//default conditions to turn valves off(= rain on) or report as e.g. SMS
#define TEMP_DEFAULT_CONDITIONAL_GRAD gradient_t::FALLING_EDGE
#define TEMP_DETECTED_THRESHOLD 23//when 23 degrees?
#define TEMP_DEFAULT_CONDITIONAL_THR_HIGH TEMP_DETECTED_THRESHOLD
#define TEMP_DEFAULT_CONDITIONAL_THR_LOW TEMP_DEFAULT_CONDITIONAL_THR_HIGH
#define TEMP_DEFAULT_REQUIRED_ACTIVITY hysteresis_t::AT_THRESHOLD_OFF

class TemperatureSensor: public Measurement {
public:
	//TemperatureSensor();
	TemperatureSensor(String alias, int sensorPin, uint32_t interval = DEFAULT_MEASUREMENT_INTERVAL);
	virtual ~TemperatureSensor();
	void begin();
	void handle();
	bool sensorDetected();
	float getHumidity();
private:
	//int _sensorPin;
	//elapsedMillis _measurementPeriode;
	void initializeSensorThresholds();
	DHT * _dht;
	float _temperature;
	float _humidity;
};

#endif /* TEMPERATURESENSOR_H_ */
