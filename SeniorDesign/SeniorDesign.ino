/* === SeniorDesign ===
 * Alexander Payne
 *
 * This is the root file of the program. The Arduino framework constructs a
 * main() function that will initialize the board, call setup() once, and call
 * loop() forever.
 */

void setup() {
	Serial.begin(115200);
	Serial.println("Arduino online");
}

void loop() {
	delay(1000);
}
