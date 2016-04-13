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

void setup() {
	Serial.begin(115200);
	Serial.println("Arduino online!");
	Serial.println();

	pinMode(2, INPUT);

	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

	nav.gps()->begin(9600);
	nav.mag()->begin();
	nav.mag()->set_gain(mag_gain_0230);

	nav.gps()->command(GPS_OUTPUT_ALL);
	nav.gps()->command(GPS_FIX_UPDATE_200);
	nav.gps()->command(GPS_PRINT_200);

	OCR1A = 0xAF;
	TIMSK1 |= _BV(OCIE1A);

	pilot.start();
	pilot.set_routine(turn_left);

	delay(500);

	pilot.set_speed(127);

}

bool flags[] = {
	true, false, false, false, false,
};

gps_coord_t demo_coord = {
	.latitude = { .i = 41379500 },
	.longitude = { .i = -85003400 },
};

uint32_t interval_us = 0;
uint32_t interval_nav = 0;
uint32_t interval_actions = 0;
gps_coord_t dist;

void loop() {
	if (millis() - interval_us > 500) {
		us_scan_all();
		interval_us = millis();
	}
	// pilot.debug();
	if (false && millis() - interval_nav > 1000) {
		dist = nav.distance_to(demo_coord);
		Serial.print("Latitude difference:  ");
		Serial.println(dist.latitude.i);
		Serial.print("Longitude difference: ");
		Serial.println(dist.longitude.i);
		nav.gps()->debug();
		// nav.mag()->debug();
		debug_mag();
		interval_nav = millis();
	}
	/*
	//  This is a simple choreography script to demonstrate different pilot
	//  control methods.
	if (flags[0] && (millis() - interval_actions > 0)) {
		Serial.println("MOVING FORWARD");
		pilot.full_stop();
		pilot.start();
		pilot.set_routine(move_forward);
		flags[0] = false;
		flags[1] = true;
	}
	else if (flags[1] && (millis() - interval_actions > 20000)) {
		Serial.println();
		Serial.println("TURNING LEFT");
		Serial.println();
		pilot.full_stop();
		pilot.start();
		pilot.set_routine(turn_left);
		flags[1] = false;
		flags[2] = true;
	}
	else if (flags[2] && (millis() - interval_actions > 40000)) {
		Serial.println();
		Serial.println("MOVING FORWARD");
		Serial.println();
		pilot.full_stop();
		pilot.start();
		pilot.set_routine(move_forward);
		flags[2] = false;
		flags[3] = true;
	}
	else if (flags[3] && (millis() - interval_actions > 60000)) {
		Serial.println();
		Serial.println("TURNING RIGHT");
		Serial.println();
		pilot.full_stop();
		pilot.start();
		pilot.set_routine(turn_right);
		flags[3] = false;
		flags[4] = true;
	}
	else if (flags[4] && (millis() - interval_actions > 80000)) {
		actions = millis();
		flags[0] = true;
		flags[1] = false;
		flags[2] = false;
		flags[3] = false;
		flags[4] = false;
	}
	*/
}
