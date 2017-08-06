/*
 * Valve.cpp
 *
 *  Created on: 20.10.2016
 *      Author: maro
 */


#include "Valve.h"
#include "Debug.h"
#include<algorithm> //needed for std find() in vector



//namespace Gardena {

Valve::Valve(HBridge* h_bridge,bool state,String alias, CommandList *cmdList){
	_hbridge = h_bridge;
	_valve_state = state;
	_valve_name = alias;
	_cmd = new Command();
	initializeDefaultValveCmds();
	_cmdList = cmdList;
	_cmdList->registerCmd(_cmd);
	_checkForEventInterval = DEFAULT_CHECK_FOR_EVENT_INTERVAL;



}

Valve::Valve(HBridge* h_bridge,bool state,String alias){
	_hbridge = h_bridge;
	_valve_state = state;
	switchValve(_valve_state);
	_valve_name = alias;
	_checkForEventInterval = DEFAULT_CHECK_FOR_EVENT_INTERVAL;

}

Valve::~Valve() {
	// TODO Auto-generated destructor stub
}


void Valve::initializeDefaultValveCmds(){
	_cmd->addCmd("valves", &toggle,  &statusTxt ,this); //all valves
	_cmd->addCmd("valve" + _valve_name, &toggle,  &statusTxt ,this);
	_cmd->addCmd("v" + _valve_name, &toggle,  &statusTxt ,this);
	_cmd->addCmd(_valve_name, &toggle,  &statusTxt ,this);
	_cmd->addCmd("ventil" + _valve_name , &toggle,  &statusTxt ,this);
	_cmd->addCmd("ventile", &toggle,  &statusTxt ,this);//alle ventile
	_cmd->addCmd("v" + _valve_name, &toggle,  &statusTxt ,this);

}


void Valve::switchValve(bool required_state){
	PRINT("switchValve(): ");PRINT(this->_valve_name);PRINT(" called, state: ");PRINTLN(required_state);
	delay(500);
  if (required_state == ON) {
	  cli();
     _hbridge->forwardOn();
     delay(_FORWARD_PULSE_LENGTH);////gardena protocol +9V for 250ms
     _hbridge->forwardOff();
     sei();
     _valve_state = ON;
  } else {//OFF
	  cli();
     _hbridge->reverseOn();
     delay(_REVERSE_PULSE_LENGTH);//gardena protocol -9V(over coil and 150Ohm resistor) for 62.5ms
     _hbridge->reverseOff();
     sei();
     _valve_state = OFF;
  }
 }

/**
 * Valve::toggle(): extract params from here (if needed, e.g if toggle at a specific time): v->_cmd->cmdStrgVector
//1.search in v->_cmd->cmdStrgVector for "on" or "an"
//2.increase index
//3.compare extracted string with iterated aliases in measurement dependency vector
//4.if found, extract threshold value and greater-than/smaller-than/off value
//5.in handle function  check continuelsy if those dependencies are "armed"
 */
void Valve::toggle(Command * objPtr){

	PRINTLN("Valve::toggle called");
	paramTree_t parTree;//TODO: add paramTree as class member
	parTree.previousBranch = "initial";
	parTree.currentBranch = "valve";
	parTree.nextBranch = "valve_on";
	Valve * vref = (Valve*)objPtr;
	//vref->walkTheCommandLine(objPtr, parTree);
	vref->walkTheCommandLine2(objPtr, parTree);
	//jsonReply["valve1"] = "on";
	//jsonReply["valve2"] = "on";
	//jsonReply["valve3"] = "off";

}

/**
 * for conditional switching the dafeault conditions are defined within each
 * measurement specific class (e.g. rainsensor or moisture sensor).
 * These conditions must describe the condition for which the valves will be turned OFF
 * not ON!!!(OFF is hardcoded below) -OBSOLETE!!!
 */
void Valve::walkTheCommandLine2(Command * objPtr,paramTree_t parTree){

	if (cmdOnBranch(objPtr,parTree)) return;
	else if (cmdOffBranch(objPtr,parTree)) return;
	else if (cmdAutoBranch(objPtr,parTree)) return;
	else if (cmdManBranch(objPtr,parTree)) return;
	else if (cmdConditionalBranch(objPtr,parTree)) return;
	else finalBranch(objPtr,parTree);

}

