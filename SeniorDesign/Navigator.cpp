#include "Navigator.hpp"
#include "Pilot.hpp"

#include <math.h>

#define NAV_WAYPOINT_PRECISION 30.0

gps_coord_t coords[] = {
	//   0 - University Center
	{
		.latitude  = { .i = +41379680 },
		.longitude = { .i = -85004450 },
	},
	//   1 - Last waypoint on the sidewalk
	{
		.latitude  = { .i = +41379670 },
		.longitude = { .i = -85004375 },
	},
	//   2 - Railroad tracks (DO NOT STOP HERE)
	{
		.latitude  = { .i = +41379640 },
		.longitude = { .i = -85004290 },
	},
	//   3 - Intersection with the Bock sidewalk
	{
		.latitude  = { .i = +41379645 },
		.longitude = { .i = -85004160 },
	},
	//   4 - Intersection with Best Hall sidewalk
	{
		.latitude  = { .i = +41379655 },
		.longitude = { .i = -85004040 },
	},
	//   5 - Bock sidewalk again
	{
		.latitude  = { .i = +41379655 },
		.longitude = { .i = -85003980 },
	},
	//   6 - Lamppost on the main sidewalk
	{
		.latitude  = { .i = +41379650 },
		.longitude = { .i = -85003860 },
	},
	//   7 - Waypoint on the main sidewalk
	{
		.latitude  = { .i = +41379645 },
		.longitude = { .i = -85003780 },
	},
	//   8 - Best Hall sidewalk junction
	{
		.latitude  = { .i = +41379640 },
		.longitude = { .i = -85003720 },
	},
	//   9 - Main sidewalk terminus
	{
		.latitude  = { .i = +41379640 },
		.longitude = { .i = -85003650 },
	},
	//  10 - Midpoint on diagonal
	{
		.latitude  = { .i = +41379615 },
		.longitude = { .i = -85003600 },
	},
	//  11 - End of court
	{
		.latitude  = { .i = +41379580 },
		.longitude = { .i = -85003540 },
	},
	//  12 - Firepit waypoint
	{
		.latitude  = { .i = +41379595 },
		.longitude = { .i = -85003480 },
	},
	//  13 - 90-degree turn for court
	//  --------------------------------
	//  NOTE: THIS IS POTENTIAL ENDPOINT
	//  --------------------------------
	{
		.latitude  = { .i = +41379600 },
		.longitude = { .i = -85003400 },
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
	_gps(&Serial3),
	_mag(),
	_pin{4} {
	_index = 0;
	_heading = ANY;
	am_pivoting = false;
	needs_immediate_turn = false;
	gps_msg_ready = false;
}

void Navigator::init() {
	_gps.begin(9600);
	_gps.command(GPS_OUTPUT_RMCGGA);
	_gps.command(GPS_FIX_UPDATE_200);
	_gps.command(GPS_PRINT_200);
	_mag.begin();
	_mag.set_gain(mag_gain_1090);

	pinMode(7, INPUT);

	//  WGM 4 (0b0100)
	//  COM{ABC} 00
	TCCR4A  = 0x01;
	TCCR4B  = 0x03;
	TCCR4C  = 0x00;
	// ICR4    = ;
	OCR4A   = 0x1000;
	// OCR4B   = ;
	// OCR4C   = ;
	// TIMSK4 |= _BV(ICIE4);
	TIMSK4 |= _BV(OCIE4A);
	// TIMSK4 |= _BV(TOIE4);
}

bool manual_flag;

uint32_t nav_clock = 0;

void Navigator::navigate() {

	//  GPS sentences arrive every 200ms. There is absolutely no reason to run
	//  the navigator routine as often as possible, as this will just result in
	//  recomputation of the same data, giving the same result, and these
	//  computations are (a) expensive and (b) difficult to memoize in our
	//  constraints.
	//  This will abort the navigation routine if it has been run in the past
	//  100ms. The shorter window allows us to guarantee that a new GPS sentence
	//  will ALWAYS be parsed within a short time of its arrival.
	if (millis() - nav_clock < 100) {
		return;
	}

	nav_clock = millis();

	//  Housekeeping - parse the latest sentence
	if (gps_msg_ready) {
		_gps.parse(_gps.latest_sentence());
		gps_msg_ready = false;
	}

#ifndef DEVEL
	//  If we don't have a valid fix, don't do anything.
	if (_gps.fix_info() == gps_fix_invalid) {
		pilot.halt();
		return;
	}
#endif

	if (digitalRead(7) && !manual_flag) {
		manual_flag = true;
	}
	if (!digitalRead(7) && manual_flag) {
		manual_flag = false;
		set_next_target();
	}

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
	//  If we've returned to the UC and completed a trip, halt.
	if ((_pin_reading && (_index == 14))
	|| (!_pin_reading && (_index == 0))
	||  (_index == 26)) {
		pilot.halt();
		return;
	}
	//  The Lookout will permit us to start back up if the above is satisfied.
	//  Step 3: If we have reached the current goal, set a new goal.
	if (arrived) {
		set_next_target();
		_heading = path[_index].outbound;
	}
	//  Step 4: Figure out how to approach the goal
	instruct_pilot();
}

//  Finite State Machine dependent on current goal
void Navigator::instruct_pilot() {
	float heading = _gps.velocity().heading;
	float bearing = real_bearing(path[_index].coord);
	float angle_off = heading - bearing;
	//  Edge cases: a far CCW heading - a far CW bearing will cause massive CW
	//  angle_off values
	//  Example: (350.0) - (5.0) = 345 degree turn CW, when it should be 15 CW
	if (angle_off > 180.0) {
		angle_off = 360.0 - angle_off;
	}
	//  Edge cases: a far CW heading - a far CCW bearing will cause massive CCW
	//  angle_off values
	//  Example: (5.0) - (350.0) = 345 degree turn CCW
	gps_coord_t coord_off = real_range(path[_index].coord);
	float tmp;

	switch (_index) {
	//  This is only targeted while the robot is being moved into position.
	//  The robot will not assist during this time.
	case  0:
		pilot.halt();
		break;
	//  These are targeted during the course of normal operation.
	//  The robot will attempt to use its location and heading values to steer
	//  towards the point.

	//  FALLTHROUGHS ARE INTENTIONAL

	//  Heading east-ish from UC
	//  Cases 1 - 3 are drifting slightly southeast
	case  1:
	case  2:
	case  3:
		//  Permit more latitude (heh) in North-checking
		if ((coord_off.latitude.i < -30) || (angle_off > 10.0)) {
			pilot.set_routine(bank_right);
		}
		else if((coord_off.latitude.i > 20) || (angle_off < -10.0)) {
			pilot.set_routine(bank_left);
		}
		else {
			pilot.set_routine(ahead_full);
		}
		break;
	//  Cases 4 - 9 run straight east
	//  Case 4 requires bending port for a short time to execute the turn
	case  4:
		if (needs_immediate_turn) {
			pilot.set_routine(pivot_left);
			delay(500);
			pilot.set_routine(ahead_full);
			needs_immediate_turn = false;
		}
	//  Heading east along main sidewalk
	case  5:
	case  6:
	case  7:
	case  8:
	case  9:
		if ((coord_off.latitude.i < -20) || (angle_off > 10.0)) {
			pilot.set_routine(bank_right);
		}
		else if ((coord_off.latitude.i > 20) || (angle_off < -10.0)) {
			pilot.set_routine(bank_left);
		}
		else {
			pilot.set_routine(ahead_full);
		}
		break;
	//  Heading SouthEast
	//  Case 10 requires bending starboard for a short time to execute the turn
	case 10:
		if (needs_immediate_turn) {
			pilot.set_routine(pivot_right);
			delay(1000);
			pilot.set_routine(ahead_full);
			needs_immediate_turn = false;
		}
	case 11:
		//  Inspect bearing angles and lat/long RATIOS
		//  Both latitude AND longitude come into play here.
		//  We want to be North (negative ΔLat) && West (positive ΔLng).
		//  If we are South (postive ΔLat) ^ East (negative ΔLng), something is
		//  wrong and we can correct with only one check. If we are South &&
		//  East, something is EXTRA wrong, and we must immediately target the
		//  next point

		//  First up, check N && W
		if ((coord_off.latitude.i < 0) && (coord_off.longitude.i > 0)) {
			tmp = (float)(coord_off.latitude.i)
			    / (float)(coord_off.longitude.i);
			//  If we are NNW, turn starboard.
			if (tmp < -1.5) {
				pilot.set_routine(bank_right);
			}
			//  If we are WNW, turn port.
			else if (tmp > -0.67) {
				pilot.set_routine(bank_left);
			}
			//  If we are  NW, go straight.
			else {
				pilot.set_routine(ahead_full);
			}
		}
		//  Check N && E
		else if ((coord_off.latitude.i < 0) && (coord_off.longitude.i < 0)) {
			pilot.set_routine(bank_right);
		}
		//  Check S && W
		else if ((coord_off.latitude.i > 0) && (coord_off.longitude.i > 0)) {
			pilot.set_routine(bank_left);
		}
		//  Check S && E
		else if ((coord_off.latitude.i > 0) && (coord_off.longitude.i < 0)) {
			++_index;
			//  panic and go north (port)
			pilot.set_routine(bank_left);
		}
		break;
	//  Heading east in courtyard
	//  Case 12 requires bending port for a short time to execute the turn
	case 12:
		if (needs_immediate_turn) {
			pilot.set_routine(pivot_left);
			delay(1000);
			pilot.set_routine(ahead_full);
			needs_immediate_turn = false;
		}
	case 13:
		//  If too far North, OR aimed too far NorthEast, turn starboard
		if ((coord_off.latitude.i < -10) || (angle_off > 15.0)) {
			pilot.set_routine(bank_right);
		}
		//  If too far South, OR aimed too far SouthEast, turn port
		else if ((coord_off.latitude.i > 5) || (angle_off < -10.0)) {
			pilot.set_routine(bank_left);
		}
		else {
			pilot.set_routine(ahead_full);
		}
		break;
	//  PIVOT FIRST then head to firepit
	case 14:
		if (am_pivoting) {
			pilot.set_routine(pivot_left);
			delay(5000);
			pilot.set_routine(ahead_full);
			am_pivoting = false;
		}
	case 15:
		//  If too far North, OR aimed too far NorthWest, turn port
		if ((coord_off.latitude.i < -10) || (angle_off < -15.0)) {
			pilot.set_routine(bank_left);
		}
		//  If too far South, OR aimed too far SouthWest, turn starboard
		else if ((coord_off.latitude.i > 5) || (angle_off > 10.0)) {
			pilot.set_routine(bank_right);
		}
		else {
			pilot.set_routine(ahead_full);
		}
	//  Bank starboard (NorthWest)
	case 16:
		if (needs_immediate_turn) {
			pilot.set_routine(pivot_right);
			delay(1000);
			pilot.set_routine(ahead_full);
			needs_immediate_turn = false;
		}
	case 17:
		//  Inspect bearing angles and lat/long RATIOS
		//  Both latitude AND longitude come into play here.
		//  We want to be South (positive ΔLat) && East (negative ΔLng).
		//  If we are North (negative ΔLat) ^ West (positive ΔLng), something is
		//  wrong and we can correct with only one check. If we are North &&
		//  West, something is EXTRA wrong and we must immediately target the
		//  next point

		//  First up, check on S && E
		if ((coord_off.latitude.i > 0) && (coord_off.longitude.i < 0)) {
			tmp = (float)(coord_off.latitude.i)
			    / (float)(coord_off.longitude.i);
			//  If we are ESE, turn port.
			if (tmp > -0.67) {
				pilot.set_routine(bank_left);
			}
			//  If we are SSE, turn starboard.
			else if (tmp < -1.5) {
				pilot.set_routine(bank_right);
			}
			//  If we are  SE, go straight.
			else {
				pilot.set_routine(ahead_full);
			}
		}
		//  Check N && E
		else if ((coord_off.latitude.i < 0) && (coord_off.longitude.i < 0)) {
			pilot.set_routine(bank_left);
		}
		//  Check S && W
		else if ((coord_off.latitude.i > 0) && (coord_off.longitude.i > 0)) {
			pilot.set_routine(bank_right);
		}
		//  Check N && W
		else if ((coord_off.latitude.i < 0) && (coord_off.longitude.i > 0)) {
			++_index;
			//  panic and go north (starboard)
			pilot.set_routine(bank_right);
		}
		break;
	//  Bank port (West)
	case 18:
		if (needs_immediate_turn) {
			pilot.set_routine(pivot_left);
			delay(1000);
			pilot.set_routine(ahead_full);
			needs_immediate_turn = false;
		}
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
		//  Turn port if North or CW, starboard if South or CCW
		if ((coord_off.latitude.i < -10) || (angle_off < -15.0)) {
			pilot.set_routine(bank_left);
		}
		else if ((coord_off.latitude.i > 10) || (angle_off > 15.0)) {
			pilot.set_routine(bank_right);
		}
		else {
			pilot.set_routine(ahead_full);
		}
		break;
	//  Bank starboard (Slightly NorthWest)
	case 24:
		if (needs_immediate_turn) {
			pilot.set_routine(pivot_right);
			delay(500);
			pilot.set_routine(ahead_full);
			needs_immediate_turn = false;
		}
	case 25:
	case 26:
		//  Permit more latitude (heh) in North-checking
		if ((coord_off.latitude.i > 30) || (angle_off > 10.0)) {
			pilot.set_routine(bank_right);
		}
		else if((coord_off.latitude.i < -20) || (angle_off < -10.0)) {
			pilot.set_routine(bank_left);
		}
		else {
			pilot.set_routine(ahead_full);
		}
		break;
	//  I don't even know what to do for error condition. Panic and target 0?
	default:
		_index = 0;
		pilot.halt();
	}
}

bool Navigator::approximately_at(gps_coord_t* goal) {
	return approximate_range(goal) < NAV_WAYPOINT_PRECISION;
}

float Navigator::approximate_range(gps_coord_t* goal) {
	gps_coord_t delta = real_range(goal);
	delta.latitude.f  = (float)delta.latitude.i;
	delta.longitude.f = (float)delta.longitude.i;
	float dist = (delta.latitude.f  * delta.latitude.f)
	           + (delta.longitude.f * delta.longitude.f);
	return sqrt(dist);
}

gps_coord_t Navigator::real_range(gps_coord_t* goal) {
	gps_coord_t ret;
	ret.latitude.i  = goal->latitude.i  - _loc_now.latitude.i;
	ret.longitude.i = goal->longitude.i - _loc_now.longitude.i;
	return ret;
}

nav_direction_t Navigator::approximate_bearing(gps_coord_t* goal) {
	float angle = real_bearing(goal);
	if (angle < 30.0) {
		return North;
	}
	if (angle < 60.0) {
		return NorthEast;
	}
	if (angle < 120.0) {
		return East;
	}
	if (angle < 150.0) {
		return SouthEast;
	}
	if (angle < 210.0) {
		return South;
	}
	if (angle < 240.0) {
		return SouthWest;
	}
	if (angle < 300.0) {
		return West;
	}
	if (angle < 330.0) {
		return NorthWest;
	}
	return North;
}

float Navigator::real_bearing(gps_coord_t* goal) {
	gps_coord_t tmp = real_range(goal);
	tmp.latitude.f  = (float)(tmp.latitude.i);
	tmp.longitude.f = (float)(tmp.longitude.i);
	//  tmp now stores the vector from _loc_now -->> goal
	//  Find the navigational angle of that vector
	float ret = atan2(tmp.longitude.f, tmp.latitude.f);
	//  Convert to degrees
	ret *= (180.0 / M_PI);
	//  Normalize from -180..180 to 0..360
	//  Just kidding DON'T DO THAT
	//  Positive angles mean turn starboard
	//  Negative angles mean turn port
	// if (ret < 0.0) {
	// 	ret += 360.0;
	// }
	return ret;
}

void Navigator::set_next_target() {
	if (_index == 26) {
		pilot.halt();
	}
	else {
		++_index;
	}
	if (_index == 1) {
		pilot.start();
	}
	//  Set any flags needed by the new destination
	if (_index == 14) {
		am_pivoting = true;
	}
	if ((_index == 4)
	||  (_index == 10)
	||  (_index == 12)
	||  (_index == 16)
	||  (_index == 18)
	||  (_index == 24)) {
		// needs_immediate_turn = true;
	}
}

GPS* Navigator::gps() {
	return &_gps;
}

Magnetometer* Navigator::mag() {
	return &_mag;
}

void Navigator::gps_msg_recv() {
	gps_msg_ready = true;
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
	Serial.println(real_range(path[_index].coord).latitude.i);
	Serial.println(real_range(path[_index].coord).longitude.i);
	Serial.println();
}
#endif

Navigator nav;

ISR(TIMER4_COMPA_vect) {
	gps_err_t err = nav.gps()->store_stream();
	if (err == gps_msg_ready) {
		nav.gps_msg_recv();
	}
}
