/*
 * CommandList.h
 *
 *  Created on: 11.06.2017
 *      Author: maro
 */

#ifndef COMMANDLIST_H_
#define COMMANDLIST_H_
#include <vector>
#include "Command.h"
#include <ArduinoJson.h>

class CommandList {
public:
	CommandList();
	virtual ~CommandList();

	std::vector <Command*> _cmdList;
	//typedef std::vector <Command> cmdList_t;
	//cmdList_t _cmdList;
	void registerCmd(Command* cmd);
	String iterate(SMS* reply, String cmd);
};

#endif /* COMMANDLIST_H_ */
