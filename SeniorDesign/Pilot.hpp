#ifndef __APPAYNE_SENIORDESIGN_PILOT_H
#define __APPAYNE_SENIORDESIGN_PILOT_H

#include <stdint.h>
#include "Navigator.hpp"

typedef enum : uint8_t {
	motor_forward,
	motor_reverse,
	motor_offline,
} pilot_motor_dir_t;

typedef struct {
	uint8_t goal;
	uint8_t speed;
	uint8_t pin;
	pilot_motor_dir_t direction;
	bool status;
} pilot_motor_info_t;

typedef enum : uint8_t {
	move_forward,
	bank_left,
	bank_right,
	pivot_left,
	pivot_right,
	ahead_full,
	all_stop,
} maneouvre_t;

//  While these modules are largely just procedural code, having object-scoping
//  is nice. There's basically no difference in the compiled result between
//  using file-scoped variables with extern'd functions, and just using a
//  singleton object.
class Pilot {
public:
	Pilot(pilot_motor_info_t* left, pilot_motor_info_t* right);
	void init(void);
	void start(void);
	void halt(void);
	void restart(void);
	void set_speed(uint8_t speed);
	void set_routine(maneouvre_t routine);
	void adjust(pilot_motor_info_t* m);

#ifdef DEVEL
	void debug(void);
#endif
private:
	pilot_motor_info_t* left;
	pilot_motor_info_t* right;
	uint8_t cruising_speed = 0;
	bool running;
	maneouvre_t activity;
};

extern Pilot pilot;

#endif//__APPAYNE_SENIORDESIGN_PILOT_H
