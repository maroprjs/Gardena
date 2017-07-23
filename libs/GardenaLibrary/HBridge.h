/*
 * HBridge.h
 *
 *  Created on: 20.10.2016
 *      Author: maro
 */

#ifndef HBRIDGE_H_
#define HBRIDGE_H_

#include "Arduino.h"
#include "Relay.h"

//namespace Gardena {

class HBridge {

public:
	HBridge();
	HBridge(Relay* frwd_contact1,Relay* rvs_contact1,Relay* frwd_contact2,Relay* rvs_contact2);
	virtual ~HBridge();
	void forwardOn();
	void forwardOff();
	void reverseOn();
	void reverseOff();
	void begin();

private://var
	  Relay* _h_bridge_forward_contact1;
	  Relay* _h_bridge_reverse_contact1;
	  Relay* _h_bridge_forward_contact2;
	  Relay* _h_bridge_reverse_contact2;
	  const uint32_t _CONTACT1_DELAY = 500; //[ms] delay ON/OFF state for forward and reverse contact1
private: //functions
	  int get_h_bridge_forward_contact1();
	  int get_h_bridge_reverse_contact1();
	  int get_h_bridge_forward_contact2();
	  int get_h_bridge_reverse_contact2();


};

//} /* namespace Gardena */

#endif /* HBRIDGE_H_ */
