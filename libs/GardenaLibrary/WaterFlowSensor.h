/**
 *  @file		WaterFlowSensor.h
 *
 *  @author		maro
 *  @date 		03.07.2017
 *  @version	x.y
 *
 *  @brief 		"Driver" for G1 Water Flow Sensor
 *
 *  @section DESCRIPTION
 *
 *  ---
 *
 */

#ifndef WATERFLOWSENSOR_H_
#define WATERFLOWSENSOR_H_
#include "Command.h"
#include "CommandList.h"
#include "Measurement.h"

	//#define MEASUREMENT_PERIODE 1000 // one second
	#define FLOW_RATE 1 //this is Q; the proper value needs to be adjusted ((Pulse frequency x 60) / Q, = flow rate in L/hour)

//default condition to recognize if sensor is detected:
#define FLOW_SENSOR_ATTACHED_THRESHOLD_HIGH 0 //this is dummy
#define FLOW_SENSOR_ATTACHED_THRESHOLD_LOW 0 //this is dummy
#define FLOW_SENSOR_ATTACHED_GRADIENT gradient_t::RISING_EDGE //sensorDetectConditions

//default conditions to report waterflow e.g. SMS
#define FLOW_DEFAULT_CONDITIONAL_GRAD gradient_t::FALLING_EDGE
#define FLOW_DETECTED_THRESHOLD 1//when waterflow below detected than report "all good" no wasting water
#define FLOW_DEFAULT_CONDITIONAL_THR_HIGH FLOW_DETECTED_THRESHOLD
#define FLOW_DEFAULT_CONDITIONAL_THR_LOW FLOW_DEFAULT_CONDITIONAL_THR_HIGH
#define FLOW_DEFAULT_REQUIRED_ACTIVITY hysteresis_t::AT_THRESHOLD_OFF

class WaterFlowSensor: public Measurement {
public:
	//WaterFlowSensor();
	WaterFlowSensor(String alias, int sensorPin, uint32_t interval = DEFAULT_MEASUREMENT_INTERVAL);
	virtual ~WaterFlowSensor();
	void begin();
	void handle();
private:
	void initializeSensorThresholds();
	void initializeCounters();
	unsigned long _waterPulsesPerMeasurementPeriode;
	unsigned long _lastTotalNumberOfPulses;
};

#endif /* WATERFLOWSENSOR_H_ */
