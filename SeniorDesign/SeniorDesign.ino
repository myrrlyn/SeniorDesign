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

bool flags[] = {
	false,
	false,
	false,
	false,
	false,
};

uint32_t interval_us = 0;
uint32_t interval_nav = 0;
uint32_t interval_actions = 0;
uint32_t interval_pilot = 0;
gps_coord_t dist;

void setup() {
	Serial.begin(115200);
	Serial.println("Arduino online!");
	Serial.println();

	pinMode(2, INPUT);

	nav.init();

	pilot.init();
	pilot.start();
#ifdef DEVEL
	pilot.set_speed(64 * 2);
#else
	pilot.set_speed(64 * 2);
#endif
#ifdef DEVEL
	// flags[0] = true;
#endif
}

#undef DEMO_CONTROL

void loop() {
	// us_scan_all();
	nav.navigate();
#ifdef DEVEL
	if (millis() - interval_nav > 1000) {
		nav.debug();
		pilot.debug();
		interval_nav = millis();
	}
	if (millis() - interval_us > 500) {
		// us_debug_all();
		interval_us = millis();
	}
#endif
	/*
	if (millis() - interval_nav > 1000) {
		dist = nav.delta_to(demo_coord);
		Serial.print("Latitude difference:  ");
		Serial.println(dist.latitude.i);
		Serial.print("Longitude difference: ");
		Serial.println(dist.longitude.i);
		Serial.print("Bearing to target: ");
		Serial.println(nav.angle_to(demo_heading));
		nav.instruct_pilot();
		nav.gps()->debug();
		// nav.mag()->debug();
		interval_nav = millis();
	}
	*/
	/*
#ifdef DEVEL
#ifdef DEMO_CONTROL
	//  This is a simple choreography script to demonstrate different pilot
	//  control methods.
	if (flags[0] && (millis() - interval_actions > 0)) {
		Serial.println("MOVING FORWARD");
		pilot.set_routine(move_forward);
		flags[0] = false;
		flags[1] = true;
	}
	else if (flags[1] && (millis() - interval_actions > 20000)) {
		Serial.println();
		Serial.println("TURNING LEFT");
		Serial.println();
		pilot.set_routine(turn_left);
		flags[1] = false;
		flags[2] = true;
	}
	else if (flags[2] && (millis() - interval_actions > 40000)) {
		Serial.println();
		Serial.println("MOVING FORWARD");
		Serial.println();
		pilot.set_routine(move_forward);
		flags[2] = false;
		flags[3] = true;
	}
	else if (flags[3] && (millis() - interval_actions > 60000)) {
		Serial.println();
		Serial.println("TURNING RIGHT");
		Serial.println();
		pilot.set_routine(turn_right);
		flags[3] = false;
		flags[4] = true;
	}
	else if (flags[4] && (millis() - interval_actions > 80000)) {
		interval_actions = millis();
		flags[0] = true;
		flags[1] = false;
		flags[2] = false;
		flags[3] = false;
		flags[4] = false;
	}
#endif//DEMO_CONTROL
#endif//DEVEL
	*/
}
