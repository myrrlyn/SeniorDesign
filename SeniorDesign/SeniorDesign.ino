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

	OCR0A = 0xAF;
	TIMSK0 |= _BV(OCIE0A);

	set_speed(0);

	start();

	delay(500);

	interval = millis();
	set_speed(127);
}

void loop() {
	us_scan_head();
	debug_motors();
	if (millis() - interval > 1000) {
		gps.debug();
		debug_mag();
		interval = millis();
	}
}
