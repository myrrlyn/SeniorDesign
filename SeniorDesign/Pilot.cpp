#include "Pilot.hpp"

#include <Arduino.h>
#include <stdint.h>

Pilot::Pilot(pilot_motor_info_t* left, pilot_motor_info_t* right) {
	this->left = left;
	this->right = right;
	running = false;
	activity = move_forward;
}

void Pilot::init() {
	//  Set up Timer 1
	//  See ATMega2560 datasheet, chapter 17
	//  See doc/Pilot.md

	TCCR1A = 0xA3;
	TCCR1B = 0x11;
	TCCR1C = 0x00;

	//  OCR1A is set by the runtime
	//  OCR1B is set by the runtime

	//  ICR1 sets the maximum value to which the counter runs. For now, set it
	//  to 0x00FF (255) so that the current API can remain unchanged.
	ICR1 = 0x00FF;

	TIMSK1 = 0x00;
}

void Pilot::start() {
	left->status = true;
	right->status = true;
	running = true;
}

void Pilot::set_speed(uint8_t speed) {
	cruising_speed = speed;
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
		*(m->pin) = m->speed;
	}
	else {
		left->speed = 0;
		right->speed = 0;
		left->status = false;
		right->status = false;
		left->direction = motor_offline;
		right->direction = motor_offline;
		running = false;
		*(left->pin) = left->speed;
		*(right->pin) = right->speed;
		// analogWrite(right->pin, right->speed);
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

uint16_t clock_left;
uint16_t clock_right;
pilot_motor_info_t ctl_left = {
	.goal = 0,
	.speed = 0,
	.pin = &OCR1A,
	.direction = motor_offline,
	.status = false,
};
pilot_motor_info_t ctl_right = {
	.goal = 0,
	.speed = 0,
	.pin = &OCR1B,
	.direction = motor_offline,
	.status = false,
};
Pilot pilot(&ctl_left, &ctl_right);

SIGNAL(TIMER3_COMPA_vect) {
	UDR0 = '3';
	// ++clock_left;
	// ++clock_right;
	// if (clock_left > ctl_left.timer) {
		// pilot.adjust(&ctl_left);
		// clock_left = 0;
	// }
	// if (clock_right > ctl_right.timer) {
		// pilot.adjust(&ctl_right);
		// clock_right = 0;
	// }
}

//  DEBUGGING
uint8_t compa, compb;

SIGNAL(TIMER1_COMPA_vect) {
	++compa;
	if (!compb) {
		UDR0 = 'a';
	}
}

SIGNAL(TIMER1_COMPB_vect) {
	++compb;
	if (!compb) {
		UDR0 = 'b';
	}
}
