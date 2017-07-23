/*
 * Clock.cpp
 *
 *  Created on: 15.07.2017
 *      Author: pionet
 */

#include "Clock.h"
#include "Arduino.h"
#include "Debug.h"

//namespace Gardena {

Clock::Clock(String alias, uint32_t interval) {
	name = alias;
	_measurementInterval = interval;
	//_cmd = new Command();
	//_cmdList = cmdList;
	//_cmdList->registerCmd(_cmd);
}

Clock::~Clock() {
	// TODO Auto-generated destructor stub
}

void Clock::begin(){
	setSyncProvider(getTeensy3Time);
	delay(100);
	if (timeStatus() == timeSet){ _clockInSync = true;PRINTLN("clock sync");}
	else{ _clockInSync = false;PRINTLN("clock notsync");}
	//setTeensy3Time(1499279807);
}

time_t Clock::getTeensy3Time()
{

  return Teensy3Clock.get();
}

void Clock::setTeensy3Time(time_t t)
{
  Teensy3Clock.set(t);
  setTime(t);

}

bool Clock::clockInSync(){

	return (_clockInSync);
}

void Clock::handle(){
	if (_elapsedTime >= _measurementInterval) {
		_elapsedTime = _elapsedTime - _measurementInterval;
		//_measuredValNow = analogRead(_sensorPin);
		PRINT(name + ": ");//PRINTLN(_measuredValNow);
		digitalClockDisplay();
	}

}

void Clock::digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
  Serial.println(now());
}

void Clock::printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void Clock::initializeSensorThresholds(){//used by common measurement class
/*TODO:
	_defaultConditions.gradient = TIME_DEFAULT_CONDITIONAL_GRAD;
	_defaultConditions.threshold_rising_edge = TIME_DEFAULT_CONDITIONAL_THR_RISE;
	_defaultConditions.threshold_falling_edge = TIME_DEFAULT_CONDITIONAL_THR_FALL;
	_defaultConditions.requiredActivity = TIME_DEFAULT_REQUIRED_ACTIVITY;

	_sensorDetectConditions.hysteresis.threshold_rising_edge = TIME_SENSOR_ATTACHED_THRESHOLD_RISE;
	_sensorDetectConditions.hysteresis.threshold_falling_edge = TIME_SENSOR_ATTACHED_THRESHOLD_FALL;
	_sensorDetectConditions.hysteresis.gradient = TIME_SENSOR_ATTACHED_GRADIENT;
*/
}

//} /* namespace Gardena */
