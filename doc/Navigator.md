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

```cpp
void Navigator::init(void);
```

This initializes the Navigator instance. Call it once in `setup()` to initialize
the GPS and Magnetometer instances for continuous use.

```cpp
void Navigator::navigate(void);
```

This is the `loop()` API. It handles all the navigation calculations, including
updating state and communicating with the Pilot to move towards waypoints.

It should only be called once in a `loop()` iteration. GPS signals only come in
every 200ms, so there is no reason to call it multiply in a loop. As long as the
`loop()` can be guaranteed to run in 100ms or less, the Navigator will function
properly.

```cpp
gps_coord_t Navigator::distance_to(gps_coord_t target);
```

This method takes a target coordinate and the current location, and computes
the distance between them. It returns this as a lat/long struct. Positive values
indicate that the target is north or east of the current location; negative
values indicate that the target is south or west.

### Other Modules

The Navigator writes to the Pilot, and is read by the scheduler.
