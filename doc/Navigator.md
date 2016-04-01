# Navigator

The Navigator module is responsible for reading from the GPS and compass to
determine the vehicle's current travel state and instruct the Pilot on how to
proceed.

## Hardware

The Navigator uses two hardware modules: A GPS and a multi-axis magnetometer.

### Magnetometer

The magnetometer serves as an overqualified compass. It allows us to measure the
earth's magnetic field and determine the direction the vehicle is facing, which
allows the Navigator to make informed decisions and correct the Pilot if the
vehicle drifts off course.
