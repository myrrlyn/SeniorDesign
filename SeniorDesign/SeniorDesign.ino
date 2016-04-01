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

void setup() {
	Serial.begin(115200);
	Serial.println("Arduino online");
	compass.begin();
	for (uint8_t idx = 0; idx < 12; ++idx) {
		Serial.println((uint32_t)us_all[idx]);
	}
}

double heading;

void loop() {
	us_scan_all();
	Serial.print("Compass: ");
	compass.compass(&heading);
	Serial.println(heading);
	delay(1000);
}
