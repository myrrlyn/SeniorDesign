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
	Serial.println("Arduino online!");
	Serial.println();

	pinMode(2, INPUT);

	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

	digitalWrite(10, HIGH);
	digitalWrite(11, HIGH);

	gps.begin(9600);

	gps.command(GPS_OUTPUT_RMCGGA);
	gps.command(GPS_FIX_UPDATE_1000);
	gps.command(GPS_PRINT_2000);

	delay(500);
}

void loop() {
	gps.store(gps.read());
	if (gps.sentence_ready()) {
		Serial.println(gps.latest_sentence());
		gps.parse((char*)gps.latest_sentence());
		gps.debug();
	}
}
