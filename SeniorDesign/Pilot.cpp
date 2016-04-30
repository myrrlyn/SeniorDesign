#include "Pilot.hpp"

#include <Arduino.h>
#include <stdint.h>

#define PILOT_ACCEL_CLOCK 0x0018

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
	running = true;
}

void Pilot::halt() {
	running = false;
	left->speed = 0;
	right->speed = 0;
}

void Pilot::restart() {
	running = true;
	left->speed = 0;
	right->speed = 0;
	analogWrite(10, 0);
	analogWrite(11, 0);
}

void Pilot::set_speed(uint8_t speed) {
	cruising_speed = speed;
	left->goal = speed;
	right->goal = speed;
}

void Pilot::set_routine(maneouvre_t routine) {
	activity = routine;
	switch (routine) {
		case ahead_full:
			left->speed = cruising_speed;
			right->speed = cruising_speed;
		case move_forward:
			left->goal = cruising_speed;
			right->goal = cruising_speed;
			left->status = true;
			right->status = true;
			break;
		//  Do not accelerate when banking; just jump speeds immediately.
		case bank_left:
			left->goal = (cruising_speed * 4) / 5;
			right->goal = cruising_speed;
			// left->speed = left->goal;
			// right->speed = right->goal;
			break;
		case bank_right:
			left->goal = cruising_speed;
			right->goal = (cruising_speed * 4) / 5;
			// left->speed = left->goal;
			// right->speed = right->goal;
			break;
		//  When pivoting, clamp the relevant motor to 0.
		//  Take care to ensure that the clamped motor accelerates rather than
		//  jumps to restore speed.
		case pivot_left:
			left->status = false;
			right->status = true;
			break;
		case pivot_right:
			left->status = true;
			right->status = false;
			break;
		case all_stop:
			left->status = false;
			right->status = false;
			break;
	}
}

void Pilot::adjust(pilot_motor_info_t* m) {
	if (running && m->status) {
		switch (activity) {
			case move_forward:
				if (m->speed < m->goal) {
					++(m->speed);
				}
				else if (m->speed > m->goal) {
					--(m->speed);
				}
				break;
			case ahead_full:
				m->speed = cruising_speed;
				break;
			case bank_left:
			case bank_right:
			case pivot_left:
			case pivot_right:
				m->speed = m->goal;
				break;
			case all_stop:
				m->speed = 0;
				break;
		}
	}
	else {
		m->speed = 0;
	}
	analogWrite(m->pin, m->speed);
}

#ifdef DEVEL
void Pilot::debug() {
	Serial.println("MOTORS");
	Serial.print("Status: ");
	Serial.println(pilot.running ? "Ready" : "Blocked");
	Serial.print("Activity: ");
	switch (activity) {
		case ahead_full:
		case move_forward:
			Serial.println("MOVING FORWARD");
			break;
		case bank_left:
			Serial.println("BANKING LEFT");
			break;
		case bank_right:
			Serial.println("BANKING RIGHT");
			break;
		case pivot_left:
			Serial.println("PIVOTING LEFT");
			break;
		case pivot_right:
			Serial.println("PIVOTING RIGHT");
			break;
		case all_stop:
			Serial.println("HALTING");
			break;
	}
	Serial.print("Speed: ");
	Serial.print(left->speed);
	Serial.print(", ");
	Serial.println(right->speed);
	Serial.println();
}
#endif

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
	if (clock_l == PILOT_ACCEL_CLOCK) {
		pilot.adjust(&ctl_left);
		clock_l = 0x0000;
	}
	++clock_r;
	if (clock_r == PILOT_ACCEL_CLOCK) {
		pilot.adjust(&ctl_right);
		clock_r = 0x0000;
	}
}
