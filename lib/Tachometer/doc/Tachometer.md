# `Tachometer` Class

This class provides a simple stateful driver for Hall-Effect sensors reading
from magnets mounted to a rotating axle. We have two such axles, and the Pilot
will read each tachometer and use it in a closed feedback loop to determine the
levels of power to give to the motors in order to achieve a desired real-world
speed.

## Hardware

Any digital Hall-Effect sensor will work. Mine are Littelfuse 3H-02-A-ND sensors
which, to be perfectly honest, I bought because they had a mounting flange and a
small LED which lit up when a magnet was nearby, making installation and testing
really simple.

## Operation

The south pole of the magnet must be facing the sensor. When the magnet is in
range, the sensor emits LOGIC HIGH. When the magnet is not in range, the sensor
emits LOGIC LOW.

## Software

The tachometer simply measures time intervals between state changes. The Arduino
framwork provides millisecond-level timer resolution, which is sufficient for
this purpose. The sensor objects track and debounce their current state, and
when a confirmed switch occurs, the current time interval is recorded and the
timer reset.

## API

The sensors only have one data line, which can go on any pin. Pins supporting
change interrupts are preferable, as this will allow background operation of the
tachometer. Foreground scheduling is lossy and "considered harmful".

###### Constructor

```cpp
Tachometer::Tachometer(gpio_t pin, double magic_number = 15000.0);
```

Constructs a Tachometer instance on a given input port. It is not ready for use until `begin()` is called.

The optional magic number argument is used to calibrate the sensor's internal
calculations to transform time interval into angular velocity.

```cpp
#include <Utility.hpp>
Tachometer tach_one(4);
```

###### Initializer

```cpp
void Tachometer::begin(void);
```

This sets the instance timer and initial state. Call it at the end of `setup()`
for a good starting point. Until this is called, instance methods return zero.

```cpp
tach_one.begin();
```

###### Read State

```cpp
bool Tachometer::read(void);
```

Non-blocking call to examine the current state of the sensor. If the sensor's
current state is different from the last call to `read()`, it updates the
instance variables with new data and returns `true`. If the current and previous
states are the same, then nothing has happened and the call returns `false`.

```cpp
tach_one.read();
```

###### Get Time Elapsed

```cpp
uint32_t Tachometer::interval(void);
```

This returns the most recent interval between pole changes the sensor has
recorded.

```cpp
uint32_t last_interval = tach_one.interval();
```

###### Get Angular Velocity

```cpp
double Tachometer::rpm(void);
```

This returns the most recent calculated RPM the sensor has seen.

```cpp
double last_rpm = tach_one.rpm();
```

###### Reset

```cpp
void Tachometer::reset(void);
```

This sets the instance's state variables to the current time and magnetic pole.
This creates a new ground reference for `read()` and will affect the next
reading and calculations.

```cpp
//  An event occurs that messes up the sensor's expectations.
us_one.reset();
```

### Private Methods

##### Calculate RPM

```cpp
double Tachometer::calc_rpm(unit32_t delta);
```

This uses an instance-local magic number to transform a time interval into an
angular velocity measurement. Each sensor should be independently
calibrated. The magic number can currently only be set during construction, as
attempting to recalibrate during runtime is currently a futile endeavour.
