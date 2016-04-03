#ifndef __APPAYNE_SENIORDESIGN_TACHOMETER_H
#define __APPAYNE_SENIORDESIGN_TACHOMETER_H

#include <Utility.hpp>

typedef enum : uint8_t {
	mag_pole_north = 0,
	mag_pole_south = 1,
} mag_pole_t;

class Tachometer {
public:
	Tachometer(gpio_t pin, double magic_number = 15000.0);
	void begin(void);
	bool read(void);
	uint32_t interval(void);
	double rpm(void);
	void reset(void);
private:
	double calc_rpm(uint32_t delta);
	double magic_number;
	double current_rpm;
	uint32_t time;
	uint32_t delta;
	gpio_t pin;
	mag_pole_t mag_pole;
	bool state;
};

#endif//__APPAYNE_SENIORDESIGN_TACHOMETER_H
