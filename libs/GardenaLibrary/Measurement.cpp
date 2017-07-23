/*
 * Measurement.cpp
 *
 *  Created on: 04.07.2017
 *      Author: pionet
 */

#include "Measurement.h"
#include "Debug.h"
#include<algorithm> //needed for std find() in vector


Measurement::Measurement(){
 _sensorPin = 255;
 _measurementInterval = DEFAULT_MEASUREMENT_INTERVAL;
 _measuredValNow = 0;
 _cmd = NULL;
 _cmdList = NULL;
 _objStatusTxt = "";
 _measurementDependencyVector.clear();
	_sensorDetected = false;
}

Measurement::Measurement(String alias, int sensorPin, uint32_t interval) {
	name = alias;
	_sensorPin = sensorPin;
	_measurementInterval = interval;
	_measuredValNow = 0;
	_cmd = NULL;
	_cmdList = NULL;
	_objStatusTxt = "";
	_measurementDependencyVector.clear();
	_sensorDetected = false;

}


Measurement::~Measurement() {
	// TODO Auto-generated destructor stub
}

void Measurement::begin(){
	PRINT(name);PRINTLN("::begin() called");
	_elapsedTime = 0;
	_objStatusTxt = "";
}

void Measurement::handle() {
	if (_elapsedTime >= _measurementInterval) {
		_elapsedTime = _elapsedTime - _measurementInterval;
		_measuredValNow = analogRead(_sensorPin);
		PRINT(name + ": ");PRINTLN(_measuredValNow);
	}
}

void Measurement::setInterval(uint32_t interval){
	_measurementInterval = interval;
}


//TODO: add more conditions on the need basis
bool Measurement::sensorDetected(){
	bool detected = false;
	if ((_measuredValNow >_sensorDetectConditions.hysteresis.threshold_high) && //e.g. Rain Sensor
			(_sensorDetectConditions.hysteresis.gradient == gradient_t::RISING_EDGE))
	{
		_sensorDetectConditions.sensorDetected = sensorDetectConditions_t::YES;
		detected = true;

	}
	else if ((_measuredValNow < _sensorDetectConditions.hysteresis.threshold_low) && //e.g Moistre Sensor
			(_sensorDetectConditions.hysteresis.gradient == gradient_t::FALLING_EDGE))
	{
		_sensorDetectConditions.sensorDetected = sensorDetectConditions_t::YES;
		detected = true;

	}
	else if (_sensorDetected == true){//set e.g from timer
		_sensorDetectConditions.sensorDetected = sensorDetectConditions_t::YES;
		detected = true;
	}
	//TODO: FALLING_OR_RISING
	else _sensorDetectConditions.sensorDetected = sensorDetectConditions_t::NO;

	return detected;

}



void Measurement::setCommandList(CommandList * cmdList){
	_cmd = new Command();
	_cmdList = cmdList;
	_cmdList->registerCmd(_cmd);
	initializeCmds();
	_objStatusTxt = "";

}


void Measurement::initializeCmds(){
	_cmd->addCmd(name, &cmdExecute,  &statusTxt ,this); //report currently measured value when SMS contains alias of measurement object
	//_cmd->addCmd(name + ")
}

/*not needed, gsm can process those commands
void Measurement::addDependency(Measurement * measurement){
	Measurement::measurementDependency_t md;
	md.measurement = measurement;
	md.hysteresis.gradient = gradient_t::NOT_MEASURED;
	md.hysteresis.threshold_reached = false;
	_measurementDependencyVector.push_back(md);


}
*/



void Measurement::cmdExecute(Command * objPtr){
	//TODO: check if aditional parameters are given in SMS, e.g. new threshold value, periodicity, measurement interval, etc
	Measurement * vref = (Measurement*)objPtr;
	std::vector<String>::const_iterator paramIterator;
	measurementDependencyVector_t::iterator measDepsIterator;
	//measurementDependency_t md;
	paramTree_t parTree;
	vref->_objStatusTxt = "";
	vref->_objStatusTxt = vref->_objStatusTxt + "->" + vref->name + ": " ;
	parTree.previousBranch = "initial";
	parTree.currentBranch = vref->name;
	parTree.nextBranch = "reset";
	vref->walkTheCommandLine(objPtr, parTree);
}

