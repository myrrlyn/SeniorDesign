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

uint32_t  interval;

void setup() {
	Serial.begin(115200);
	Serial.println("Arduino online!");
	Serial.println();

	pinMode(2, INPUT);

	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

	digitalWrite(10, HIGH);
	digitalWrite(11, HIGH);

	gps.begin(9600);


	// gps.command(GPS_COMM_BAUD_57600);
	// gps.begin(57600);

	gps.command(GPS_OUTPUT_ALL);
	gps.command(GPS_FIX_UPDATE_200);
	gps.command(GPS_PRINT_200);

	delay(500);

	interval = millis();
}

void loop() {
	if (millis() - interval > 1000) {
		gps.debug();
		interval = millis();
	}
}
