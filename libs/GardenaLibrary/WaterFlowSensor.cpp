/**
 *  @file		WaterFlowSensor.cpp
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

#include "WaterFlowSensor.h"
#include "Debug.h"

volatile unsigned long total_water_sample_count = 0; //measuring the rising edges of the water flow signal
//unsigned long current_water_sample_count = 0;
void waterFlowPulse ()     //This is the function that the interupt calls
{
	total_water_sample_count++;  //This function measures the rising and falling edge of the hall effect sensors signal
}

//WaterFlowSensor::WaterFlowSensor() {
//	_sensorPin = 3;
//	initializeCounters();
//}

WaterFlowSensor::WaterFlowSensor(String alias, int sensorPin, uint32_t interval) {
	name = alias;
	_sensorPin = sensorPin;
	_measurementInterval = interval;
	initializeCounters();
	initializeSensorThresholds();
	//_cmd = new Command();done in genereal measurement class
	//_cmdList = cmdList;
	//_cmdList->registerCmd(_cmd);

}

WaterFlowSensor::~WaterFlowSensor() {
	// TODO Auto-generated destructor stub
}

void WaterFlowSensor::initializeSensorThresholds(){
	_defaultConditions.gradient = FLOW_DEFAULT_CONDITIONAL_GRAD;
	_defaultConditions.threshold_high = FLOW_DEFAULT_CONDITIONAL_THR_HIGH;
	_defaultConditions.threshold_low = FLOW_DEFAULT_CONDITIONAL_THR_LOW;
	_defaultConditions.requiredActivity = FLOW_DEFAULT_REQUIRED_ACTIVITY;

	_sensorDetectConditions.hysteresis.threshold_high = FLOW_SENSOR_ATTACHED_THRESHOLD_HIGH;
	_sensorDetectConditions.hysteresis.threshold_low = FLOW_SENSOR_ATTACHED_THRESHOLD_LOW;
	_sensorDetectConditions.hysteresis.gradient = FLOW_SENSOR_ATTACHED_GRADIENT;
}


void WaterFlowSensor::initializeCounters(){
	_waterPulsesPerMeasurementPeriode = 0;
	_lastTotalNumberOfPulses = 0;
	total_water_sample_count = 0;
}
void WaterFlowSensor::begin(){
	PRINTLN("WaterFlowSensor::begin() called");
	_elapsedTime = 0;
	_objStatusTxt = "";
	pinMode(_sensorPin, INPUT);
	pinMode(_sensorPin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(_sensorPin), waterFlowPulse, RISING);
}

void WaterFlowSensor::handle(){
   //int rpm;
   //unsigned int samples_per_given_time = 0;
   if (_elapsedTime >= _measurementInterval) {
	   _elapsedTime = _elapsedTime - _measurementInterval;
      //cli();      //Disable interrupts <-ToDo: check if enabling/disabling interrupt changes accuracy!!!
      _waterPulsesPerMeasurementPeriode = total_water_sample_count - _lastTotalNumberOfPulses;
      _lastTotalNumberOfPulses = total_water_sample_count;
      //rpm = (samples_per_given_time * 60)/FLOW_RATE; //(Pulse frequency x 60) / Q, = flow rate in L/min
      //sei(); //enable interrupt  <-ToDo: check if enabling/disabling interrupt changes accuracy!!!
      //Serial.print (rpm, DEC); //Prints the number calculated above
      //Serial.println (" L/min");
	   PRINT("water pulses measure: ");
	   PRINTLN(_waterPulsesPerMeasurementPeriode);
	   _measuredValNow = _waterPulsesPerMeasurementPeriode;
   }
}





