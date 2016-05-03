#ifndef __APPAYNE_SENIORDESIGN_NAVIGATOR_HPP
#define __APPAYNE_SENIORDESIGN_NAVIGATOR_HPP

#include <GPS.hpp>
#include <Magnetometer.hpp>
#include <Utility.hpp>

typedef enum : uint16_t {
	//  330/-30 to 30
	North = 0,
	//  30 to 60
	NorthEast = 45,
	//  60 to 120
	East = 90,
	//  120 to 150
	SouthEast = 135,
	//  150 to 210/-150
	South = 180,
	//  210/-150 to 240/-120
	SouthWest = 225,
	//  240/-120 to 300/-60
	West = 270,
	//  300/-60 to 330/-30
	NorthWest = 315,
	//  Misc
	ANY = 400,
} nav_direction_t;

//  Start_End_Direction
typedef enum : uint8_t {
	uc_fawick_east,
	uc_fawick_south,
	fawick_uc_north,
	fawick_uc_west,
	NOT_YET_ON_ROUTE,
} nav_segment_t;

typedef struct {
	gps_coord_t* coord;
	nav_direction_t inbound;
	nav_direction_t outbound;
	nav_segment_t current_segment;
	nav_segment_t next_segment;
} nav_waypoint_t;

class Navigator {
public:
	Navigator(void);
	void init(void);
	void navigate(void);

	bool approximately_at(gps_coord_t* goal);
	float approximate_range(gps_coord_t* goal);
	gps_coord_t real_range(gps_coord_t* goal);
	nav_direction_t approximate_bearing(gps_coord_t* goal);
	float real_bearing(gps_coord_t* goal);

	void set_next_target(void);

	void instruct_pilot(void);

	GPS* gps(void);
	Magnetometer* mag(void);

	inline void gps_msg_recv(void);

#ifdef DEVEL
	void debug(void);
#endif
private:
	GPS _gps;
	Magnetometer _mag;
	gpio_t _pin;

	//  5V means FAWICK, 0V means UC
	bool _pin_reading;

	gps_coord_t _loc_now;
	float _dir_now;
	uint8_t _index;
	nav_direction_t _heading;
	bool am_pivoting;
	bool needs_immediate_turn;

	volatile bool gps_msg_ready;
};

extern Navigator nav;

#endif//__APPAYNE_SENIORDESIGN_NAVIGATOR_HPP
