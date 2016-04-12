/* === SeniorDesign ===
 * Alexander Payne
 *
 * This is the root file of the program. The Arduino framework constructs a
 * main() function that will initialize the board, call setup() once, and call
 * loop() forever.
 */

#include <Utility.hpp>

#include "Lookout.hpp"
#include "Navigator.hpp"
#include "Pilot.hpp"

uint32_t interval;
uint32_t actions;

void setup() {
	Serial.begin(115200);
	Serial.println("Arduino online!");
	Serial.println();

	pinMode(2, INPUT);

	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

	gps.begin(9600);
	compass.begin();
	compass.set_gain(mag_gain_0230);

	gps.command(GPS_OUTPUT_ALL);
	gps.command(GPS_FIX_UPDATE_200);
	gps.command(GPS_PRINT_200);

	OCR1A = 0xAF;
	TIMSK1 |= _BV(OCIE1A);

	pilot.start();
	pilot.set_routine(turn_left);

	delay(500);

	interval = millis();
	actions = millis();
	pilot.set_speed(127);

}

bool flags[] = {
	true, false, false, false, false,
};

void loop() {
	us_scan_head();
	pilot.debug();
	// if (millis() - interval > 1000) {
	// 	gps.debug();
	// 	debug_mag();
	// 	interval = millis();
	// }
	// delay(500);
	/*
	if (flags[0] && (millis() - actions > 0)) {
		Serial.println("MOVING FORWARD");
		pilot.full_stop();
		pilot.start();
		pilot.set_routine(move_forward);
		flags[0] = false;
		flags[1] = true;
	}
	else if (flags[1] && (millis() - actions > 20000)) {
		Serial.println();
		Serial.println("TURNING LEFT");
		Serial.println();
		pilot.full_stop();
		pilot.start();
		pilot.set_routine(turn_left);
		flags[1] = false;
		flags[2] = true;
	}
	else if (flags[2] && (millis() - actions > 40000)) {
		Serial.println();
		Serial.println("MOVING FORWARD");
		Serial.println();
		pilot.full_stop();
		pilot.start();
		pilot.set_routine(move_forward);
		flags[2] = false;
		flags[3] = true;
	}
	else if (flags[3] && (millis() - actions > 60000)) {
		Serial.println();
		Serial.println("TURNING RIGHT");
		Serial.println();
		pilot.full_stop();
		pilot.start();
		pilot.set_routine(turn_right);
		flags[3] = false;
		flags[4] = true;
	}
	else if (flags[4] && (millis() - actions > 80000)) {
		actions = millis();
		flags[0] = true;
		flags[1] = false;
		flags[2] = false;
		flags[3] = false;
		flags[4] = false;
	}
	*/
}
