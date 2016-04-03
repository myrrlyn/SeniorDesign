# `Ultrasonic` Class

This class provides a driver interface for the HC-SR04 ultrasound sensor module.
The hardware module uses 44kHz tones for echolocation, and should be inaudible
to any organism and relatively unaffected by environmental conditions.

## Hardware

HC-SR04 has two forward-facing sonic components: a speaker and a microphone.
They are labeled T for Transmit and R for Receive. It also has four controller
pins: VCC, TRIG, ECHO, and GND. VCC takes a maximum of 5V from GND, though the
sensors can operate at 3.3V (I believe).

The microcontroller drives TRIG and uses it to signal the module to begin a
measurement. The module drives ECHO and uses it to report back to the
microcontroller.

## Operation

The module fires an ultrasound cone that has a 15 degree arc edge to edge. It
needs at least 2cm of empty space in order to begin counting properly, and tops
out at ~4m.

## Software

To commence a measurement, the microcontroller should perform the following:

1. Assert LOW on the TRIG pin.
2. Delay 10μs (microseconds).
3. Assert HIGH on the TRIG pin.
4. Delay 10μs.
5. Assert LOW on the TRIG pin.

Once the TRIG pin is driven low, the module will immediately send 8 bursts of
44kHz ultrasound and begin listening for the return signal. At this time, we
must instantly switch over to waiting on the ECHO line. The module will hold
ECHO low until it has a value, then drive ECHO high for a variable amount of
time, and then let it fall to LOW again. The time duration of the HIGH pulse
is directly proportional to the distance measured by the sensor's echolocation.

It is the user's responsibility to translate pulse time durations into physical
distance units.

## API

The sensor uses a pair of IO ports and has a single access function. The
constructor takes the IO pair attached to the module, and then is instantly
ready to be used. The module's activation status is controlled with the Boolean
methods. When inactive, the access function does nothing and returns `UINT_MAX`.

###### Constructor

```cpp
Ultrasonic::Ultrasonic(iopair_t pair);
```

Creates an Ultrasonic instance. Should be placed in a permanent scope, such as
top-level (`.data` segment) or before an infinite loop (however, this will put
it in the stack. Be careful with this).

###### Status

```cpp
void Ultrasonic::status(bool);  //  Setter
bool Ultrasonic::status(void);  //  Getter
```

Enables and disables the sensor. Defaults to enabled. Setting to false causes
`measure()` to do nothing and return a fake value of `0xFFFFFFFF`.

###### Reader

```cpp
uint32_t Ultrasonic::measure(void);
```

Triggers the ultrasonic sensor to fire and awaits its response.

NOTE: The `measure()` call **IS BLOCKING**. It depends on the correct operation
of the Arduino library's `millis()` function. Interrupts will not interfere
significantly, but be aware that this function SHOULD NOT be used in
time-critical code.

#### Example

```cpp
#include <Utility.hpp>
#include <Ultrasonic.hpp>

iopair_t us_pins = { .tx = 26, .rx 27 };
Ultrasonic us(us_pins);

us.status();
//  Returns true.

us.measure();
//  BLOCKING. Returns a uint32_t.

us.status(false);

us.status();
//  Returns false.

us.measure();
//  IMMEDIATE. Returns 0xFFFFFFFF.

```
