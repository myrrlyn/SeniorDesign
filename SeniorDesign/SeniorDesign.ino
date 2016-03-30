/* === SeniorDesign ===
 * Alexander Payne
 *
 * This is the root file of the program. The Arduino framework constructs a
 * main() function that will initialize the board, call setup() once, and call
 * loop() forever.
 */

#include <Utility.hpp>

void setup() {
	Serial.begin(115200);
	Serial.println("Arduino online");
	iopair_t comm = { .tx = 3, .rx = 2 };
	pinsMode(comm);
	digitalWrite(comm.tx, HIGH);
}

void loop() {
	delay(1000);
}
