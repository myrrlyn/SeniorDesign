#include "Pilot.hpp"

#include <Arduino.h>
#include <stdint.h>

Pilot::Pilot(pilot_motor_info_t* left, pilot_motor_info_t* right) {
	this->left = left;
	this->right = right;
	running = false;
	activity = all_stop;
}

void Pilot::init() {
	//  Set up Timer 3
	//  We are using this as a software timer to accelerate the motors
	OCR3A = 0xAF;
	TIMSK3 |= _BV(OCIE3A);
}

void Pilot::start() {
	left->status = true;
	right->status = true;
	running = true;
}

void Pilot::set_speed(uint8_t speed) {
	cruising_speed = speed;
	left->goal = speed;
	right->goal = speed;
}

void Pilot::set_routine(maneouvre_t routine) {
	activity = routine;
	switch (routine) {
		case move_backwards:
		case move_forward:
			left->goal = cruising_speed;
			right->goal = cruising_speed;
			break;
		case turn_left:
			left->goal = cruising_speed >> 2;
			right->goal = cruising_speed;
			break;
		case turn_right:
			left->goal = cruising_speed;
			right->goal = cruising_speed >> 2;
			break;
		case all_stop:
			left->status = false;
			left->status = false;
			running = false;
	}
}

void Pilot::adjust(pilot_motor_info_t* m) {
	if (activity != all_stop) {
		if (running && m->speed < m->goal) {
			m->speed += 1;
		}
		else if (running && m->speed > m->goal) {
			m->speed -= 1;
		}
		analogWrite(m->pin, m->speed);
	}
	else {
		left->speed = 0;
		right->speed = 0;
		left->status = false;
		right->status = false;
		left->direction = motor_offline;
		right->direction = motor_offline;
		running = false;
		analogWrite(10, 0);
		analogWrite(11, 0);
	}
}

void Pilot::debug() {
	Serial.print("Status: ");
	Serial.println(pilot.running ? "Running" : "Blocked");
	Serial.print("Speed: ");
	Serial.print(left->speed);
	Serial.print(", ");
	Serial.println(right->speed);
}

pilot_motor_info_t ctl_left = {
	.goal = 0,
	.speed = 0,
	.pin = 10,
	.direction = motor_offline,
	.status = false,
};
pilot_motor_info_t ctl_right = {
	.goal = 0,
	.speed = 0,
	.pin = 11,
	.direction = motor_offline,
	.status = false,
};

uint16_t clock_l = 0;
uint16_t clock_r = 0;

Pilot pilot(&ctl_left, &ctl_right);

SIGNAL(TIMER3_COMPA_vect) {
	++clock_l;
	if (clock_l == 0x00FF) {
		pilot.adjust(&ctl_left);
		clock_l = 0x00;
	}
	++clock_r;
	if (clock_r == 0x00FF) {
		pilot.adjust(&ctl_right);
		clock_r = 0x00;
	}
}
