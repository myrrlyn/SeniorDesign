#ifndef __APPAYNE_SENIORDESIGN_ULTRASONIC_H
#define __APPAYNE_SENIORDESIGN_ULTRASONIC_H

#include <Utility.hpp>

class Ultrasonic {
public:
	Ultrasonic(iopair_t pins);
	uint32_t measure(void);
	uint8_t measure_inches(void);
	void status(bool status);
	bool status(void);
private:
	iopair_t _pins;
	bool _status = true;
};

#endif//__APPAYNE_SENIORDESIGN_ULTRASONIC_H