void Measurement::walkTheCommandLine(Command * objPtr,paramTree_t parTree){
	//if (cmdConditionalBranch(objPtr,parTree)) return;moved to gsm
	if (cmdResetBranch(objPtr,parTree)) return;
	else if (cmdThrHBranch(objPtr,parTree)) return;
	else if (cmdThrLBranch(objPtr,parTree)) return;
	else if (cmdGradientBranch(objPtr,parTree)) return;
	else finalBranch(objPtr,parTree);
}

/*
bool Measurement::cmdConditionalBranch(Command * objPtr,paramTree_t parTree){
	bool retVal = false;
	Measurement * vref = (Measurement*)objPtr;
	std::vector<String>::const_iterator paramIterator;
	Measurement::measurementDependencyVector_t::iterator measDepsIterator;
	Measurement::measurementDependency_t md;
	///////////////////////////////////////////////////////////////////////////////////////////////
	if ((parTree.currentBranch == vref->name) && (parTree.nextBranch == "conditional")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		String measObjAlias = "";
		for (paramIterator = vref->_cmd->cmdStrgVector.begin();paramIterator != vref->_cmd->cmdStrgVector.end();++paramIterator){
			measObjAlias = *paramIterator;
			measDepsIterator = vref->_measurementDependencyVector.begin();
			while (measDepsIterator != vref->_measurementDependencyVector.end()){
				md = *measDepsIterator;
				if (md.measurement->name == measObjAlias){
					//keeping own record for report conditions, others might want different, but us the same sensor
					md.hysteresis.gradient = md.measurement->getDefaultConditions().gradient;
					md.hysteresis.threshold_high = md.measurement->getDefaultConditions().threshold_high;
					md.hysteresis.threshold_low = md.measurement->getDefaultConditions().threshold_low;
					md.hysteresis.threshold_reached = md.measurement->getDefaultConditions().threshold_reached;
					md.hysteresis.requiredActivity = md.measurement->getDefaultConditions().requiredActivity;//TODO: is that necessary here?
					_objStatusTxt = _objStatusTxt + "report if " + md.measurement->name + " at ";
					_objStatusTxt = _objStatusTxt + " thrh=" + String(md.measurement->getDefaultConditions().threshold_high);
					_objStatusTxt = _objStatusTxt + " thrl=" + String(md.measurement->getDefaultConditions().threshold_low);
					_objStatusTxt = _objStatusTxt + " grad= " + String(md.measurement->getDefaultConditions().gradient);
					_objStatusTxt = _objStatusTxt + " act= " + String(md.measurement->getDefaultConditions().requiredActivity);
					*measDepsIterator = md;
					parTree.previousBranch = parTree.currentBranch;
					parTree.currentBranch = parTree.nextBranch;
					parTree.nextBranch = "final";
					break;
				}
				else {
					parTree.nextBranch = "reset";
				}
				measDepsIterator++;
			}
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine(objPtr, parTree);
		retVal = true;;
	}
	return retVal;
};

*/
//reset threshold conditions to default
bool Measurement::cmdResetBranch(Command * objPtr,paramTree_t parTree){
	bool retVal = false;
	Measurement * vref = (Measurement*)objPtr;
	std::vector<String>::const_iterator paramIterator;
	String parStrg = "";
	//Measurement::measurementDependencyVector_t::iterator measDepsIterator;
	//Measurement::measurementDependency_t md;
	///////////////////////////////////////////////////////////////////////////////////////////////
	if ((parTree.currentBranch == vref->name) && (parTree.nextBranch == "reset")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		paramIterator=find(vref->_cmd->cmdStrgVector.begin(),vref->_cmd->cmdStrgVector.end(),"reset");
		if (paramIterator != vref->_cmd->cmdStrgVector.end()){
			vref->initializeSensorThresholds();
			_objStatusTxt = _objStatusTxt + " thrh=" + String(vref->getDefaultConditions().threshold_high);
			_objStatusTxt = _objStatusTxt + " thrl=" + String(vref->getDefaultConditions().threshold_low);
			_objStatusTxt = _objStatusTxt + " grad= " + String(vref->getDefaultConditions().gradient);
			_objStatusTxt = _objStatusTxt + " act= " + String(vref->getDefaultConditions().requiredActivity);
			parTree.previousBranch = parTree.currentBranch;
			parTree.currentBranch = parTree.nextBranch;
			parTree.nextBranch = "final";
		}
		else{
			parTree.nextBranch = "define_thrh";
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine(objPtr, parTree);
		retVal = true;
	}
	return retVal;
};


//looks up higher threshold value (=thrH)
bool Measurement::cmdThrHBranch(Command * objPtr,paramTree_t parTree){
	bool retVal = false;
	Measurement * vref = (Measurement*)objPtr;
	std::vector<String>::const_iterator paramIterator;
	String parStrg = "";
	//Measurement::measurementDependencyVector_t::iterator measDepsIterator;
	//Measurement::measurementDependency_t md;
	///////////////////////////////////////////////////////////////////////////////////////////////
	if ((parTree.nextBranch == "define_thrh")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		paramIterator=find(vref->_cmd->cmdStrgVector.begin(),vref->_cmd->cmdStrgVector.end(),"thrh");
		if (paramIterator != vref->_cmd->cmdStrgVector.end()){
			paramIterator++;
			if (paramIterator != vref->_cmd->cmdStrgVector.end()){
				parStrg = *paramIterator;
				vref->_defaultConditions.threshold_high = parStrg.toInt();
				_objStatusTxt = _objStatusTxt + " thrh=" + String(vref->getDefaultConditions().threshold_high);
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "define_thrl";
			}
			else{
				//parameter missing
				_objStatusTxt = _objStatusTxt + " thrh=??? (value missed) ";
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "final";
			}
		}
		else{//threshold_high not given, using default and check out for theshold_low
			_objStatusTxt = _objStatusTxt + "using default thrh=" + String(vref->getDefaultConditions().threshold_high);//in case thrh keyword missed
			parTree.nextBranch = "define_thrl";
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine(objPtr, parTree);
		retVal = true;
	}
	return retVal;
};

//looks up lower threshold value (=thrL)
bool Measurement::cmdThrLBranch(Command * objPtr,paramTree_t parTree){
	bool retVal = false;
	Measurement * vref = (Measurement*)objPtr;
	std::vector<String>::const_iterator paramIterator;
	String parStrg = "";
	//Measurement::measurementDependencyVector_t::iterator measDepsIterator;
	//Measurement::measurementDependency_t md;
	///////////////////////////////////////////////////////////////////////////////////////////////
	if ((parTree.nextBranch == "define_thrl")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		paramIterator=find(vref->_cmd->cmdStrgVector.begin(),vref->_cmd->cmdStrgVector.end(),"thrl");
		if (paramIterator != vref->_cmd->cmdStrgVector.end()){
			paramIterator++;
			if (paramIterator != vref->_cmd->cmdStrgVector.end()){
				parStrg = *paramIterator;
				vref->_defaultConditions.threshold_low = parStrg.toInt();
				_objStatusTxt = _objStatusTxt + " thrl=" + String(vref->getDefaultConditions().threshold_low);
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "define_gradient";
			}
			else{
				//parameter missing
				_objStatusTxt = _objStatusTxt + " thrl=??? (value missed) ";
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "final";
			}
		}
		else{//threshold_low not given, using default and check out for gradient
			parTree.nextBranch = "define_gradient";
			_objStatusTxt = _objStatusTxt + "using default thrl=" + String(vref->getDefaultConditions().threshold_low);//in case thrl keyword missed

		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine(objPtr, parTree);
		retVal = true;
	}
	return retVal;
};

//looks up gradient value (=grad)
bool Measurement::cmdGradientBranch(Command * objPtr,paramTree_t parTree){
	bool retVal = false;
	Measurement * vref = (Measurement*)objPtr;
	std::vector<String>::const_iterator paramIterator;
	String parStrg = "";
	//Measurement::measurementDependencyVector_t::iterator measDepsIterator;
	//Measurement::measurementDependency_t md;
	///////////////////////////////////////////////////////////////////////////////////////////////
	if ((parTree.nextBranch == "define_gradient")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		paramIterator=find(vref->_cmd->cmdStrgVector.begin(),vref->_cmd->cmdStrgVector.end(),"grad");
		if (paramIterator != vref->_cmd->cmdStrgVector.end()){
			paramIterator++;
			if (paramIterator != vref->_cmd->cmdStrgVector.end()){
				parStrg = *paramIterator;
				vref->_defaultConditions.gradient = vref->_grad_map.strgToGradient(parStrg);
				_objStatusTxt = _objStatusTxt + " grad=" + String(vref->getDefaultConditions().gradient);
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "final";
			}
			else{
				//parameter missing;
				_objStatusTxt = _objStatusTxt + " grad=??? (value missed) ";
				parTree.previousBranch = parTree.currentBranch;
				parTree.currentBranch = parTree.nextBranch;
				parTree.nextBranch = "final";
			}
		}
		else{//gradient not given, using default and go to final
			parTree.nextBranch = "final";
			_objStatusTxt = _objStatusTxt + "using default grad=" + String(vref->getDefaultConditions().gradient);//in case grad keyword missed
		}
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		this->walkTheCommandLine(objPtr, parTree);
		retVal = true;
	}
	return retVal;
};


bool Measurement::finalBranch(Command * objPtr,paramTree_t parTree){
	bool retVal = false;
	Measurement * vref = (Measurement*)objPtr;
	if (/*(parTree.currentBranch == vref->name) &&*/(parTree.nextBranch == "final")){
		PRINT("previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		if (parTree.currentBranch == vref->name){//no valid additional parameter has been found, general status will be reported
			_objStatusTxt = " measVal=" + String(vref->getCurrentMeasVal());
			_objStatusTxt = _objStatusTxt + " thrh=" + String(vref->getDefaultConditions().threshold_high);
			_objStatusTxt = _objStatusTxt + " thrl=" + String(vref->getDefaultConditions().threshold_low);
			_objStatusTxt = _objStatusTxt + " grad=" + String(vref->getDefaultConditions().gradient);
		}
		parTree.previousBranch = parTree.currentBranch;
		parTree.currentBranch = "final";
		parTree.nextBranch = "initial";
		PRINT("OUT: previousBranch: ");PRINT(parTree.previousBranch);PRINT(" currBranch: ");PRINT(parTree.currentBranch);PRINT(" nextBranch: ");PRINTLN(parTree.nextBranch);
		retVal = true;
	}
	return retVal;
};

String Measurement::statusTxt(SMS* smsReply,Command * objPtr){
	PRINT("Measurement::statusTxt called");
	Measurement *m = (Measurement*)objPtr;
	smsReply->msg = smsReply->msg + m->name + "=" + m->_measuredValNow;
	PRINTLN("SMS reply: " + smsReply->msg);
	m->_objStatusTxt = "\"" + m->name + "\":\"" + m->_objStatusTxt + "\"";
	return m->_objStatusTxt;//filled in execution function
}

int Measurement::getCurrentMeasVal(){
	return _measuredValNow;
}


Measurement::hysteresis_t Measurement::getDefaultConditions(){
	return _defaultConditions;
}

