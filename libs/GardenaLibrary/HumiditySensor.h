/*
 * HumiditySensor.h
 *
 *  Created on: 22.07.2017
 *      Author: pionet
 */

#ifndef HUMIDITYSENSOR_H_
#define HUMIDITYSENSOR_H_
#include "Measurement.h"
#include "Command.h"
#include "CommandList.h"
#include "TemperatureSensor.h"

//default condition to recognize if sensor is detected:
#define HUMIDITY_SENSOR_ATTACHED_THRESHOLD_HIGH 0 // dummy
#define HUMIDITY_SENSOR_ATTACHED_THRESHOLD_LOW 0 //dummx
#define HUMIDITY_SENSOR_ATTACHED_GRADIENT gradient_t::UNDEFINED //sensorDetectConditions

//default conditions to turn valves off(= rain on) or report as e.g. SMS
#define HUMIDITY_DEFAULT_CONDITIONAL_GRAD gradient_t::RISING_EDGE
#define HUMIDITY_DETECTED_THRESHOLD 80//?
#define HUMIDITY_DEFAULT_CONDITIONAL_THR_HIGH HUMIDITY_DETECTED_THRESHOLD
#define HUMIDITY_DEFAULT_CONDITIONAL_THR_LOW HUMIDITY_DEFAULT_CONDITIONAL_THR_HIGH
#define HUMIDITY_DEFAULT_REQUIRED_ACTIVITY hysteresis_t::AT_THRESHOLD_OFF

class HumiditySensor: public Measurement {
public:
	HumiditySensor(String alias,  TemperatureSensor* sensor, uint32_t interval = DEFAULT_MEASUREMENT_INTERVAL);
	virtual ~HumiditySensor();
	void begin();
	void handle();
	bool sensorDetected();
private:
	void initializeSensorThresholds();
	TemperatureSensor* _sensor;
	float _humidity;
};

#endif /* HUMIDITYSENSOR_H_ */
