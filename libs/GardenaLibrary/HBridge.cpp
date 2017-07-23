/*
 * HBridge.cpp
 *
 *  Created on: 20.10.2016
 *      Author: maro
 */


#include "HBridge.h"
#include "State.h"
#include "Debug.h"

//namespace Gardena {

HBridge::HBridge() {
	  _h_bridge_forward_contact1 = NULL;
	  _h_bridge_reverse_contact1 = NULL;
	  _h_bridge_forward_contact2 = NULL;
	  _h_bridge_reverse_contact2 = NULL;

}

HBridge::HBridge(Relay* frwd_contact1,Relay* rvs_contact1,Relay* frwd_contact2,Relay* rvs_contact2){
  _h_bridge_forward_contact1 = frwd_contact1;
  _h_bridge_reverse_contact1 = rvs_contact1;
  _h_bridge_forward_contact2 = frwd_contact2;
  _h_bridge_reverse_contact2 = rvs_contact2;

};




HBridge::~HBridge() {
	// TODO Auto-generated destructor stub
}

void HBridge::forwardOn(){
	_h_bridge_forward_contact1->on();
    delay(_CONTACT1_DELAY);//first contact turns on/off earlier/later to get a little less bouncing
    _h_bridge_forward_contact2->on();

}

void HBridge::forwardOff(){
	_h_bridge_forward_contact2->off();
    delay(_CONTACT1_DELAY);//first contact turns on/off earlier/later to get a little less bouncing
    _h_bridge_forward_contact1->off();

}

void HBridge::reverseOn(){
	_h_bridge_reverse_contact1->on();
    delay(_CONTACT1_DELAY);//first contact turns on/off earlier/later to get a little less bouncing
    _h_bridge_reverse_contact2->on();

}

void HBridge::reverseOff(){
	_h_bridge_reverse_contact2->off();
    delay(_CONTACT1_DELAY);//first contact turns on/off earlier/later to get a little less bouncing
    _h_bridge_reverse_contact1->off();

}


void HBridge::begin(){//this is because teensy forgets pinmode setting at constructor in Relay class
	_h_bridge_forward_contact1->begin();
	_h_bridge_forward_contact2->begin();
	_h_bridge_reverse_contact1->begin();
	_h_bridge_reverse_contact2->begin();
}



int HBridge::get_h_bridge_forward_contact1(){
  return  _h_bridge_forward_contact1->_relay_pin;

};
int HBridge::get_h_bridge_reverse_contact1(){
  return  _h_bridge_reverse_contact1->_relay_pin;

};
int HBridge::get_h_bridge_forward_contact2(){
  return  _h_bridge_forward_contact2->_relay_pin;

};
int HBridge::get_h_bridge_reverse_contact2(){
  return  _h_bridge_reverse_contact2->_relay_pin;

};


//} /* namespace Gardena */
