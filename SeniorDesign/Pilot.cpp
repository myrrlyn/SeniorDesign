#include "Pilot.hpp"

#include <Arduino.h>
#include <stdint.h>

Pilot::Pilot() {
	left = {
		.goal = 0,
		.speed = 0,
		.direction = motor_offline,
		.status = false,
	};
	right = {
		.goal = 0,
		.speed = 0,
		.direction = motor_offline,
		.status = false,
	};
	running = false;
	activity = move_forward;
}

void Pilot::start() {
	left.status = true;
	right.status = true;
	running = true;
}

void Pilot::full_stop() {
	left.status = false;
	left.direction = motor_offline;
	left.speed = 0;
	right.status = false;
	right.direction = motor_offline;
	right.speed = 0;
	running = false;
	activity = all_stop;
	analogWrite(10, 0);
	analogWrite(11, 0);
}

void Pilot::set_speed(uint8_t speed) {
	left.goal = speed;
	right.goal = speed;
}

void Pilot::set_routine(maneouvre_t routine) {
	activity = routine;
}

void Pilot::adjust() {
#ifdef DEVEL
	// Serial.println("Adjusting speed");
#endif
	switch (activity) {
		case move_backwards:  //  Intentional fallthrough
		case move_forward: {
			adjust('l');
			adjust('r');
			break;
		}
		case turn_left: {
			stop('l');
			adjust('r');
			break;
		}
		case turn_right: {
			adjust('l');
			stop('r');
			break;
		}
		case all_stop:
		default: {
			full_stop();
		}
	}
}

void Pilot::adjust(char motor) {
	switch (motor) {
		case 'L':  //  Intentional fallthrough
		case 'l': {
			if (left.status == true) {
				if (left.goal > left.speed) {
					++left.speed;
				}
				else if (left.goal < left.speed) {
					--left.speed;
				}
				analogWrite(10, left.speed);
			}
			break;
		}
		case 'R':  //  Intentional fallthrough
		case 'r': {
			if (right.status == true) {
				if (right.goal > right.speed) {
					++right.speed;
				}
				else if (right.goal < right.speed) {
					--right.speed;
				}
				analogWrite(11, right.speed);
			}
			break;
		}
		default: {
#ifdef DEVEL
			Serial.println("ERROR: ADJUSTED A NON-EXISTENT MOTOR!");
#endif
			break;
		}
	}
}

void Pilot::stop(char motor) {
	switch (motor) {
		case 'L':
		case 'l': {
			left.speed = 0;
			analogWrite(10, 0);
			break;
		}
		case 'R':
		case 'r': {
			right.speed = 0;
			analogWrite(11, 0);
			break;
		}
		default: {
			full_stop();
		}
	}
}

void Pilot::debug() {
	Serial.print("Status: ");
	Serial.println(pilot.running ? "Running" : "Blocked");
	Serial.print("Speed: ");
	Serial.print(left.speed);
	Serial.print(", ");
	Serial.println(right.speed);
}

uint32_t accelerator;
Pilot pilot;

SIGNAL(TIMER1_COMPA_vect) {
	++accelerator;
	if (accelerator >= 250) {
		pilot.adjust();
		accelerator = 0;
	}
}
