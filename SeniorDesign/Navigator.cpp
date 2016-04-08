#include <Arduino.h>

#include <GPS.hpp>
#include <Magnetometer.hpp>

GPS gps(&Serial1);
Magnetometer compass;

//  Library-provided callback attached to Serial1 RX
void serialEvent1() {
	gps.store(gps.read());
	if (gps.sentence_ready()) {
		// Serial.println(gps.latest_sentence());
		gps.parse(gps.latest_sentence());
	}
}