bool Valve::cmdOnBranch(Command * objPtr,paramTree_t parTree){
	bool retVal = false;
	Valve * vref = (Valve*)objPtr;
	std::vector<String>::const_iterator paramIterator;

	////////////////////////////////////////////////////////////////////////////////////
	if ((parTree.currentBranch == "valve") && (parTree.nextBranch == "valve_on")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		paramIterator=find(vref->_cmd->cmdStrgVector.begin(),vref->_cmd->cmdStrgVector.end(),"on");
		if (paramIterator != vref->_cmd->cmdStrgVector.end()){
			vref->switchValve(ON);
			parTree.previousBranch = parTree.currentBranch;
			parTree.currentBranch = parTree.nextBranch;
			parTree.nextBranch = "final";
		}else{
			parTree.nextBranch = "valve_off";
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine2(objPtr, parTree);
		retVal =  true;
	};
	////////////////////////////////////////////////////////////////////////////////////////////
	return retVal;
};

bool Valve::cmdOffBranch(Command * objPtr,paramTree_t parTree){
	bool retVal = false;
	Valve * vref = (Valve*)objPtr;
	std::vector<String>::const_iterator paramIterator;

	////////////////////////////////////////////////////////////////////////////////////////////
	if ((parTree.currentBranch == "valve") && (parTree.nextBranch == "valve_off")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		paramIterator=find(vref->_cmd->cmdStrgVector.begin(),vref->_cmd->cmdStrgVector.end(),"off");
		if (paramIterator != vref->_cmd->cmdStrgVector.end()){
			vref->switchValve(OFF);
			parTree.previousBranch = parTree.currentBranch;
			parTree.currentBranch = parTree.nextBranch;
			parTree.nextBranch = "final";
		}else{
			parTree.nextBranch = "valve_auto";
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine2(objPtr, parTree);
		retVal = true;
	}
	return retVal;

};
bool Valve::cmdAutoBranch(Command * objPtr,paramTree_t parTree){
	bool retVal = false;
	Valve * vref = (Valve*)objPtr;
	std::vector<String>::const_iterator paramIterator;
	Measurement::measurementDependencyVector_t::iterator measDepsIterator;
	Measurement::measurementDependency_t md;
	///////////////////////////////////////////////////////////////////////////////////////////////
	if ((parTree.currentBranch == "valve") && (parTree.nextBranch == "valve_auto")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		paramIterator=find(vref->_cmd->cmdStrgVector.begin(),vref->_cmd->cmdStrgVector.end(),"auto");
		if (paramIterator != vref->_cmd->cmdStrgVector.end()){
			measDepsIterator = vref->_measurementDependencyVector.begin();
			while (measDepsIterator != vref->_measurementDependencyVector.end()){
				md = *measDepsIterator;
				if (md.measurement->name == _autoDependency/*default defined in header*/) PRINT("valve_auto ");PRINT(_autoDependency);PRINTLN(" iterator found");break;
				measDepsIterator++;
			}
			if (measDepsIterator != vref->_measurementDependencyVector.end()){
				md.hysteresis.gradient = md.measurement->getDefaultConditions().gradient;
				md.hysteresis.threshold_high = md.measurement->getDefaultConditions().threshold_high;
				md.hysteresis.threshold_low = md.measurement->getDefaultConditions().threshold_low;
				md.hysteresis.requiredActivity = md.measurement->getDefaultConditions().requiredActivity;
				*measDepsIterator = md;
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "final";
			}else
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "final";
		}else{
			parTree.nextBranch = "valve_man";
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine2(objPtr, parTree);
		retVal = true;
	}
	return retVal;
};

bool Valve::cmdManBranch(Command * objPtr,paramTree_t parTree){
	bool retVal = false;
	Valve * vref = (Valve*)objPtr;
	std::vector<String>::const_iterator paramIterator;
	Measurement::measurementDependencyVector_t::iterator measDepsIterator;
	Measurement::measurementDependency_t md;
	///////////////////////////////////////////////////////////////////////////////////////////////
	//TODO: currentBranch should be "valve_man", but can only work if parTree is class member
	if ((parTree.currentBranch == "valve") && (parTree.nextBranch == "valve_man")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		paramIterator=find(vref->_cmd->cmdStrgVector.begin(),vref->_cmd->cmdStrgVector.end(),"man");
		if (paramIterator != vref->_cmd->cmdStrgVector.end()){//"man" recognized
			measDepsIterator = vref->_measurementDependencyVector.begin();
			/*when only rain should be stopped:
			while (measDepsIterator != vref->_measurementDependencyVector.end()){
				md = *measDepsIterator;
				if (md.measurement->name == "rain")break;
				measDepsIterator++;
			}
			if (measDepsIterator != vref->_measurementDependencyVector.end()){
				md.condition.gradient = Measurement::conditional::NOT_MEASURED;
				*measDepsIterator = md;
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "final";
			}else
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "final";
				*/
			//better to clear all automatic switching conditions:
			while (measDepsIterator != vref->_measurementDependencyVector.end()){
				md = *measDepsIterator;
				md.hysteresis.gradient = Measurement::gradient_t::NOT_MEASURED;
				*measDepsIterator = md;
				measDepsIterator++;
			}
			parTree.previousBranch = parTree.currentBranch;
			parTree.currentBranch = parTree.nextBranch;
			parTree.nextBranch = "final";
		}else{
			parTree.nextBranch = "valve_cond";
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine2(objPtr, parTree);
		retVal = true;
	}
	return retVal;
};


bool Valve::cmdConditionalBranch(Command * objPtr,paramTree_t parTree){
	bool retVal = false;
	Valve * vref = (Valve*)objPtr;
	std::vector<String>::const_iterator paramIterator;
	Measurement::measurementDependencyVector_t::iterator measDepsIterator;
	Measurement::measurementDependency_t md;
	///////////////////////////////////////////////////////////////////////////////////////////////
	if ((parTree.currentBranch == "valve") && (parTree.nextBranch == "valve_cond")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		String measObjAlias = "";
		//TODO: this iterates also through "valve->X" or "valves"<-make offset or store iterator in another variable if conflicts occure
		for (paramIterator = vref->_cmd->cmdStrgVector.begin();paramIterator != vref->_cmd->cmdStrgVector.end();++paramIterator){
			measObjAlias = *paramIterator;
			measDepsIterator = vref->_measurementDependencyVector.begin();
			while (measDepsIterator != vref->_measurementDependencyVector.end()){
				md = *measDepsIterator;
				if (md.measurement->name == measObjAlias){
					md.hysteresis.gradient = md.measurement->getDefaultConditions().gradient;
					md.hysteresis.threshold_high = md.measurement->getDefaultConditions().threshold_high;
					md.hysteresis.threshold_low = md.measurement->getDefaultConditions().threshold_low;
					md.hysteresis.requiredActivity = md.measurement->getDefaultConditions().requiredActivity;
					*measDepsIterator = md;
					parTree.previousBranch = parTree.currentBranch;
					parTree.currentBranch = parTree.nextBranch;
					parTree.nextBranch = "final";
					break;
				}
				else {
					parTree.nextBranch = "final";
				}
				measDepsIterator++;
			}
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine2(objPtr, parTree);
		retVal = true;;
	}
	return retVal;
};

bool Valve::finalBranch(Command * objPtr,paramTree_t parTree){
	bool retVal = false;
	if (/*(parTree.currentBranch == "valve") &&*/(parTree.nextBranch == "final")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		parTree.previousBranch = parTree.currentBranch;
		parTree.currentBranch = "final";
		parTree.nextBranch = "initial";
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		retVal = true;
	}
	return retVal;
};

void Valve::walkTheCommandLine(Command * objPtr,paramTree_t parTree){

	Valve * vref = (Valve*)objPtr;
	std::vector<String>::const_iterator paramIterator;
	Measurement::measurementDependencyVector_t::iterator measDepsIterator;
	Measurement::measurementDependency_t md;
	////////////////////////////////////////////////////////////////////////////////////
	if ((parTree.currentBranch == "valve") && (parTree.nextBranch == "valve_on")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		paramIterator=find(vref->_cmd->cmdStrgVector.begin(),vref->_cmd->cmdStrgVector.end(),"on");
		if (paramIterator != vref->_cmd->cmdStrgVector.end()){
			vref->switchValve(ON);
			parTree.previousBranch = parTree.currentBranch;
			parTree.currentBranch = parTree.nextBranch;
			parTree.nextBranch = "final";
		}else{
			parTree.nextBranch = "valve_off";
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine(objPtr, parTree);
		return;
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	if ((parTree.currentBranch == "valve") && (parTree.nextBranch == "valve_off")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		paramIterator=find(vref->_cmd->cmdStrgVector.begin(),vref->_cmd->cmdStrgVector.end(),"off");
		if (paramIterator != vref->_cmd->cmdStrgVector.end()){
			vref->switchValve(OFF);
			parTree.previousBranch = parTree.currentBranch;
			parTree.currentBranch = parTree.nextBranch;
			parTree.nextBranch = "final";
		}else{
			parTree.nextBranch = "valve_auto";
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine(objPtr, parTree);
		return;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	if ((parTree.currentBranch == "valve") && (parTree.nextBranch == "valve_auto")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		paramIterator=find(vref->_cmd->cmdStrgVector.begin(),vref->_cmd->cmdStrgVector.end(),"auto");
		if (paramIterator != vref->_cmd->cmdStrgVector.end()){
			measDepsIterator = vref->_measurementDependencyVector.begin();
			while (measDepsIterator != vref->_measurementDependencyVector.end()){
				md = *measDepsIterator;
				if (md.measurement->name == _autoDependency) PRINT("valve_auto ");PRINT(_autoDependency);PRINTLN(" iterator found");break;
				measDepsIterator++;
			}
			if (measDepsIterator != vref->_measurementDependencyVector.end()){
				md.hysteresis.gradient = md.measurement->getDefaultConditions().gradient;
				md.hysteresis.threshold_high = md.measurement->getDefaultConditions().threshold_high;
				md.hysteresis.threshold_low = md.measurement->getDefaultConditions().threshold_low;
				md.hysteresis.requiredActivity = md.measurement->getDefaultConditions().requiredActivity;
				*measDepsIterator = md;
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "final";
			}else
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "final";
		}else{
			parTree.nextBranch = "valve_man";
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine(objPtr, parTree);
		return;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	//TODO: currentBranch should be "valve_man", but can only work if parTree is class member
	if ((parTree.currentBranch == "valve") && (parTree.nextBranch == "valve_man")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		paramIterator=find(vref->_cmd->cmdStrgVector.begin(),vref->_cmd->cmdStrgVector.end(),"man");
		if (paramIterator != vref->_cmd->cmdStrgVector.end()){//"man" recognized
			measDepsIterator = vref->_measurementDependencyVector.begin();
			/*when only rain should be stopped:
			while (measDepsIterator != vref->_measurementDependencyVector.end()){
				md = *measDepsIterator;
				if (md.measurement->name == "rain")break;
				measDepsIterator++;
			}
			if (measDepsIterator != vref->_measurementDependencyVector.end()){
				md.condition.gradient = Measurement::conditional::NOT_MEASURED;
				*measDepsIterator = md;
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "final";
			}else
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "final";
				*/
			//better to clear all automatic switching conditions:
			while (measDepsIterator != vref->_measurementDependencyVector.end()){
				md = *measDepsIterator;
				md.hysteresis.gradient = Measurement::gradient_t::NOT_MEASURED;
				*measDepsIterator = md;
				measDepsIterator++;
			}
			parTree.previousBranch = parTree.currentBranch;
			parTree.currentBranch = parTree.nextBranch;
			parTree.nextBranch = "final";
		}else{
			parTree.nextBranch = "valve_cond";
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine(objPtr, parTree);
		return;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	if ((parTree.currentBranch == "valve") && (parTree.nextBranch == "valve_cond")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		String measObjAlias = "";
		//TODO: this iterates also through "valve->X" or "valves"<-make offset or store iterator in another variable if conflicts occure
		for (paramIterator = vref->_cmd->cmdStrgVector.begin();paramIterator != vref->_cmd->cmdStrgVector.end();++paramIterator){
			measObjAlias = *paramIterator;
			measDepsIterator = vref->_measurementDependencyVector.begin();
			while (measDepsIterator != vref->_measurementDependencyVector.end()){
				md = *measDepsIterator;
				if (md.measurement->name == measObjAlias){
					md.hysteresis.gradient = md.measurement->getDefaultConditions().gradient;
					md.hysteresis.threshold_high = md.measurement->getDefaultConditions().threshold_high;
					md.hysteresis.threshold_low = md.measurement->getDefaultConditions().threshold_low;
					md.hysteresis.requiredActivity = md.measurement->getDefaultConditions().requiredActivity;
					*measDepsIterator = md;
					parTree.previousBranch = parTree.currentBranch;
					parTree.currentBranch = parTree.nextBranch;
					parTree.nextBranch = "final";
					break;
				}
				else {
					parTree.nextBranch = "final";
				}
				measDepsIterator++;
			}
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine(objPtr, parTree);
		return;
	}


	if (/*(parTree.currentBranch == "valve") &&*/(parTree.nextBranch == "final")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		parTree.previousBranch = parTree.currentBranch;
		parTree.currentBranch = "final";
		parTree.nextBranch = "initial";
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);

		return;
	}

}


//function statusTxt(..) composes:
//-return message for real SMS towards GSM modem in smsReply
//-return message in "self-made" json format
//-json message using ArduinoJson library
//TODO: clean/unify this!!!
//this is what I want: "valve1":{"status":"on","info":"rain and moist"}
String Valve::statusTxt(SMS* smsReply,Command * objPtr){
	PRINT("Valve::statusTxt called");
	Valve *v = (Valve*)objPtr;
	JsonObject& jValve = smsReply->jsonMsg->createNestedObject(v->_valve_name);

	bool manual = true;
	String retVal = "\"" + v->_valve_name + "\"" + ":{" + "\"" + "status" + "\"" + ":" + "\"";
	smsReply->msg = smsReply->msg + " " +  v->_valve_name + ": ";
	if (v->_valve_state == ON){
		retVal = retVal + "on" ;
		smsReply->msg = smsReply->msg + "on ";
		jValve["status"] = "on";

	}
	else {
		retVal = retVal + "off" ;
		smsReply->msg = smsReply->msg + "off ";
		jValve["status"] = "off";

	}
	retVal = retVal + "\"" + "," + "\"" + "info" + "\"" + ":" + "\"";
	smsReply->msg = smsReply->msg + "info: ";
	//extract params from here (if needed, e.g status 'armed for specific times'): v->_cmd->cmdStrgVector
	//iterate through all dependencies and act accordingly
	Measurement::measurementDependencyVector_t::iterator measDepsIterator;
	Measurement::measurementDependency_t md;
	String jInfoStrg = "";
	for (measDepsIterator = v->_measurementDependencyVector.begin();
			measDepsIterator != v->_measurementDependencyVector.end(); ++measDepsIterator)
	{
		md = *measDepsIterator;
		if (md.hysteresis.gradient != Measurement::gradient_t::NOT_MEASURED){
			manual = false;
			retVal = retVal + md.measurement->name + " automatic ";
			smsReply->msg =  smsReply->msg + md.measurement->name + " automatic ";
			jInfoStrg = md.measurement->name + " automatic ";
			if (md.measurement->sensorDetected() == false) {
				retVal = retVal + " but no sensor!";
				smsReply->msg =  smsReply->msg + " but no sensor!";
				jInfoStrg = jInfoStrg + " but no sensor";
			}else{
				retVal = retVal + " val: " + md.measurement->getCurrentMeasVal();
				smsReply->msg =  smsReply->msg + " val: " + md.measurement->getCurrentMeasVal();
				jInfoStrg = jInfoStrg + " val: " + md.measurement->getCurrentMeasVal();
			}
		}

	}
	if (manual){
		retVal = retVal + "manual";
		smsReply->msg =  smsReply->msg + "man";
		jInfoStrg = "manual";
	}
	retVal = retVal + "\"" + "}";
	jValve["info"] = jInfoStrg;
	smsReply->jsonData->add(jValve);
	PRINTLN(" with json return:  " + retVal);
	PRINTLN(" with sms return:  " + smsReply->msg);
	return retVal;
}

String Valve::getName(){
	return _valve_name;
}

//subscribe for a certain measurement
void Valve::subscribe(Measurement * measurement){
	Measurement::measurementDependency_t md;
	md.measurement = measurement;
	md.hysteresis.gradient = Measurement::gradient_t::NOT_MEASURED;
	_measurementDependencyVector.push_back(md);
	// now these dependencies are added, so I can extend the command list,
	//switching valves based on measured thresholds

}

void Valve::begin(){
	PRINTLN("Valve::begin() called");
	_hbridge->begin();
	switchValve(_valve_state);
	_autoDependency = DEFAULT_AUTO_DEPENDENCY;
	_elapsedTime = 0;
}
void Valve::handle(){

	if (_elapsedTime >= _checkForEventInterval) {
		_elapsedTime = _elapsedTime - _checkForEventInterval;


		//iterate through all dependencies and act accordingly
		Measurement::measurementDependencyVector_t::iterator measDepsIterator;
		Measurement::measurementDependency_t md;
		uint8_t numberOfRequiredOFF = 0,numberOfRequiredON = 0;//OFF-request is dominant; used for OR-operation
		bool validThreshold = false;
		for (measDepsIterator = _measurementDependencyVector.begin();
				measDepsIterator != _measurementDependencyVector.end(); ++measDepsIterator)
		{

			md = *measDepsIterator;
//			PRINT("handle: ");PRINT(md.measurement->name);PRINT(" gradient: ");PRINT(md.hysteresis.gradient);
			if (md.measurement->sensorDetected() == true){
				Measurement::measVal_t measVal = md.measurement->getCurrentMeasVal();
//				PRINT("current measVal ");PRINT(measVal);PRINT(" theshold high: ");PRINT(md.hysteresis.threshold_high);
//				PRINT(" theshold low: ");PRINTLN(md.hysteresis.threshold_low);
//				PRINT("requiredValveState: ");PRINT(md.hysteresis.requiredActivity);PRINT(" current valveState: ");PRINTLN(_valve_state);
				switch  (md.hysteresis.gradient){
				case Measurement::gradient_t::EQUAL:PRINT(this->_valve_name);PRINT(" ");PRINT(md.measurement->name);PRINTLN(" gradient eqal not used");break;
				case Measurement::gradient_t::UNDEFINED:PRINT(this->_valve_name);PRINT(" ");PRINT(md.measurement->name);PRINTLN(" gradient undefined");break;
					case Measurement::gradient_t::NOT_MEASURED:PRINT(this->_valve_name);PRINT(" "); PRINT(md.measurement->name);PRINTLN(" NOT_MESUERED");break;
				    case Measurement::gradient_t::RISING_EDGE:
				    	PRINT(this->_valve_name);PRINT(" ");PRINT(md.measurement->name);PRINTLN(": RISING_EDGE ");
				    	PRINT(" measValNow: ");PRINT(measVal);
				    	PRINT(" theshold high: ");PRINT(md.hysteresis.threshold_high);
				    	PRINT(" theshold low: ");PRINT(md.hysteresis.threshold_low);
				    	PRINT(" required actvity: ");PRINT(md.hysteresis.requiredActivity);
						if ((measVal > md.hysteresis.threshold_high)){
							validThreshold = true;
							if (md.hysteresis.requiredActivity == Measurement::hysteresis_t::AT_THRESHOLD_OFF){ numberOfRequiredOFF++;PRINTLN(" > off requested");}
							else if (md.hysteresis.requiredActivity == Measurement::hysteresis_t::AT_THRESHOLD_ON){ numberOfRequiredON++;PRINTLN(" > on requested");}
						}
						else if((measVal <= md.hysteresis.threshold_low)){//hysteresis negation
							validThreshold = true;
							if (md.hysteresis.requiredActivity == Measurement::hysteresis_t::AT_THRESHOLD_OFF) {numberOfRequiredON++;PRINTLN(" <= on requested");}
							else if (md.hysteresis.requiredActivity == Measurement::hysteresis_t::AT_THRESHOLD_ON) {numberOfRequiredOFF++;PRINTLN(" <= off requested");}
						}
						break;
					case Measurement::gradient_t::FALLING_EDGE://so there are more command options
						PRINT(this->_valve_name);PRINT(" ");PRINT(md.measurement->name);PRINTLN(": FALLING_EDGE ");
				    	PRINT(" measValNow: ");PRINT(measVal);
				    	PRINT(" theshold high: ");PRINT(md.hysteresis.threshold_high);
				    	PRINT(" theshold low: ");PRINT(md.hysteresis.threshold_low);
				    	PRINT(" required actvity: ");PRINT(md.hysteresis.requiredActivity);
						if ((measVal < md.hysteresis.threshold_low)){
							validThreshold = true;
							if (md.hysteresis.requiredActivity == Measurement::hysteresis_t::AT_THRESHOLD_OFF){ numberOfRequiredOFF++;PRINTLN(" < off requested");}
							else if (md.hysteresis.requiredActivity == Measurement::hysteresis_t::AT_THRESHOLD_ON) {numberOfRequiredON++;PRINTLN(" < on requested");}
						}else if ((measVal >= md.hysteresis.threshold_high)){//hysteresis negation
							validThreshold = true;
							if (md.hysteresis.requiredActivity == Measurement::hysteresis_t::AT_THRESHOLD_OFF){ numberOfRequiredON++;PRINTLN(" >= on requested");}
							else if (md.hysteresis.requiredActivity == Measurement::hysteresis_t::AT_THRESHOLD_ON) {numberOfRequiredOFF++;PRINTLN(" >= off requested");}
						};
						break;
					case Measurement::gradient_t::RISING_OR_FALLING_EDGE://e.g. for timer -> in this case thresholds need to be read dynamically for periodic (e.g. daily) switching
						PRINT(this->_valve_name);PRINT(" ");PRINT(md.measurement->name);PRINTLN(": RISING_OR_FALLING_EDGE ");
				    	PRINT(" measValNow: ");PRINT(measVal);
				    	PRINT(" theshold high: ");PRINT(md.hysteresis.threshold_high);
				    	PRINT(" theshold low: ");PRINT(md.hysteresis.threshold_low);
				    	PRINT(" required actvity: ");PRINT(md.hysteresis.requiredActivity);
						if ((measVal > md.measurement->getDefaultConditions().threshold_high) || (measVal < md.measurement->getDefaultConditions().threshold_low)){
							validThreshold = true;
							if (md.hysteresis.requiredActivity == Measurement::hysteresis_t::AT_THRESHOLD_OFF) {numberOfRequiredOFF++;PRINTLN(" > < off requested");}
							else if (md.hysteresis.requiredActivity == Measurement::hysteresis_t::AT_THRESHOLD_ON) {numberOfRequiredON++;PRINTLN(" > < on requested");}
						}
						else if((measVal <= md.measurement->getDefaultConditions().threshold_high)|| (measVal >= md.measurement->getDefaultConditions().threshold_low)){//hysteresis negation
							validThreshold = true;
							if (md.hysteresis.requiredActivity == Measurement::hysteresis_t::AT_THRESHOLD_OFF){ numberOfRequiredON++;PRINTLN(" <= >=on requested");}
							else if (md.hysteresis.requiredActivity == Measurement::hysteresis_t::AT_THRESHOLD_ON) {numberOfRequiredOFF++;PRINTLN(" <= >= off requested");}
						}
						break;
				}


			}else{
				PRINT(md.measurement->name);PRINTLN(" sensor not detected! ");
			}
		}
		if (validThreshold == true){
			validThreshold = false;
			PRINT("required offs: "); PRINTLN(numberOfRequiredOFF);
			PRINT("required ons: "); PRINTLN(numberOfRequiredON);
			//OFF-request is dominant; used for OR-operation:
			if (numberOfRequiredOFF > 0){
				if (this->_valve_state == ON)switchValve(OFF);
			}
			else if(numberOfRequiredON > 0){
				if (this->_valve_state == OFF)switchValve(ON);
			}

		}
	}
}

//TODO: this is a zombie right now...start to use it!
void Valve::setAutoDependencyName(String alias){
	_autoDependency = alias;
}


//} /* namespace Gardena */

