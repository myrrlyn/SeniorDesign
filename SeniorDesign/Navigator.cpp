#include "Navigator.hpp"
#include "Pilot.hpp"

#include <math.h>

#define NAV_WAYPOINT_PRECISION 30.0

gps_coord_t coords[] = {
	//   0 - University Center
	{
		.latitude  = { .i = +41379700 },
		.longitude = { .i = -85004493 },
	},
	//   1 - Last waypoint on the sidewalk
	{
		.latitude  = { .i = +41379684 },
		.longitude = { .i = -85004350 },
	},
	//   2 - Railroad tracks (DO NOT STOP HERE)
	{
		.latitude  = { .i = +41379675 },
		.longitude = { .i = -85004287 },
	},
	//   3 - Intersection with the Bock sidewalk
	{
		.latitude  = { .i = +41379631 },
		.longitude = { .i = -85004181 },
	},
	//   4 - Intersection with Best Hall sidewalk
	{
		.latitude  = { .i = +41379630 },
		.longitude = { .i = -85004037 },
	},
	//   5 - Bock sidewalk again
	{
		.latitude  = { .i = +41379628 },
		.longitude = { .i = -85003880 },
	},
	//   6 - Lamppost on the main sidewalk
	{
		.latitude  = { .i = +41379640 },
		.longitude = { .i = -85003918 },
	},
	//   7 - Waypoint on the main sidewalk
	{
		.latitude  = { .i = +41379634 },
		.longitude = { .i = -85003806 },
	},
	//   8 - Best Hall sidewalk junction
	{
		.latitude  = { .i = +41379640 },
		.longitude = { .i = -85003762 },
	},
	//   9 - Main sidewalk terminus
	{
		.latitude  = { .i = +41379640 },
		.longitude = { .i = -85003706 },
	},
	//  10 - Midpoint on diagonal
	{
		.latitude  = { .i = +41379610 },
		.longitude = { .i = -85003662 },
	},
	//  11 - End of court
	{
		.latitude  = { .i = +41379590 },
		.longitude = { .i = -85003590 },
	},
	//  12 - Firepit waypoint
	{
		.latitude  = { .i = +41379600 },
		.longitude = { .i = -85003512 },
	},
	//  13 - 90-degree turn for court
	//  --------------------------------
	//  NOTE: THIS IS POTENTIAL ENDPOINT
	//  --------------------------------
	{
		.latitude  = { .i = +41379612 },
		.longitude = { .i = -85003425 },
	},
/*
	//  14 - Bend in sidewalk
	{
		.latitude  = { .i = +41379590 },
		.longitude = { .i = -85003430 },
	},
	//  15 - Bend in sidewalk
	{
		.latitude  = { .i = +41379540 },
		.longitude = { .i = -85003420 },
	},
	//  16 - Fawick
	{
		.latitude  = { .i = +41379500 },
		.longitude = { .i = -85003420 },
	},
*/
};

