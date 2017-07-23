/*
 * Relay.h
 *
 *  Created on: 20.10.2016
 *      Author: maro
 */

#ifndef RELAY_H_
#define RELAY_H_

//namespace Gardena {

class Relay {
public:

	Relay();
	Relay(int pin);
	virtual ~Relay();
	void relayTest();
	void on();
	void off();
	void begin();

	int _relay_pin;
	bool _relay_state;



};

//} /* namespace Gardena */

#endif /* RELAY_H_ */
