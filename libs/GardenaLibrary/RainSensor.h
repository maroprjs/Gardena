/**
 *  @file		RainSensor.h
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
#ifndef RAINSENSOR_H_
#define RAINSENSOR_H_
//#include <Arduino.h>
#include "Command.h"
#include "CommandList.h"
#include "Measurement.h"

//default condition to recognize if sensor is detected:
#define RAIN_SENSOR_ATTACHED_THRESHOLD_HIGH 300 // (when sensor attached, measured value around 512, otherwise far below (normally 0))
#define RAIN_SENSOR_ATTACHED_THRESHOLD_LOW 300 //when below that value, sensor most likely not attached
												//hence sensor be reported as detected at rising threshold
												//=above threshold
												//there is no hysteresis
#define RAIN_SENSOR_ATTACHED_GRADIENT gradient_t::RISING_EDGE //sensorDetectConditions

//default conditions to turn valves off(= rain on) or report as e.g. SMS
#define RAIN_DEFAULT_CONDITIONAL_GRAD gradient_t::RISING_EDGE
#define RAIN_DETECTED_THRESHOLD 900//when rain detected, measured threshold usually 1024
#define RAIN_DEFAULT_CONDITIONAL_THR_HIGH RAIN_DETECTED_THRESHOLD
#define RAIN_DEFAULT_CONDITIONAL_THR_LOW RAIN_DEFAULT_CONDITIONAL_THR_HIGH
#define RAIN_DEFAULT_REQUIRED_ACTIVITY hysteresis_t::AT_THRESHOLD_OFF





class RainSensor: public Measurement {
public:

	RainSensor(String alias, int sensorPin, CommandList * cmdList, uint32_t interval = DEFAULT_MEASUREMENT_INTERVAL);
	RainSensor(String alias, int sensorPin, uint32_t interval = DEFAULT_MEASUREMENT_INTERVAL);
	virtual ~RainSensor();
	//void begin();<-implemented in common measurement class
	//void handle();

private:
	void initializeSensorThresholds();

};

#endif /* RAINSENSOR_H_ */
