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
