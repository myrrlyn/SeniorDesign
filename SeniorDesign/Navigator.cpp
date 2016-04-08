#include <Arduino.h>

#include <GPS.hpp>
#include <Magnetometer.hpp>

GPS gps(&Serial1);
Magnetometer compass;
double heading;

void debug_mag() {
	compass.compass(&heading, mag_axes_xy);
	Serial.println(heading);
	compass.compass(&heading, mag_axes_xz);
	Serial.println(heading);
	compass.compass(&heading, mag_axes_yz);
	Serial.println(heading);
	compass.compass(&heading, mag_axes_yx);
	Serial.println(heading);
	compass.compass(&heading, mag_axes_zx);
	Serial.println(heading);
	compass.compass(&heading, mag_axes_zy);
	Serial.println(heading);
}

//  Library-provided callback attached to Serial1 RX
void serialEvent1() {
	gps.store(gps.read());
	if (gps.sentence_ready()) {
		// Serial.println(gps.latest_sentence());
		gps.parse(gps.latest_sentence());
	}
}
