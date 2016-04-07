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

	pinMode(2, INPUT);

	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

	digitalWrite(10, HIGH);
	digitalWrite(11, HIGH);
	delay(2500);
}


void loop() {
	Serial.print("Pin 2: ");
	Serial.println(digitalRead(2));
	us_scan_head();
	delay(1000);
}
