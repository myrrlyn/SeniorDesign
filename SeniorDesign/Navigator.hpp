#ifndef __APPAYNE_SENIORDESIGN_NAVIGATOR_HPP
#define __APPAYNE_SENIORDESIGN_NAVIGATOR_HPP

#include <GPS.hpp>
#include <Magnetometer.hpp>

typedef enum : uint16_t {
	//  330 (-30) to 30
	North = 0,
	//  30 to 60
	NorthEast = 45,
	//  60 to 120
	East = 90,
	//  120 to 150
	SouthEast = 135,
	//  150 to 210 (-150)
	South = 180,
	//  210 (-150) to 240 (-120)
	SouthWest = 225,
	//  240 (-120) to 300 (-60)
	West = 270,
	//  300 (-60) to 330 (-30)
	NorthWest = 315,
} nav_angle_t;

class Navigator {
public:
	Navigator(void);
	gps_coord_t delta_to(gps_coord_t target);
	double absolute_distance_to(gps_coord_t target);
	double angle_to(double target);
	double bearing_to(gps_coord_t target);

	nav_angle_t approximate_heading(void);
	bool location_is_approximately(gps_coord_t target);

	void set_target(gps_coord_t target);
	gps_coord_t get_target(void);
	void select_target_from_set(void);

	void instruct_pilot(void);

	GPS* gps(void);
	Magnetometer* mag(void);
private:
	GPS _gps;
	Magnetometer _mag;
	gps_coord_t _target_destination;
	double _target_heading;
	bool at_endpoint;
	bool going_east;
	uint8_t current_target_index;
};

extern Navigator nav;

#endif//__APPAYNE_SENIORDESIGN_NAVIGATOR_HPP
