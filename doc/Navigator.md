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

## Other Modules

The Navigator writes to the Pilot, and is read by the scheduler.

## Methods

```cpp
	gps_coord_t distance_to(gps_coord_t target);
```

This method takes a target coordinate and the current location, and computes
the distance between them. It returns this as a lat/long struct. Positive values
indicate that the target is north or east of the current location; negative
values indicate that the target is south or west.
