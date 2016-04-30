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

uint32_t interval_debug = 0;
uint32_t interval_actions = 0;
uint8_t demo_stage = 0;

void setup() {
	Serial.begin(115200);
	Serial.println("Arduino online!");
	Serial.println();

	pinMode(2, INPUT);

	nav.init();

	pilot.init();
	pilot.start();
	pilot.set_speed(255);
}

#define DEMO_CONTROL

void loop() {
/*
	us_scan_head();
	nav.navigate();
#ifdef DEVEL
	if (millis() - interval_debug > 1000) {
		us_debug_all();
		nav.debug();
		pilot.debug();
		interval_debug = millis();
	}
#endif
*/
#ifdef DEVEL
#ifdef DEMO_CONTROL
	us_scan_head();
	if (millis() - interval_debug > 1000) {
		Serial.println();
		us_debug_all();
		pilot.debug();
		interval_debug = millis();
	}
	//  This is a simple choreography script to demonstrate different pilot
	//  control methods.
	if (millis() - interval_actions > 20000) {
		Serial.println();
		switch (demo_stage) {
			case 0:
				Serial.println("MOVING FORWARD");
				pilot.set_routine(move_forward);
				break;
			case 1:
				Serial.println("BANKING LEFT...");
				pilot.set_routine(bank_left);
				break;
			case 2:
				Serial.println("MOVING FORWARD");
				pilot.set_routine(ahead_full);
				break;
			case 3:
				Serial.println("BANKING RIGHT...");
				pilot.set_routine(bank_left);
				break;
			case 4:
				Serial.println("MOVING FORWARD");
				pilot.set_routine(ahead_full);
				break;
			case 5:
				Serial.println("PIVOTING LEFT...");
				pilot.set_routine(pivot_left);
				break;
			case 6:
				Serial.println("ACCELERATING FORWARD");
				pilot.set_routine(move_forward);
				break;
			case 7:
				Serial.println("PIVOTING RIGHT...");
				pilot.set_routine(pivot_right);
				break;
			case 8:
				Serial.println("HALTING");
				pilot.set_routine(all_stop);
				break;
		}
		Serial.println();
		++demo_stage;
		demo_stage %= 9;
		interval_actions = millis();
	}
#endif//DEMO_CONTROL
#endif//DEVEL
}
