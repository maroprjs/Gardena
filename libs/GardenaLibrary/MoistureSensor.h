/**
 *  @file		MoistureSensor.h
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

#ifndef MOISTURESENSOR_H_
#define MOISTURESENSOR_H_
//#include <Arduino.h>
#include "Command.h"
#include "CommandList.h"
#include "Measurement.h"

	//#define MEASUREMENT_INTERVAL 1000
#define MOIST_SENSOR_ATTACHED_THRESHOLD_HIGH 600 //when above that value, sensor most likely not attached
#define MOIST_SENSOR_ATTACHED_THRESHOLD_LOW 600 //hence sensor be reported as detected at falling threshold
#define MOIST_SENSOR_ATTACHED_GRADIENT gradient_t::FALLING_EDGE //sensorDetectConditions

//default conditions to turn valves off or report as e.g. SMS
#define MOIST_DEFAULT_CONDITIONAL_GRAD gradient_t::RISING_EDGE
#define MOIST_DETECTED_THRESHOLD 300//when moist detected, measured threshold usually above 300
#define MOIST_DEFAULT_CONDITIONAL_THR_HIGH MOIST_DETECTED_THRESHOLD
#define MOIST_DEFAULT_CONDITIONAL_THR_LOW MOIST_DEFAULT_CONDITIONAL_THR_HIGH
#define MOIST_DEFAULT_REQUIRED_ACTIVITY hysteresis_t::AT_THRESHOLD_OFF


class MoistureSensor: public Measurement {
public:

	MoistureSensor(String alias, int sensorPin, CommandList * cmdList, uint32_t interval = DEFAULT_MEASUREMENT_INTERVAL);
	MoistureSensor(String alias, int sensorPin, uint32_t interval = DEFAULT_MEASUREMENT_INTERVAL);
	virtual ~MoistureSensor();
	//void begin();
	//void handle();
private:
	void initializeSensorThresholds();

};

#endif /* MOISTURESENSOR_H_ */
