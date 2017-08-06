/*
 * CommandList.cpp
 *
 *  Created on: 11.06.2017
 *      Author: maro
 */

#include "CommandList.h"
#include "Debug.h"

CommandList::CommandList() {


}

CommandList::~CommandList() {
	// TODO Auto-generated destructor stub
}

void CommandList::registerCmd(Command * cmd){
	_cmdList.push_back(cmd);
}

//return value used for SMS towards GSM, SMS* pointer for web response
String CommandList::iterate(SMS* reply, String cmd){
	//PRINTLN("CommandList::iterate(String) called");
	String retVal = "";
	String str = "";
	Command* c;
    for (std::vector<Command*>::const_iterator it = _cmdList.begin();
         it != _cmdList.end(); ++it)
    {
    	c= *it;
    	str = c->execute(reply, cmd);
    	if ((retVal != "")&&(str != "")){
    		retVal = retVal + ",";
    		//bracketCount++;//TODO: make json per ArduinoJson
    	}

    	if (str != ""){
    		retVal =  retVal + str;
    		str = "";
    	}
    	PRINT(" commandlist iterate: ");PRINTLN(retVal);
    }
    PRINT("return from commandlist iterate: ");PRINTLN(retVal);
	return retVal;
}


