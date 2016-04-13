#ifndef __APPAYNE_SENIORDESIGN_NAVIGATOR_HPP
#define __APPAYNE_SENIORDESIGN_NAVIGATOR_HPP

#include <GPS.hpp>
#include <Magnetometer.hpp>

class Navigator {
public:
	Navigator(void);
	gps_coord_t distance_to(gps_coord_t target);

	// void debug_gps(void);
	// void debug_mag(void);

	GPS* gps(void);
	Magnetometer* mag(void);
private:
	GPS _gps;
	Magnetometer _mag;
};

// extern GPS gps;
// extern Magnetometer compass;
extern Navigator nav;

extern void debug_gps(void);
extern void debug_mag(void);

#endif//__APPAYNE_SENIORDESIGN_NAVIGATOR_HPP
