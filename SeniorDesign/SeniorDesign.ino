/* === SeniorDesign ===
 * Alexander Payne
 *
 * This is the root file of the program. The Arduino framework constructs a
 * main() function that will initialize the board, call setup() once, and call
 * loop() forever.
 */

#include <Utility.hpp>

#include "Lookout.hpp"

void setup() {
	Serial.begin(115200);
	Serial.println("Arduino online");
	for (uint8_t idx = 0; idx < 12; ++idx) {
		Serial.println((uint32_t)us_all[idx]);
	}
}

void loop() {
	us_scan_all();
	delay(1000);
}