nav_waypoint_t path[] = {
	//   0 - Arrive at the UC from anywhere
	//  This coordinate is selected on first powerup. It will not be selected
	//  during runtime.
	{
		.coord = &coords[0],
		.inbound = ANY,
		.outbound = East,
		.current_segment = NOT_YET_ON_ROUTE,
		.next_segment = uc_fawick_east,
	},
	//   1 - Arrive at the sidewalk choke from UC
	{
		.coord = &coords[1],
		.inbound = East,
		.outbound = East,
		.current_segment = uc_fawick_east,
		.next_segment = uc_fawick_east,
	},
	//   2 - Arrive at the railroad crossing from the sidewalk choke
	//  PLEASE TRY NOT TO STOP HERE
	{
		.coord = &coords[2],
		.inbound = East,
		.outbound = East,
		.current_segment = uc_fawick_east,
		.next_segment = uc_fawick_east,
	},
	//   3 - Arrive at the Bock branch from the railroad crossing
	{
		.coord = &coords[3],
		.inbound = East,
		.outbound = East,
		.current_segment = uc_fawick_east,
		.next_segment = uc_fawick_east,
	},
	//   4 - Best Hall sidewalk
	{
		.coord = &coords[4],
		.inbound = East,
		.outbound = East,
		.current_segment = uc_fawick_east,
		.next_segment = uc_fawick_east,
	},
	//   5 - Another Bock sidewalk
	{
		.coord = &coords[5],
		.inbound = East,
		.outbound = East,
		.current_segment = uc_fawick_east,
		.next_segment = uc_fawick_east,
	},
	//   6 - Lamppost
	{
		.coord = &coords[6],
		.inbound = East,
		.outbound = East,
		.current_segment = uc_fawick_east,
		.next_segment = uc_fawick_east,
	},
	//   7 - Midway along main run
	{
		.coord = &coords[7],
		.inbound = East,
		.outbound = East,
		.current_segment = uc_fawick_east,
		.next_segment = uc_fawick_east,
	},
	//   8 - Another Best Hall sidewalk
	{
		.coord = &coords[8],
		.inbound = East,
		.outbound = East,
		.current_segment = uc_fawick_east,
		.next_segment = uc_fawick_east,
	},
	//   9 - End of main sidewalk run; time to head SouthEast
	{
		.coord = &coords[9],
		.inbound = East,
		.outbound = SouthEast,
		.current_segment = uc_fawick_east,
		.next_segment = uc_fawick_east,
	},
	//  10 - Midpoint on the SouthEast run (this coordinate should probably be
	//  tweaked)
	{
		.coord = &coords[10],
		.inbound = SouthEast,
		.outbound = SouthEast,
		.current_segment = uc_fawick_east,
		.next_segment = uc_fawick_east,
	},
	//  11 - Bottom of the Centennial Courtyard
	{
		.coord = &coords[11],
		.inbound = SouthEast,
		.outbound = East,
		.current_segment = uc_fawick_east,
		.next_segment = uc_fawick_east,
	},
	//  12 - East to Firepit
	{
		.coord = &coords[12],
		.inbound = East,
		.outbound = East,
		.current_segment = uc_fawick_east,
		.next_segment = uc_fawick_east,
	},
	//  13 - Fawick Terminus (fuck that pivot-right-and-sidewalk bullshit)
	{
		.coord = &coords[13],
		.inbound = East,
		.outbound = West,
		.current_segment = uc_fawick_east,
		.next_segment = fawick_uc_west,
	},
	//  14 - West to Firepit
	//  ------------------------------------------------------------------
	//  NOTE: WE ARE NOT MAKING THE TURN TO GO SOUTH TO FAWICK DOOR
	//  THAT IS AN UNTENABLE NAVIGATION CHOICE AND WILL CAUSE ALL SORTS OF
	//  BAD BAD JUJU
	//  ------------------------------------------------------------------
	//  NOTE: AT THIS TIME, NAVIGATION SHOULD PAUSE AND AWAIT THE SELECTOR BEING
	//  THROWN TO 'UC'
	//  ------------------------------------------------------------------------
	{
		.coord = &coords[12],
		.inbound = West,
		.outbound = West,
		.current_segment = fawick_uc_west,
		.next_segment = fawick_uc_west,
	},
	//  15 - Reach the end of the court from the firepit
	{
		.coord = &coords[11],
		.inbound = West,
		.outbound = NorthWest,
		.current_segment = fawick_uc_west,
		.next_segment = fawick_uc_west,
	},
	//  16 - Diagonal midpoint
	{
		.coord = &coords[10],
		.inbound = NorthWest,
		.outbound = NorthWest,
		.current_segment = fawick_uc_west,
		.next_segment = fawick_uc_west,
	},
	//  17 - Reach the main sidewalk
	{
		.coord = &coords[9],
		.inbound = NorthWest,
		.outbound = West,
		.current_segment = fawick_uc_west,
		.next_segment = fawick_uc_west,
	},
	//  18 - Best Hall sidewalk
	{
		.coord = &coords[8],
		.inbound = West,
		.outbound = West,
		.current_segment = fawick_uc_west,
		.next_segment = fawick_uc_west,
	},
	//  19
	{
		.coord = &coords[7],
		.inbound = West,
		.outbound = West,
		.current_segment = fawick_uc_west,
		.next_segment = fawick_uc_west,
	},
	//  20 - Lamppost
	{
		.coord = &coords[6],
		.inbound = West,
		.outbound = West,
		.current_segment = fawick_uc_west,
		.next_segment = fawick_uc_west,
	},
	//  21 - Bock sidewalk
	{
		.coord = &coords[5],
		.inbound = West,
		.outbound = West,
		.current_segment = fawick_uc_west,
		.next_segment = fawick_uc_west,
	},
	//  22 - Another Best Hall sidewalk
	{
		.coord = &coords[4],
		.inbound = West,
		.outbound = West,
		.current_segment = fawick_uc_west,
		.next_segment = fawick_uc_west,
	},
	//  23 - Another Bock sidewalk
	{
		.coord = &coords[3],
		.inbound = West,
		.outbound = West,
		.current_segment = fawick_uc_west,
		.next_segment = fawick_uc_west,
	},
	//  24 - Railroad Tracks (still not a stop)
	{
		.coord = &coords[2],
		.inbound = West,
		.outbound = West,
		.current_segment = fawick_uc_west,
		.next_segment = fawick_uc_west,
	},
	//  25 - Sidewalk waypoint
	{
		.coord = &coords[1],
		.inbound = West,
		.outbound = West,
		.current_segment = fawick_uc_west,
		.next_segment = fawick_uc_west,
	},
	//  26
	//  ------------------------------------------------------------------------
	//  NOTE: AT THIS TIME, NAVIGATION SHOULD PAUSE UNTIL THE SELECTOR SWITCH IS
	//  THROWN TO 'FAWICK'
	//  ------------------------------------------------------------------------
	{
		.coord = &coords[0],
		.inbound = West,
		.outbound = East,
		.current_segment = fawick_uc_west,
		.next_segment = uc_fawick_east,
	},
};

