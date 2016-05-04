# `Lookout` Module

This module holds the `Ultrasonic` instances for all the ultrasonic sensors and
provides foreground loops for iterating across them and performing measurements.

In general operation, the main `loop()` function should perpetually scan the
ultrasonic sensors and pass their measurements to the Pilot. The Navigator will
know to disable or enable individual sensors based on current location, as it
will need to lie to the Pilot module in some instances.

`Lookout.hpp` gives access to the scan functions and the array of pointers to
each sensor. It also `#include`s `<Ultrasonic.hpp>`, so simply `#include`ing
the Lookout header will provide everything needed to use the ultrasonic sensors.

The Lookout is not class-wrapped, since it is has almost no internal state. The
two bare integers used for tracking which sensor is next on the scan list are a
case for class wrapping, but they are file-local and the only exposed state in
the module, so I elected not to do so.

Each sensor maintains its own ring buffer, and pilot state is computed on scan
rather than memoized, so procedural code is fine. The main reason scan state is
even kept, instead of cycling through all six sensors in the head on each scan
call, is that the calls were taking too long and blocking Navigator operation.

## API

```cpp
void us_scan_all(void);
void us_scan_head(void);
void us_scan_tail(void);
void us_debug(uint8_t num);
void us_debug_all(void);
```

The front, back, or full sensor arrays can be scanned by `loop()`. `scan_all()`
calls both `scan_head()` and `scan_tail()`; the individual array scanners cycle
through their collection and only call one sensor per call.

The specific debugger reports the numeric measurement returned by the sensor in
question. The `num` parameter is the sensor's position in the full array. The
general debugger reports the status of the entire collection as clear or
blocked.

The debugger functions only exist in the development environment.
