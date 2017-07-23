/*
 * Command.cpp
 *
 *  Created on: 25.10.2016
 *      Author: MARO
 */

#include "Command.h"
#include "Debug.h"

//namespace Gardena {

Command::Command() {
	// TODO Auto-generated constructor stub
	//_cmd = "";
	//_valve = NULL;
	//_cmdList = cmdList;
	//_execute = execute;
}

/*
Command::Command(CommandList * cmdList) {
	// TODO Auto-generated constructor stub
	//_cmd = "";
	//_valve = NULL;
	_cmdList = cmdList;
	_cmdList->registerCmd(this);

}*/

Command::~Command() {
	// TODO Auto-generated destructor stub
}

void Command::addCmd(String cmdTxt, void (*exeFunction)(Command * objPtr),String (*statusFunction)(SMS* reply,Command * objPtr), Command * objPtr){
	cmdVariation_t cmdVar;
	cmdVar.cmdTxt = cmdTxt;
	cmdVar.exeFunction = exeFunction;
	cmdVar.statusFunction = statusFunction;
	cmdVar.objPtr = objPtr;
	_cmdVariationVector.push_back(cmdVar);
}


//{"scope":"valves", "data":{"valve1":{"status":"on","info":"rain and moist"},"valve2":{"status":"off","info":"man"}}}
//{"valve1":{"status":"on","info":"rain and moist"},"valve2":{"status":"off","info":"man"}}
String Command::execute(SMS* reply,String cmd){
	String retVal = "";

	PRINTLN("Command::execute called");

	//store command for later extraction of paramters inside specific object
	_currentCmd = cmd;//this stores raw format for later insertion into vector

	//format command;
	cmd.trim();
	cmd.replace(" ", "");
	cmd.toLowerCase();

	//for each registered command variation execute (store cmd params in vector, to be retrieved by the parent object itself)
    for (std::vector<cmdVariation_t>::const_iterator it = _cmdVariationVector.begin();
         it != _cmdVariationVector.end(); ++it)
    {
    	PRINT("cmd: ");PRINT(cmd);PRINT(" cmdText: ");PRINTLN(it->cmdTxt);
        if (Contains(cmd, it->cmdTxt) == true ){//https://stackoverflow.com/questions/5029612/how-to-match-text-in-string-in-arduino
        	cmdToStrgVector(_currentCmd);//the dependent object extracts its parameters from command->cmdStrgVector
        	it->exeFunction(it->objPtr);
        	retVal = retVal + it->statusFunction(reply, it->objPtr);
        	PRINT("it return: "); PRINTLN(reply->msg);
        }
    }


	return retVal;
}




//https://stackoverflow.com/questions/5029612/how-to-match-text-in-string-in-arduino
/*
bool Command::Contains(String s, String search) {
    int max = s.length() - search.length();

    for (int i = 0; i <= max; i++) {
        if (s.substring(i) == search) return true; // or i
    }

    return false; //or -1
}
*/
bool Command::Contains(String s, String search) {
	bool retVal = false;
	if (search.length() > s.length()) return retVal;
	if (s.substring(0, search.length()) == search) retVal = true;

    return retVal;
}

//https://stackoverflow.com/questions/9072320/split-string-into-string-array
String Command::getSubstring(String data, char separator, int index)
{
 int found = 0;
 int strIndex[] = {0, -1};
 int maxIndex = data.length()-1;

 for(int i=0; i<=maxIndex && found<=index; i++){
   if(data.charAt(i)==separator || i==maxIndex){
       found++;
       strIndex[0] = strIndex[1]+1;
       strIndex[1] = (i == maxIndex) ? i+1 : i;
   }
 }

 return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void Command::cmdToStrgVector(String sms){
	int i = 0;
	cmdStrgVector.clear();
	String substring = getSubstring(sms,' ',i);
	//if (substring != "") cmdStrgVector.push_back(substring.toLowerCase());

	while (substring != ""){
		cmdStrgVector.push_back(substring.toLowerCase());
		i++;
		substring = getSubstring(sms,' ',i);

	}
}






//} /* namespace Gardena */
