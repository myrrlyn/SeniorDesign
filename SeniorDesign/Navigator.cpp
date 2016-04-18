#include <Arduino.h>

#include "Navigator.hpp"
#include "Pilot.hpp"

gps_coord_t waypoints[] = {
	//  0 - Fawick
	{
		.latitude  = { .i = +41379500 },
		.longitude = { .i = -85003420 }
	},
	//  1 - Bend in sidewalk
	{
		.latitude  = { .i = +41379540 },
		.longitude = { .i = -85003420 }
	},
	//  2 - Bend in sidewalk
	{
		.latitude  = { .i = +41379590 },
		.longitude = { .i = -85003430 },
	},
	//  3 - 90-degree turn for court
	{
		.latitude  = { .i = +41379600 },
		.longitude = { .i = -85003430 },
	},
	//  4 - Firepit waypoint
	{
		.latitude  = { .i = +41379600 },
		.longitude = { .i = -85003530 },
	},
	//  5 - End of court
	{
		.latitude  = { .i = +41379610 },
		.longitude = { .i = -85003650 },
	},
	//  6 - Main sidewalk terminus
	{
		.latitude  = { .i = +41379655 },
		.longitude = { .i = -85003710 },
	},
	//  7 - Waypoint on the main sidewalk
	{
		.latitude  = { .i = +41379656 },
		.longitude = { .i = -85003818 },
	},
	//  8 - Waypoint on the main sidewalk
	{
		.latitude  = { .i = +41379657 },
		.longitude = { .i = -85004025 },
	},
	//  9 - Intersection with the Bock sidewalk
	{
		.latitude  = { .i = +41379658 },
		.longitude = { .i = -85004175 },
	},
	//  10 - Railroad tracks (DO NOT STOP HERE)
	{
		.latitude  = { .i = +41379660 },
		.longitude = { .i = -85004260 },
	},
	//  11 - Last waypoint on the sidewalk
	{
		.latitude  = { .i = +41379675 },
		.longitude = { .i = -85004380 },
	},
	//  12 - University Center
	{
		.latitude  = { .i = +41379770 },
		.longitude = { .i = -85004520 },
	},
};

Navigator::Navigator() :
	_gps(&Serial1),
	_mag() {
	going_east = true;
	at_endpoint = true;
	set_target(waypoints[12]);
}

gps_coord_t Navigator::delta_to(gps_coord_t target) {
	gps_coord_t tmp;
	gps_coord_t self = _gps.location();
	tmp.latitude.i = target.latitude.i - self.latitude.i;
	tmp.longitude.i = target.longitude.i - self.longitude.i;
	return tmp;
}

double Navigator::absolute_distance_to(gps_coord_t target) {
	gps_coord_t delta = delta_to(target);
	delta.latitude.i *= delta.latitude.i;
	delta.longitude.i *= delta.longitude.i;
	double dist = delta.latitude.i + delta.longitude.i;
	return sqrt(dist);
}

double Navigator::angle_to(double target) {
	double tmp = _gps.velocity().heading;
	tmp = target - tmp;
	if (tmp < -180.0) {
		tmp += 360.0;
	}
	return tmp;
}

double Navigator::bearing_to(gps_coord_t target) {
	gps_coord_t delta = delta_to(target);
	return atan2(delta.longitude.i, delta.latitude.i);
}

nav_angle_t Navigator::approximate_heading() {
	double tmp;
	mag_err_t magerr;
	nav_angle_t ret;
	do {
		magerr = _mag.compass(&tmp);
	} while (magerr != mag_err_none);
	if ((tmp >= 330.0 && tmp < 360.0) || (tmp >= 0.0 && tmp < 30.0)) {
		ret = North;
	}
	else if (tmp >= 30.0 && tmp < 60.0) {
		ret = NorthEast;
	}
	else if (tmp >= 60.0 && tmp < 120.0) {
		ret = East;
	}
	else if (tmp >= 120.0 && tmp < 150.0) {
		ret = SouthEast;
	}
	else if (tmp >= 150.0 && tmp < 210.0) {
		ret = South;
	}
	else if (tmp >= 210.0 && tmp < 240.0) {
		ret = SouthWest;
	}
	else if (tmp >= 240.0 && tmp < 300.0) {
		ret = West;
	}
	else if (tmp >= 300.0 && tmp < 330.0) {
		ret = NorthWest;
	}
	return ret;
}

bool Navigator::location_is_approximately(gps_coord_t target) {
	gps_coord_t tmp = _gps.location();
	gps_coord_t ret;
	ret.latitude.i = tmp.latitude.i - target.latitude.i;
	ret.longitude.i = tmp.longitude.i - target.longitude.i;

	Serial.println(ret.latitude.i);
	Serial.println(ret.longitude.i);

	if (ret.latitude.i < 0) {
		ret.latitude.i *= -1;
	}
	if (ret.longitude.i < 0) {
		ret.longitude.i *= -1;
	}
	return (ret.latitude.i < 25 && ret.longitude.i < 25);
}

void Navigator::set_target(gps_coord_t target) {
	_target_destination = target;
	_target_heading = bearing_to(target);
}

gps_coord_t Navigator::get_target() {
	return _target_destination;
}

void Navigator::select_target_from_set() {
	if (going_east) {
		if (at_endpoint) {
			current_target_index = 11;
			at_endpoint = false;
		}
		else {
			--current_target_index;
		}
		if (location_is_approximately(waypoints[3])) {
			at_endpoint = true;
			going_east = false;
		}
	}
	else {
		if (at_endpoint) {
			current_target_index = 4;
			at_endpoint = false;
		}
		else {
			++current_target_index;
		}
		if (location_is_approximately(waypoints[12])) {
			at_endpoint = true;
			going_east = true;
		}
	}
	Serial.print("CURRENT TARGET INDEX: ");
	Serial.println(current_target_index);
	set_target(waypoints[current_target_index]);
}

void Navigator::instruct_pilot() {
	double dist = absolute_distance_to(_target_destination);
	Serial.print("Distance: ");
	Serial.println(dist);
	Serial.print("Bearing: ");
	double turn = angle_to(_target_heading);
	Serial.println(turn);
	Serial.print("Maneouvre: ");
	if (dist < 250.0) {
		pilot.set_routine(all_stop);
		Serial.println("STOPPING");
	}
	/*
	else if (turn < -10 && turn > -50) {
		pilot.set_routine(turn_left);
		Serial.println("Turning left");
	}
	else if (turn > 10 && turn < 50) {
		pilot.set_routine(turn_right);
		Serial.println("Turning right");
	}*/
	else {
		pilot.set_routine(move_forward);
		Serial.println("Moving forward");
	}
}

GPS* Navigator::gps() {
	return &_gps;
}

Magnetometer* Navigator::mag() {
	return &_mag;
}

Navigator nav;

//  Library-provided callback attached to Serial1 RX
void serialEvent1() {
	gps_err_t err;
	//  Completely drain the GPS stream -- we cannot depend on loop() to let
	//  this run often enough to only read once per cycle
	// while (nav.gps()->available()) {
		//  A single read-and-store function would probably be better...
		err = nav.gps()->store_stream();
		//  This isn't an interrupt function, it's a callback executed in the
		//  foreground of the main-while loop on the same level as loop()
		//  As such, we are not really under a time constraint here vs doing
		//  the parsing in loop(). Separation of concerns thus says to keep this
		//  in here, out of loop()'s view.
		if (err == gps_msg_ready) {
			// Serial.println(gps.latest_sentence());
			nav.gps()->parse(nav.gps()->latest_sentence());
		}
	// }
}
