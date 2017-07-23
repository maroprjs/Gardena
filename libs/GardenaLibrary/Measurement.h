/*
 * Measurement.h
 *
 *  Created on: 04.07.2017
 *      Author: maro
 *
 *      Class for Analog (!!!) Measurements
 */

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_
//#include <Arduino.h>
#include "Command.h"
#include "CommandList.h"
#include "State.h"
#include <ArduinoJson.h>





	#define DEFAULT_MEASUREMENT_INTERVAL 5000


class Measurement:public Command {
public://types
	typedef unsigned long measVal_t;
	typedef unsigned long threshold_t;
	typedef enum grad{NOT_MEASURED, RISING_EDGE, FALLING_EDGE, RISING_OR_FALLING_EDGE,EQUAL,UNDEFINED} gradient_t;

	struct grad_map{
		enum grad strgToGradient(String s){
			   if ( s == "notmeasured" ) return NOT_MEASURED;
			    else if ( s == "rise" )  return RISING_EDGE;
			    else if ( s == "fall" )  return FALLING_EDGE;
			    else if ( s == "risefall" ) return RISING_OR_FALLING_EDGE;
			    else if ( s == "equal" ) return EQUAL;
			    else return UNDEFINED;
		}
	};
	typedef grad_map grad_map_t;

	struct hysteresis
	{
		threshold_t threshold_high;
		threshold_t threshold_low;
		gradient_t gradient;
		enum activity  {AT_THRESHOLD_ON = ON, AT_THRESHOLD_OFF = OFF} requiredActivity;
	};
	typedef hysteresis hysteresis_t;
//	struct conditional
//	{
//		hysteresis_t hysteresis;
//		enum grad{NOT_MEASURED, RISING_EDGE, FALLING_EDGE, RISING_OR_FALLING_EDGE} gradient;
//		enum activity  {AT_THRESHOLD_ON = ON, AT_THRESHOLD_OFF = OFF} requiredActivity;
		//int theshold;
		//enum grad{NOT_MEASURED, BLW_THR, ABV_THR, EQU_THR,RISING_EDGE, FALLING_EDGE} gradient;
//	} ;
//	typedef conditional conditional_t;

	struct sensorDetectConditions
	{
		hysteresis_t hysteresis;
		enum detected {NO, YES} sensorDetected;


	};
	typedef sensorDetectConditions sensorDetectConditions_t;
//	typedef conditional defaultConditions_t;

	/*TODO: this could be used to implement automated reporting (similiar as in valve):*/
	//typedef Measurement::conditional thresholdReachedConditions_t;
	struct measurementDependency
	{
		Measurement* measurement;
		//conditional_t condition;
		hysteresis_t hysteresis;
		//thresholdReachedConditions_t condition;
		//enum measState {REPORT_CONT_ON, REPORT_ONCE, REPORT_OFF} requiredMeasState;
		//enum valveState {VALVE_ON = ON, VALVE_OFF = OFF} requiredValveState;

	} ;
	typedef measurementDependency measurementDependency_t;
	typedef std::vector<measurementDependency_t> measurementDependencyVector_t;

	//*/



public://vars
	String name;
	sensorDetectConditions_t _sensorDetectConditions;



public://functions
	Measurement();
	Measurement(String alias, int sensorPin, uint32_t interval = DEFAULT_MEASUREMENT_INTERVAL);
	virtual ~Measurement();
	void begin();
	void handle();
	void setInterval(uint32_t interval);
	void setCommandList(CommandList * cmdList);
	//void addDependency(Measurement * measurement);moved to gsm
	static void cmdExecute(Command * objPtr);
	static String statusTxt(SMS* reply,Command * objPtr);
	bool sensorDetected();
	int getCurrentMeasVal();
	//defaultConditions_t getDefaultConditions();
	//conditional_t getDefaultConditions();
	hysteresis_t getDefaultConditions();

protected:
	int _sensorPin;
	measVal_t _measuredValNow;
	//conditional_t _defaultConditions;
	hysteresis_t _defaultConditions;
	elapsedMillis _elapsedTime;
	uint32_t _measurementInterval;
	Command * _cmd;
	CommandList * _cmdList;
	String _objStatusTxt;
	measurementDependencyVector_t _measurementDependencyVector;
	grad_map_t _grad_map;
	bool _sensorDetected;


protected:
	void initializeCmds();
	virtual void initializeSensorThresholds() = 0;
	void walkTheCommandLine(Command * objPtr,paramTree_t parTree);
	//bool cmdConditionalBranch(Command * objPtr,paramTree_t parTree);move to gsm
	bool cmdResetBranch(Command * objPtr,paramTree_t parTree);
	bool cmdThrHBranch(Command * objPtr,paramTree_t parTree);
	bool cmdThrLBranch(Command * objPtr,paramTree_t parTree);
	bool cmdGradientBranch(Command * objPtr,paramTree_t parTree);
	bool finalBranch(Command * objPtr,paramTree_t parTree);
};

#endif /* MEASUREMENT_H_ */