Navigator::Navigator() :
	_gps(&Serial1),
	_mag(),
	_pin{4} {
	_state = &path[0];
	_index = 0;
	_heading = ANY;
	am_pivoting = false;
}

void Navigator::init() {
	_gps.begin(9600);
	_gps.command(GPS_OUTPUT_RMCGGA);
	_gps.command(GPS_FIX_UPDATE_200);
	_gps.command(GPS_PRINT_200);
	_mag.begin();
	_mag.set_gain(mag_gain_1090);
}

void Navigator::navigate() {
#ifndef DEVEL
	//  If we don't have a valid fix, don't do anything.
	if (_gps.fix_info() == gps_fix_invalid) {
		pilot.halt();
		return;
	}
#endif

	//  Step 0: Determine what the switch says is the target.
	_pin_reading = (bool)digitalRead(_pin);
	//  Step 1: Determine where we are.
	_loc_now = _gps.location();
	_dir_now = _gps.velocity().heading;
	//  Step 2: Determine if we have reached where we are going.
	bool arrived = approximately_at(path[_index].coord);
	//  Emergency condition: if we are still targeting the start, do nothing.
	//  Make the humans work to set this up properly.
	if (!arrived && path[_index].current_segment == NOT_YET_ON_ROUTE) {
		return;
	}
	//  Emergency condition: if we are at the terminus to which the selector is
	//  set, DO NOTHING
	//  If switch says FAWICK and we're at FAWICK, or if switch says UC and
	//  we're at UC, halt.
	if ((_pin_reading && (_index == 11))
	|| (!_pin_reading && (_index == 0))) {
		pilot.halt();
		return;
	}
	else {
		pilot.start();
	}
	//  Step 3: If we have reached the current goal, set a new goal.
	if (arrived) {
		set_next_target();
		_heading = _state->outbound;
	}
	//  Step 4: Figure out how to approach the goal
	//  Step 4.1: Figure out the goal's bearing from present location
	nav_direction_t bearing = approximate_bearing(_state->coord);
	//  Step 4.2: If the goal is clockwise of nominal direciton, bank right.
	//  If the goal is counterclockwise of nominal direction, bank left.
	//  If the goal is within the arc of nominal direction, go forward.
	//
	//  Oh, and, by the way, first check if we're at a terminus and ready to
	//  depart, in which case we must PIVOT (not BANK) for a set amount of time
	if (_pin_reading && (_index == 0)) {
#ifdef DEVEL
		Serial.println("PIVOTING TO FACE FAWICK");
#endif
		if (am_pivoting) {
			pilot.set_routine(pivot_left);
			//  PWM works even during delay
			//  Tweak this magic number as we go
			delay(5000);
			pilot.set_routine(move_forward);
			am_pivoting = false;
		}
	}
	else if (!_pin_reading && (_index == 11)) {
#ifdef DEVEL
		Serial.println("PIVOTING TO FACE UC");
#endif
		if (am_pivoting) {
			pilot.set_routine(pivot_left);
			delay(5000);
			pilot.set_routine(move_forward);
			am_pivoting = false;
		}
	}
	else if (_pin_reading && (_index == 4)) {
		pilot.set_routine(bank_left);
		delay(1000);
	}
	else if (path[_index].outbound == bearing) {
		pilot.set_routine(move_forward);
	}
	else if ((path[_index].outbound < bearing)
	|| (path[_index].outbound == NorthWest
	&&  bearing == North)) {
		pilot.set_routine(bank_right);
	}
	else if ((path[_index].outbound > bearing)
	|| (path[_index].outbound == North
	&&  bearing == NorthWest)) {
		pilot.set_routine(bank_left);
	}
}

