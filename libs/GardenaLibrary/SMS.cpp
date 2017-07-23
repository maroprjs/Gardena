/*
 * SMS.cpp
 *
 *  Created on: 24.10.2016
 *      Author: maro
 */

#include "SMS.h"

//namespace Gardena {

SMS::SMS() {
	// TODO Auto-generated constructor stub
	smsMaxLen = 250;
	smsIdx = 0;
	smslen = 0;
	newMsgArrived = false;

}

SMS::~SMS() {
	// TODO Auto-generated destructor stub
}

//} /* namespace Gardena */
