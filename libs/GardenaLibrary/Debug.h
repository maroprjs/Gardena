/*
 * Debug.h
 *
 *  Created on: 21.10.2016
 *      Author: maro
 */

#ifndef DEBUG_H_
#define DEBUG_H_





//#define DEBUG
#ifdef DEBUG
	#define PRINT_INIT(x)	Serial.begin(x)
	#define PRINT(x)  	Serial.print(x)
	#define PRINTLN(x)  Serial.println(x)
	#define ZVERSION "debug xxx " + String(__DATE__)
	#define ZUP_TIME " " + String(millis()/1000) + " [s]"

#else
	#define PRINT_INIT(...)
	#define PRINT(...)
	#define PRINTLN(...)
	#define ZVERSION "v1.1.0" + String(__DATE__)
	#define ZUP_TIME " " + String(millis()/1000) + " [s]"
#endif

#endif /* DEBUG_H_ */
