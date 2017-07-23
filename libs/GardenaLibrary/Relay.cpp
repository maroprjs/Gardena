/*
 * Relay.cpp
 *
 *  Created on: 20.10.2016
 *      Author: maro
 */

#include "Relay.h"
//#include "Arduino.h"
#include "State.h"

//namespace Gardena {

Relay::Relay() {

	_relay_pin = 255;
	_relay_state = OFF;


}

Relay::Relay(int pin){
  _relay_pin = pin;
  _relay_state = OFF;
  pinMode(_relay_pin, OUTPUT);
  relayTest();
  digitalWrite(_relay_pin,_relay_state);
}

Relay::~Relay() {
	// TODO Auto-generated destructor stub
}
void Relay::relayTest(){
	delay(500);
	digitalWrite(_relay_pin,ON);
	delay(500);
	digitalWrite(_relay_pin,OFF);
}

void Relay::on(){
	digitalWrite(_relay_pin,ON);
}
void Relay::off(){
	digitalWrite(_relay_pin,OFF);
}

void Relay::begin(){
	  pinMode(_relay_pin, OUTPUT);//at least Teensy pins 18&19 forget pinmode setting, done in constructor!
	  digitalWrite(_relay_pin,_relay_state);
}


//} /* namespace Gardena */
