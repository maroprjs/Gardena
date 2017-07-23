/*
 * Command.h
 *
 *  Created on: 25.10.2016
 *      Author: maro
 */

#ifndef COMMAND_H_
#define COMMAND_H_

//#include "Valve.h"
#include <Arduino.h>
#include <vector>
//#include <string>
//#include "CommandList.h"
#include "SMS.h"

//namespace Gardena {

class Command {
public://types

	//typedef std::vector <String> valve_on_cmd_variations_list_t;
    struct cmdVariation
    {
    	String cmdTxt;
    	void (*exeFunction)(Command * objPtr);
    	String (*statusFunction)(SMS* reply,Command * objPtr);
    	Command * objPtr;
    } ;

    typedef cmdVariation cmdVariation_t;
    typedef std::vector<cmdVariation_t> cmdVariationVector_t;
	//String (*_execute)(String cmd) = NULL;

	struct paramTree
	{
		String previousBranch = "";
		String currentBranch = "";
		String nextBranch = "";
	};
	typedef paramTree paramTree_t;

public: //functions
    Command();
	virtual ~Command();
	void addCmd(String cmdTxt, void (*exeFunction)(Command * objPtr),String (*statusFunction)(SMS* reply,Command * objPtr), Command * objPtr);
	String execute(SMS* reply,String cmd);

public://variables
    std::vector<String>cmdStrgVector;

private: //functions
    bool Contains(String s, String search);
	String getSubstring(String data, char separator, int index);
	void cmdToStrgVector(String sms);

private://variables
	String _currentCmd;
    cmdVariationVector_t _cmdVariationVector;


};

//} /* namespace Gardena */

#endif /* COMMAND_H_ */
