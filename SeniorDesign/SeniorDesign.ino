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

gps_coord_t active_target = {
	.latitude  = { .i = +41379600 },
	.longitude = { .i = -85003530 },
};

gps_coord_t demo_coord = {
	.latitude  = { .i = 41379015 },
	.longitude = { .i = -85004300 },
};

void setup() {
	Serial.begin(115200);
	Serial.println("Arduino online!");
	Serial.println();

	pinMode(2, INPUT);

	nav.gps()->begin(9600);
	nav.mag()->begin();
	nav.mag()->set_gain(mag_gain_1090);

	nav.gps()->command(GPS_OUTPUT_ALL);
	nav.gps()->command(GPS_FIX_UPDATE_200);
	nav.gps()->command(GPS_PRINT_200);

	nav.set_target(demo_coord);

	pilot.init();
	pilot.start();
	pilot.set_speed(64);
	pilot.set_routine(move_forward);
}

bool flags[] = {
	false,
	false,
	false,
	false,
	false,
};

double demo_heading = 90.00;

uint32_t interval_us = 0;
uint32_t interval_nav = 0;
uint32_t interval_actions = 0;
gps_coord_t dist;

void loop() {
	/*
	us_scan_all();
	if (millis() - interval_us > 500) {
		// us_scan_all();
		us_debug_all();
		interval_us = millis();
	}
	*/
	// pilot.debug();
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
	//  This is a simple choreography script to demonstrate different pilot
	//  control methods.
	/*
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
		interval_actions = millis();
		flags[0] = true;
		flags[1] = false;
		flags[2] = false;
		flags[3] = false;
		flags[4] = false;
	}
	*/
}
