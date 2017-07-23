/*
 * Timer.h
 *
 *  Created on: 16.07.2017
 *      Author: pionet
 */

#ifndef TIMER_H_
#define TIMER_H_
#include "Command.h"
#include "CommandList.h"
#include "Measurement.h"
#include "Clock.h"

//default condition to recognize if sensor is detected:
#define TIMER_SENSOR_ATTACHED_THRESHOLD_HIGH timeSet  // (when sensor attached, measured value around 512, otherwise far below (normally 0))
#define TIMER_SENSOR_ATTACHED_THRESHOLD_LOW timeSet //when below that value, sensor most likely not attached
												//hence sensor be reported as detected at rising threshold
												//=above threshold
												//there is no hysteresis
#define TIMER_SENSOR_ATTACHED_GRADIENT gradient_t::UNDEFINED //sensorDetectConditions

//default conditions to turn valves off(= rain on) or report as e.g. SMS
#define TIMER_DEFAULT_CONDITIONAL_GRAD gradient_t::RISING_OR_FALLING_EDGE
#define TIMER_DETECTED_THRESHOLD 1//not used
#define TIMER_DEFAULT_CONDITIONAL_THR_HIGH TIMER_DETECTED_THRESHOLD//TODO
#define TIMER_DEFAULT_CONDITIONAL_THR_LOW TIMER_DEFAULT_CONDITIONAL_THR_HIGH//TODO
#define TIMER_DEFAULT_REQUIRED_ACTIVITY hysteresis_t::AT_THRESHOLD_OFF

class Timer:public Measurement {
public:
	Timer(String alias,Clock* clock, CommandList * cmdList, uint32_t interval = DEFAULT_MEASUREMENT_INTERVAL);
	virtual ~Timer();
	void handle();
	void begin();
private:
	void initializeSensorThresholds();
	bool sensorDetected();
	Clock* _clock;

};

#endif /* TIMER_H_ */
