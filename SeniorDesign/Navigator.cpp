#include <Arduino.h>

#include <GPS.hpp>
#include <Magnetometer.hpp>

GPS gps(&Serial1);
Magnetometer compass;
double heading;
int16_t x, y, z;

gps_coord_t waypoints[] = {
	//  Fawick
	{
		.latitude  = { .i = +41379500 },
		.longitude = { .i = -85003420 }
	},
	//  Bend in sidewalk
	{
		.latitude  = { .i = +41379540 },
		.longitude = { .i = -85003420 }
	},
	//  Bend in sidewalk
	{
		.latitude  = { .i = +41379590 },
		.longitude = { .i = -85003430 },
	},
	//  90-degree turn for court
	{
		.latitude  = { .i = +41379600 },
		.longitude = { .i = -85003430 },
	},
	//  Firepit waypoint
	{
		.latitude  = { .i = +41379600 },
		.longitude = { .i = -85003530 },
	},
	//  End of court
	{
		.latitude  = { .i = +41379610 },
		.longitude = { .i = -85003650 },
	},
	//  Main sidewalk terminus
	{
		.latitude  = { .i = +41379655 },
		.longitude = { .i = -85003710 },
	},
	//  Waypoint on the main sidewalk
	{
		.latitude  = { .i = +41379656 },
		.longitude = { .i = -85003818 },
	},
	//  Waypoint on the main sidewalk
	{
		.latitude  = { .i = +41379657 },
		.longitude = { .i = -85004025 },
	},
	//  Intersection with the Bock sidewalk
	{
		.latitude  = { .i = +41379658 },
		.longitude = { .i = -85004175 },
	},
	//  Railroad tracks (DO NOT STOP HERE)
	{
		.latitude  = { .i = +41379660 },
		.longitude = { .i = -85004260 },
	},
	//  Last waypoint on the sidewalk
	{
		.latitude  = { .i = +41379675 },
		.longitude = { .i = -85004380 },
	},
	//  University Center
	{
		.latitude  = { .i = +41379770 },
		.longitude = { .i = -85004520 },
	},
};

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
