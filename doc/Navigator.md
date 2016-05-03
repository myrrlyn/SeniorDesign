# `Navigator` Module

The Navigator module is responsible for collecting GPS and magnetometer input to
determine the vehicle's current position and heading. It also determines the
vehicle's current state on the track and instructs the Pilot module on how to
proceed.

The Navigator handles collection and parsing of its sensor data internally, and
exposes pointers to the GPS and magnetometer objects for general use by the
scheduling routines (`setup()` and `loop()`).

## Hardware

The Navigator uses two hardware modules: A GPS and a multi-axis magnetometer.

### GPS

The GPS provides somewhat-noisy GPS information and some guesses on velocity.
Its heading and the compass' heading should be mixed together to get a net
result.

I will need to implement a rolling-average window to figure out a de-bounced
position solution.

### Magnetometer

The magnetometer serves as an overqualified compass. It allows us to measure the
earth's magnetic field and determine the direction the vehicle is facing, which
allows the Navigator to make informed decisions and correct the Pilot if the
vehicle drifts off course.

Due to magnetic noise from the motors, even under Faraday shielding, we are not
using the magnetometer at this time. As such, heading information is derived
solely from the GPS. This is somewhat problematic at low speed, as the GPS
signal bounces.

### Switch

The Navigator uses a switch on pin 4 to determine which terminus to target. A
5V signal indicates Fawick Hall; a 0V signal indicates University Center.

This switch should only be adjusted at a terminus, as the route tracking is a
full loop (UC&rarr;Fawick&rarr;UC), not a single bidirectional path.

## Software

Navigator is a singleton class that owns a GPS and magnetometer, and traces the
robot's position along the route. It communicates with the Pilot to implement
course decisions. It does not need to communicate with the Lookout, as our route
does not have any segments requiring specific sensors to be ignored.

#### Public Types

##### General Directions

```cpp
enum : uint16_t nav_direction_t;
```

This enum maps named directions into navigational degrees. The possible values
are:

| Name      | Value |
|:---------:|------:|
| North     |   0   |
| NorthEast |  45   |
| East      |  90   |
| SouthEast | 135   |
| South     | 180   |
| SouthWest | 225   |
| West      | 270   |
| NorthWest | 315   |
| ANY       | 400   |

The ANY value is used to indicate that a specific direction is unimportant. It
is just a filler value for structures that expect a direction but in a
particular context the direction is irrelevant. Ordinarily, that value would be
zero, but North takes 0.

##### Route Segment

```cpp
enum : uint8_t nav_segment_t;
```

This denotes the sections of the route. They are named as START_END_DIRECTION.
The north and south elements are unused. The east and west elements are used to
determine behavior at the terminus and were planned to adjust environmental
sensitivity, but, this was never realized.

The possible values are:

1. `uc_fawick_east`
2. `uc_fawick_south`
3. `fawick_uc_north`
4. `fawick_uc_west`

##### Waypoint Data

```cpp
struct nav_waypoint_t;
```

This structure defines a single waypoint. An array of waypoints serves as the
route the robot follows.

It has the following members:

1. `gps_coord_t* coord` is a pointer to the coordinate location of the waypoint.
2. `nav_direction_t inbound` is the direction the robot will be traveling when
it arrives at this waypoint.
3. `nav_direction_t outbound` is the direction the robot will be traveling when
it departs this waypoint.
4. `nav_segment_t current_segment` is the segment of the route the robot is on
when traveling towards the waypoint.
5. `nav_segment_t next_segment` is the segment of the route the robot travels
when departing this waypoint.

The inbound and outbound directions were intended to be used as references to
cause enforced turns, but doing so conflicted with the GPS harvesting, so it is
currently unused. The robot banks appropriately anyway.

The segment members are used to identify the terminus and, together with the
control switch, halt or resume robot movement.

#### Instances and Methods

`Navigator.hpp` defines a single Navigator instance.

```cpp
extern Navigator nav;
```

This is the only instance that should ever be used in this project. Initialize
it in `setup()` and navigate with it in `loop()`.

##### Constructor

```cpp
Navigator::Navigator(void);
```

Constructs a Navigator module instance. This also constructs the GPS and
Magnetometer drivers, which should be known only to the Navigator.

##### Initializer

```cpp
void Navigator::init(void);
```

This initializes the Navigator instance. Call it once in `setup()` to initialize
the GPS and Magnetometer instances for continuous use. It also sets up a timer
to run the GPS harvesting.

##### Navigator Master

```cpp
void Navigator::navigate(void);
```

This is the `loop()` API. It handles all the navigation calculations, including
updating state and communicating with the Pilot to move towards waypoints.

It should only be called once in a `loop()` iteration. It will self-terminate if
run more than once every 100ms. This handles the entirety of the navigation, and
can be somewhat lengthy at times.

This is a high priority task, and while it only needs to run every 200ms it
absolutely **NEEDS** to run when a new GPS signal is received.

##### Navigation Calculations

```cpp
bool Navigator::approximately_at(gps_coord_t* goal);
float Navigator::approximate_range(gps_coord_t* goal);
gps_coord_t Navigator::real_range(gps_coord_t* goal);
nav_direction_t Navigator::approximate_bearing(gps_coord_t* goal);
float Navigator::real_bearing(gps_coord_t* goal);
```

These functions provide processing functionality for determining where the robot
is in relation to a given location.

Because the GPS signal is noisy, we use `approximately_at()` to provide some
error acceptance. The tolerance is defined in the `NAV_WAYPOINT_PRECISION`
macro. It is presently set to 30.0; this value can be scaled up or down as
needed.

The `real_` functions measure the exact distance and bearing between the current
location and the target. `real_range()` returns a `gps_coord_t` that is the
Manhattan distance &ndash; the latitude field represents how far north of target
the robot is, and the longitude field represents how far east of target.
`real_bearing()` returns the angle between the two points (as navigation, not
Cartesian).

`approximate_range()` provides the Pythagorean distance between current location
and the target. `approximately_at()` simply compares this value with the
tolerance to determine whether the robot is within an error radius, for example.
`approximate_bearing()` reduces real bearings onto the `nav_direction_t` set.
It does not do this equally; the primary directions (North, East, South, West)
are 60&deg; wide and the secondary directions (NE, SE, SW, NW) are 30&deg wide.

`real_bearing()` and `approximate_range()` involve floating point math.
`real_bearing()` also requires trigonometry (`atan2()`). These functions may be
slow; hence why `navigate()` should not be called continuously.

##### Route Management

```cpp
void Navigator::set_next_target(void);
```

This runs the state machine to determine when to cycle targets, what behavior is
needed at each target, and shuts down at the endpoints. It is just a giant
switch statement. Note that each case in the switch stack is the FUTURE point,
not the CURRENT point. Try to dodge the inevitable off-by-one errors.

##### Pilot Communicator

```cpp
void Navigator::instruct_pilot(void);
```

This sends movement requests to the Pilot. Requests are computed in `navigate()`
to match the Pilot API to the Navigator's spatial awareness.

##### Miscellaneous

```cpp
GPS* Navigator::gps(void);
Magnetometer* Navigator::mag(void);
inline void gps_msg_recv(void);
```

These provide access to the GPS and Magnetometer. The `gps_msg_recv()` function
is used by the timer to inform the Navigator that a complete message is ready
for parsing.

### Other Modules

The Navigator writes to the Pilot, and is called by the scheduler.