void Navigator::instruct_pilot() {

}

bool Navigator::approximately_at(gps_coord_t* goal) {
	return approximate_range(goal) < 50.0;
}

float Navigator::approximate_range(gps_coord_t* target) {
	gps_coord_t delta;
	delta.latitude.f = (float)(target->latitude.i - _loc_now.latitude.i);
	delta.longitude.f = (float)(target->longitude.i - _loc_now.longitude.i);
	float dist = (delta.latitude.f * delta.latitude.f)
	           + (delta.longitude.f * delta.longitude.f);
	return sqrt(dist);
}

nav_direction_t Navigator::approximate_bearing(gps_coord_t* goal) {
	gps_coord_t tmp;
	tmp.latitude.f  = (float)(goal->latitude.i  - _loc_now.latitude.i);
	tmp.longitude.f = (float)(goal->longitude.i - _loc_now.longitude.i);
	//  tmp now stores the vector from _loc_now -->> goal
	//  Find the navigational angle of that vector
	float angle = atan2(tmp.longitude.f, tmp.latitude.f);
	//  atan2 returns between -pi and +pi. Check return values against
	//  direction borders (in radians).
	if (angle < (-150.0 * M_PI / 180.0)) {
		return South;
	}
	if (angle < (-120.0 * M_PI / 180.0)) {
		return SouthWest;
	}
	if (angle < (-60.0 * M_PI / 180.0)) {
		return West;
	}
	if (angle < (-30.0 * M_PI / 180.0)) {
		return NorthWest;
	}
	if (angle < (30.0 * M_PI / 180.0)) {
		return North;
	}
	if (angle < (60.0 * M_PI / 180.0)) {
		return NorthEast;
	}
	if (angle < (120.0 * M_PI / 180.0)) {
		return East;
	}
	if (angle < (150.0 * M_PI / 180.0)) {
		return SouthEast;
	}
	return South;
}

void Navigator::set_next_target() {
	if ((_index == 19)
	||  (_index == 0)) {
		_index = 1;
	}
	else {
		++_index;
	}
	if (_index == 11 || _index == 19) {
		am_pivoting = true;
	}
	_state = &path[_index];
}

GPS* Navigator::gps() {
	return &_gps;
}

Magnetometer* Navigator::mag() {
	return &_mag;
}

#ifdef DEVEL
void Navigator::debug() {
	Serial.println("NAVIGATOR");
	_gps.debug();
	Serial.println("TARGETING");
	Serial.print("Waypoint index: ");
	Serial.println(_index);
	Serial.print("Switch: ");
	Serial.println(_pin_reading ? "FAWICK" : "UC");
	Serial.println("RANGE AND BEARING");
	Serial.println(approximate_range(path[_index].coord));
	Serial.println(approximate_bearing(path[_index].coord));
	Serial.println("True offset:");
	Serial.println(path[_index].coord->latitude.i - _loc_now.latitude.i);
	Serial.println(path[_index].coord->longitude.i - _loc_now.longitude.i);
	Serial.println();
}
#endif

Navigator nav;

//  Library-provided callback attached to Serial1 RX
void serialEvent1() {
	gps_err_t err = nav.gps()->store_stream();
	//  This isn't an interrupt function, it's a callback executed in the
	//  foreground of the main-while loop on the same level as loop()
	//  As such, we are not really under a time constraint here vs doing
	//  the parsing in loop(). Separation of concerns thus says to keep this
	//  in here, out of loop()'s view.
	if (err == gps_msg_ready) {
#ifdef DEVEL
		// Serial.println(nav.gps()->latest_sentence());
#endif
		nav.gps()->parse(nav.gps()->latest_sentence());
	}
}
