/*
 * Gardena.h
 *
 *  Created on: 23.07.2017
 *      Author: pionet
 */

#ifndef GARDENA_H_
#define GARDENA_H_

namespace GARDENA_ESP {

class Gardena {
public:
	Gardena();
	virtual ~Gardena();
	void begin();
	void handle();
private:
	void myfunction();
};

} /* namespace GARDENA_ESP */

#endif /* GARDENA_H_ */
