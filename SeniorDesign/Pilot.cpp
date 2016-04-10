#include <Arduino.h>
#include <stdint.h>

uint8_t current_speed;
uint8_t current_goal;
bool regular_movement = false;

uint32_t pilot_interval;
uint32_t accelerator = 0;
bool dir;

void start() {
	regular_movement = true;
}

void full_stop() {
	regular_movement = false;
	current_speed = 0;
	analogWrite(10, 0);
	analogWrite(11, 0);
}

void set_speed(uint8_t new_speed) {
	current_goal = new_speed;
}

void adjust() {
	if (current_goal > current_speed) {
		++current_speed;
	}
	// else if (current_goal > current_speed) {
	// 	current_speed += (current_goal - current_speed + 1) / 2;
	// }
	else if (current_goal < current_speed) {
		--current_speed;
	}
	// else if (current_goal < current_speed) {
	// 	current_speed -= (current_speed - current_goal + 1) / 2;
	// }
	// Serial.print("PWM Value: ");
	// Serial.println(current_speed);
	analogWrite(10, current_speed);
	analogWrite(11, current_speed);
}

bool change_gears = false;

void debug_motors() {
	//  When we reach target speed, set the flag
	// if (current_speed == current_goal) {
		// change_gears = true;
	// }
	// if (change_gears) {
		// change_gears = false;
		// if (dir) {
		// 	set_speed(current_goal + 20);
		// 	if (current_goal > 256 - 32) {
		// 		dir = false;
		// 	}
		// }
		// else {
		// 	set_speed(current_goal - 10);
		// 	if (current_goal < 16) {
		// 		dir = true;
		// 	}
		// }
	// 	Serial.print("New target: ");
	// 	Serial.println(current_goal);
	// }
	Serial.print("Blocked? ");
	Serial.println(regular_movement ? "Running" : "Blocked");
	Serial.print("Speed: ");
	Serial.println(current_speed);
}

SIGNAL(TIMER0_COMPA_vect) {
	if (regular_movement) {
		++accelerator;
		if (accelerator >= 500) {
			adjust();
			accelerator = 0;
		}
	}
}
