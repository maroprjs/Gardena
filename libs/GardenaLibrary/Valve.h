/*
 * Valve.h
 *
 *  Created on: 20.10.2016
 *      Author: maro
 */

#ifndef VALVE_H_
#define VALVE_H_

//#include <Arduino.h>
//#include <functional>
//#include <vector>
#include "State.h"
#include "HBridge.h"
#include "Command.h"
#include "CommandList.h"
#include "Measurement.h"
#include <ArduinoJson.h>

//namespace Gardena {
	#define DEFAULT_CHECK_FOR_EVENT_INTERVAL 10000 //10 s



class Valve: public Command {
	const String DEFAULT_AUTO_DEPENDENCY = "rain";

public://types
	/*
	typedef Measurement::conditional thresholdReachedConditions_t;
	struct measurementDependency
	{
		Measurement* measurement;
		thresholdReachedConditions_t condition;
		enum valveState {VALVE_ON = ON, VALVE_OFF = OFF} requiredValveState;


	} ;
	typedef measurementDependency measurementDependency_t;
	typedef std::vector<measurementDependency_t> measurementDependencyVector_t;
*/
public:
	//Valve();
	Valve(HBridge* h_bridge,bool state,String alias,CommandList * cmdList);
	Valve(HBridge* h_bridge,bool state,String alias);
	virtual ~Valve();
	void switchValve(bool required_state);
	static void toggle(Command * objPtr);
	//static void turnOff(Command * objPtr);
	static String statusTxt(SMS* reply,Command * objPtr);
	String getName();
	void subscribe(Measurement * measurement);
	void begin();
	void handle();
	void setAutoDependencyName(String alias);

private://functions
	void initializeDefaultValveCmds();
    void walkTheCommandLine(Command * objPtr,paramTree_t parTree);
    void walkTheCommandLine2(Command * objPtr,paramTree_t parTree);
	bool cmdOnBranch(Command * objPtr,paramTree_t parTree);
	bool cmdOffBranch(Command * objPtr,paramTree_t parTree);
	bool cmdAutoBranch(Command * objPtr,paramTree_t parTree);
	bool cmdManBranch(Command * objPtr,paramTree_t parTree);
	bool cmdConditionalBranch(Command * objPtr,paramTree_t parTree);
	bool finalBranch(Command * objPtr,paramTree_t parTree);

private://variables
	//valve protocol (TODO: these are used by delay() function but don't give a warning!..Check this out!)
	const uint32_t _FORWARD_PULSE_LENGTH = 250; //[ms] pulse to turn valve ON, respectivly "forward contact2"
	const uint32_t _REVERSE_PULSE_LENGTH = 62.5; //[ms] pulse to turn valve OFF, respectivly "reverse contact2"
	elapsedMillis _elapsedTime;
	uint32_t _checkForEventInterval;
	bool _valve_state;
	String _valve_name;
	HBridge* _hbridge;
	Command * _cmd;
	CommandList * _cmdList;
	Measurement::measurementDependencyVector_t _measurementDependencyVector;
	String _autoDependency;
    DynamicJsonBuffer *_jsonBuffer;

};

//} /* namespace Gardena */

#endif /* VALVE_H_ */
