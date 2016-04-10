#include <Arduino.h>

#include <GPS.hpp>
#include <Magnetometer.hpp>

GPS gps(&Serial1);
Magnetometer compass;
double heading;
int16_t x, y, z;

void debug_mag() {
	Serial.print("RAW: ");
	compass.read_raw(&x, &y, &z);
	Serial.print(x);
	Serial.print(' ');
	Serial.print(y);
	Serial.print(' ');
	Serial.println(z);
	compass.compass(&heading, mag_axes_xy);
	Serial.print("XY: ");
	Serial.println(heading);
	compass.compass(&heading, mag_axes_xz);
	Serial.print("XZ: ");
	Serial.println(heading);
	compass.compass(&heading, mag_axes_yz);
	Serial.print("YZ: ");
	Serial.println(heading);
	compass.compass(&heading, mag_axes_yx);
	Serial.print("YX: ");
	Serial.println(heading);
	compass.compass(&heading, mag_axes_zx);
	Serial.print("ZX: ");
	Serial.println(heading);
	compass.compass(&heading, mag_axes_zy);
	Serial.print("ZY: ");
	Serial.println(heading);
	Serial.println();
}

//  Library-provided callback attached to Serial1 RX
void serialEvent1() {
	//  Completely drain the GPS stream -- we cannot depend on loop() to let
	//  this run often enough to only read once per cycle
	while (gps.available()) {
		//  A single read-and-store function would probably be better...
		gps.store(gps.read());
		//  This isn't an interrupt function, it's a callback executed in the
		//  foreground of the main-while loop on the same level as loop()
		//  As such, we are not really under a time constraint here vs doing
		//  the parsing in loop(). Separation of concerns thus says to keep this
		//  in here, out of loop()'s view.
		if (gps.sentence_ready()) {
			// Serial.println(gps.latest_sentence());
			gps.parse(gps.latest_sentence());
		}
	}
}
