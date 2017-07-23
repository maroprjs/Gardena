/**
 *  @file		teensy3_2.ino
 *
 *  @author		maro
 *  @date		mm/dd/yyyy
 *  @version	x.y
 *
 *  @brief 		Main
 *
 *  @section DESCRIPTION
 *
 *  control gardena watering valves through gsm sms
 *
 */
#include "Gardena.h"

Gardena gardena;

void setup()
{
	gardena.begin();

}


void loop()
{
	gardena.handle();
}

