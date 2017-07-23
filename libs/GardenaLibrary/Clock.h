/*
 * Clock.h
 *
 *  Created on: 15.07.2017
 *      Author: pionet
 */

#ifndef CLOCK_H_
#define CLOCK_H_
#include <TimeLib.h>
#include "Command.h"
#include "CommandList.h"
#include "Measurement.h"

//namespace Gardena {

class Clock:public Measurement {
public:
	Clock(String alias, uint32_t interval = DEFAULT_MEASUREMENT_INTERVAL);
	virtual ~Clock();
	void begin();
	void handle();
	bool clockInSync();

private:
	void initializeSensorThresholds();
	static time_t getTeensy3Time();
	static void setTeensy3Time(time_t t);
	void printDigits(int digits);
	void digitalClockDisplay();
	bool _clockInSync = false;

};

//} /* namespace Gardena */

#endif /* CLOCK_H_ */
