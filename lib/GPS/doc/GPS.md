# `GPS` Driver

This is a driver for the Adafruit Ultimate GPS Breakout v3 module. The hardware
device consists of an MTK3339 GPS component and some accessory circuits.

## Overview

The GPS driver consumes serial communications from the hardware module and
produces parsed, meaningful information for public use. Although the hardware
communicates using serial character streams, the driver is **not** a subclass of
`SoftwareSerial`. The driver exposes the bare minimum of Serial APIs required to
run communications. The driver makes the parsed information available for public
consumption, as well as the most recent string for debugging purposes or if any
other class wants to try their hand at parsing.

## Hardware

The MTK3339 chipset has a small ceramic patch antenna and GPS processor set to
automatically do their job on power-up. The Adafruit component puts some
accessory circuits around the chipset and provides nine I/O pins for access. One
of the accessories is a u.FL port for an external antenna, if the onboard one is
not sensitive enough or in a shielded spot.

It is able to be powered by its host device or by a coin cell battery attached
to its ventral face, and communicates with the host using twin serial Tx/Rx
channels. The module runs on 3.3V, but can accept up to 5V VCC.

Powering the device with an external battery will keep it online even when the
host is asleep, letting it keep GPS fix while keeping the host is shut down.

### Pins

1. PPS -- This is an output pin that emits a 50-100ms HIGH pulse every second.
It can be used for synchronizing with the microcontroller.
2. VIN -- This is the power input supply. It is capable of receiving up to 5V,
though the MTK3339 is 3.3V internally. An onboard 3.3V regulator steps VIN down
to usable levels.
3. GND -- The ground reference. Must be common ground with the microcontroller.
4. RX  -- receives byte streams from a microcontroller.
5. TX  -- transmits byte streams to a microcontroller.
6. FIX -- emits a status code representing acquisition state. Toggles every
second while searching; pulses HIGH for 200ms every 15 seconds after fix is
acquired.
7. VBAT -- used for the voltage input from an external battery, if one is not
mounted to its ventral side.
8. EN  -- a software on/off switch. Pulled up internally; when driven LOW the
module goes to sleep.
9. 3.3V -- The output from the onboard 3.3V regulator. Can be used to provide
clean and steady 3.3V to other devices, up to 100mA.

## Operation

When powered on, the module immediately begins searching for satellites. It will
also emit byte streams on TX according to its default settings (1Hz write, RMC
and GGA sentences, 9600 baud).

Pulling EN to ground will put the module to sleep; it will reboot when EN goes
HIGH or floats.

Writing to RX can configure the module. See documentation on PMTK commands.

## Software

The module performs the GPS calculations internally. It communicates in both
directions using ASCII characters, and terminates sentences in `\r\n\0`.
Messages do not have a header, and may or may not include an XOR checksum
for message verification.

Internal message storage is accomplished using two RingBuffer instances (see
RingBuffer documentation). Each buffer is statically allocated and *should* be
large enough to hold a complete sentence. If a sentence is larger than the
buffer, then the buffer will continuously overwrite its last cell rather than
overflow. This will cause a checksum failure, and the message will be discarded
unparsed.

## API

The `GPS` class uses a `Serial` object by composition, not inheritance. Common
API elements are a formality.

The GPS instance must be constructed with the address of a pre-existing `Serial`
instance. It can accept either built in `HardwareSerial` objects (such as
`Serial1`) or specialty `SoftwareSerial` instances. The GPS constructor does not
set pin modes or even know what pins are in use -- this is the responsibility of
the Serial device.

Once constructed, the GPS instance must be initialized in `setup()` by calling
`begin()` with an optional baud rate. This defaults to 9600 baud. The
microcontroller's perception of the baud rate should be changed only AFTER
sending a new baud instruction to the GPS unit. Predefined messages for this
purpose are in the header ready for use. NOTE: Haven't actually tried this.

Once initialized, the GPS module will automatically begin its work and
communicate with the microcontroller according to its print update settings.
The microcontroller needs to monitor the Serial device and store GPS traffic,
either by using a timer or change interrupt or in the foreground.

The `available()` method can be used to abort a read if there is nothing ready.
The `read()` method automatically calls it, returning -1 if it fails. The
`store()` method automatically discards -1 values, so it is perfectly safe to
leave `gps.store(gps.read())` in an infinite loop or an ISR.

The `sentence_ready()` flag goes up when a GPS sentence is fully stored. At this
time, the buffers switch roles, so the stable sentence is in `buf_stable`.
The `latest_sentence()` method will permit reading it as a standard C string
with `NULL` termination. `latest_sentence()` also puts the flag back down, so
take care not to cause interference if a method is waiting on the flag.

The `parse()` method takes in a C string and sets instance variables. If given a
NULL string, `parse()` defaults to reading from the stable buffer. The parameter
is made public, rather than hard-coded to the stable buffer, so that the class
can provide parsing services to sentences acquired elsewhere.

Once a successful `parse()` occurs, the various getter methods can be used to
retrieve the parsed data. Data is stable until overwritten by a later parse.

This class is fully self-contained and uses no global/class-static variables,
and does *not* have to be used as a singleton. `Adafruit_GPS` library, I'm
looking at you.

### Recommended Usage

For fully automatic operation, use a hardware Serial device, as those have
automatic interrupt behavior on RX availability with defined callbacks. For
example, the `Serial1` RX pin has an interrupt routine that is configured to
call the `void serialEvent1(void)` function on occurrence.

```cpp
#include <GPS.hpp>

GPS example(&Serial1);

void setup(void) {
	Serial.begin(115200);
	example.begin(9600);
}

void serialEvent1(void) {
	example.store_stream();
}

void loop(void) {
	if (example.sentence_ready()) {
//  Display the raw sentence
		Serial.println(example.latest_sentence());
//  Parse it and store the information
		example.parse(example.latest_sentence());
//  Display the debug printout
		example.debug();
	}
}
```

### Types and Definitions

Several custom types and macros are defined by this driver. The `GPS_*` macros
are preconfigured sentences ready to be sent to the device via `command()`.

###### `gps_err_t`

This enumerates the various error/status codes that `GPS` methods can return.

1. `gps_err_none` &ndash; Successful operation.
2. `gps_err_nocomm` &ndash; Communication is not established with the device.
3. `gps_err_badcsum` &ndash; A sentence provided a checksum but failed to meet
it.
4. `gps_err_nocsum` &ndash; A sentence did not provide a checksum at all.
5. `gps_err_nofix` &ndash; A valid sentence indicated a GPS fix had not yet been
acquired, and so there is no data to parse.
6. `gps_err_noparse` &ndash; A valid sentence is of an NMEA type for which no
parser is defined. Parsers can be added to this driver or as a subclass. Parsers
must be registered with their tag in the main `parse()` function multiplexer.
7. `gps_err_nochar` &ndash; The Serial device does not have a character to read.
8. `gps_err_timeout` &ndash; A blocking call has expired unsatisfied.
9. `gps_msg_ready` &ndash; A complete sentence has been stored and is ready for
access via `latest_sentence()`.

###### `gps_time_t`

This structure stores a full timestamp. It has the following fields:

1. `uint8_t year` &ndash; The two least significant digits of the current year.
2. `uint8_t month` &ndash; The month, in 1 through 12.
3. `uint8_t day` &ndash; The day of the month, starting at 1.
4. `uint8_t hour` &ndash; The hour, in 0 through 23.
5. `uint8_t minute` &ndash; The minute, in 0 through 59.
6. `uint8_t second` &ndash; The second, in 0 through 59. GPS time does not
recognize leap seconds.
7. `uint16t millisecond` &ndash; The millisecond, in 0 through 999.

###### `gps_coord_u`

This union allows for the storage of coordinate values as 32-bit integral or
floating-point numbers. C does not enforce union type safety; that's up to the
user. This library uses only integrals internally, at this time.

- `double d` &ndash; Access 32 bits of storage as IEEE754 floating point.
- `int32_t i` &ndash; Access 32 bits of storage as signed integer.

###### `gps_coord_t`

This struct stores a latitude and longitude. Altitude is stored separately.

1. `gps_coord_u latitude` &ndash; The latitude, in -90.0 through 90.0.
2. `gps_coord_u longitude` &ndash; The longitude, in -180.0 through 180.0.

###### `gps_velocity_t`

The RMC sentence contains a computed velocity vector based on the most recent
few points measured.

1. `double speed` &ndash; The land-speed component, in knots.
2. `double heading` &ndash; The heading, in degrees from true North.

###### `gps_fix_t`

This enumerates the fix types the antenna can acquire.

1. `0` &ndash; No valid fix yet.
2. `1` &ndash; The fix is based solely on GPS.
3. `2` &ndash; The fix is augmented DGPS.

### Methods

###### Constuctors

```cpp
GPS(HardwareSerial* hwser);
GPS(SoftwareSerial* swser);
```

Constructs a GPS instance with a given Serial device, which must be instantiated
prior to this. The Serial device is initialized later. This is the only
opportunity to set a Serial device, so passing in NULL will prevent the GPS
instance from ever attempting communication with hardware; however, it will
still serve as a functional NMEA parser.

```cpp
SoftwareSerial ss(52, 53);
GPS sgps(&ss);
GPS hgps(&Serial1);
```

###### Initializer

```cpp
gps_err_t begin(uint16_t baud = 9600);
```

Initializes the Serial device at the specified baud rate. If no Serial device is
given, this returns NOCOMM; otherwise it returns success. The baud rate defaults
to 9600, as this is the rate to which the hardware module automatically sets
itself.

```cpp
hgps.begin();
```

###### Serial->Storage Shunt

```cpp
gps_err_t store_stream(void);
```

This method shunts characters from the Serial device to internal storage. It
uses `available()` internally, and will run until the Serial device is dry. It
returns either success, or message complete.

This is the method that should be called to transfer data from the Serial stream
to instance storage. `available()` is deprecated and will be removed; the direct
stream accessors have already been marked `protected` and can no longer be
invoked publicly.

###### Sentence Availability Flag

```cpp
bool sentence_ready(void);
```

Returns true when a complete sentence has been stored and made ready for access.
Returns false otherwise.

A sentence is considered complete when the '\n' glyph arrives and is stored. The
terminating `\0` glyph is not used, since it may not be detected by the Serial
device.

```cpp
if (hgps.sentence_ready()) {
	Serial.println("New sentence!");
}
```

###### Sentence Accessor

```cpp
char* latest_sentence(void);
```

This method yields a pointer to the first cell in the off-duty, stable, buffer,
permitting standard C-style iteration across the buffer contents. All unused
buffer cells are set to `0x00`, so the strings are `NULL`-terminated as expected
by C idiom. At this time, plans to stow length alongside data are under
consideration but on low priority.

This method also sets the flag returned by `sentence_ready()` to false. As such,
the `sentence_ready()` flag should not be used to trigger an automatic `parse()`
of the `latest_sentence()`. The `store_sentence()` method will return
`gps_msg_ready` &ndash; automatic parsing should be based on a capture of this
code.

```cpp
Serial.println(hgps.latest_sentence());
//  Prints an NMEA sentence to console.
```

###### PARSER

```cpp
gps_err_t parse(char* sentence);
```

This is the only public interface to the parsing routines. As the parser engine
is distinct from the Serial communication interface, it takes a pointer to a
string &ndash; this pointer can be `latest_sentence()` or one provided by the
client.

This method handles checksum verification and dispatches the sentence to the
appropriate NMEA parser. If the sentence fails checksum verification, it returns
`gps_err_badcsum` or `gps_err_nocsum`; if the sentence is not of a type for
which a parser is defined, it returns `gps_err_noparse`. Otherwise, the relevant
parser is invoked and this method returns the called parser's error code.

Parsing is not guaranteed to be an atomic operation. A proper field may have its
data stored immediately or when the sentence is complete. If an improper field
is encountered, it may be skipped, or cause the parser to abort and skip the
rest of the sentence. If a `parse()` call returns anything other than success,
**all** the information available for access should be considered suspect until
a successful parse occurs.

However, if a well-formed sentence appears, it is very likely to have all its
fields correct and parseable. The GPS module is good at its job.

```cpp
gps_err_t err;
err = hgps.parse(hgps.latest_sentence());
//  hgps' instance variables are updated, if the sentence was parseable.
```

###### Module Command

```cpp
gps_err_t command(const char* sentence);
```

Sends a specific command string to the module. The module will discard any that
do not conform to NMEA specifications, including the checksum calculation. It is
recommended that only the sentences pre-defined in the header file are used with
this method.

```cpp
hgps.command(GPS_API_UPDATE_5000);
```

###### Await Response from Module

```cpp
gps_err_t await_response(const char* sentence, uint8_t timeout = 5);
```

This is a **BLOCKING** call to wait for a specific response type from the GPS
module. It will collect up to `timeout` sentences, comparing each to the given
string, and return success or a timeout error.

```cpp
hgps.await_response(GPS_AWAKE);
//  Waits for the module to report that it is fully awake
```

###### Module Sleep

```cpp
gps_err_t pause(bool status);
```

Puts the module to sleep or wakes it up. Fix status may or may not be maintained
through sleep.

`true` sleeps; `false` wakes.

Does nothing if you set it to the status it already has.

```cpp
hgps.pause(true);
//  module is asleep
hgps.pause(false);
//  module wakes up
```

#### DATA ACCESSORS

###### Timestamp

```cpp
gps_time_t timestamp(void);
```

This returns the timestamp as recorded from the last parse.

Note that the RMC and GGA sentences store the date component at the end of the
message, and the time at the beginning, so until a fix is acquired the time may
be stored but a parse will abort with `gps_err_nofix` before reaching the date.
As such, this method can always be used to set a clock, but cannot guarantee
setting a calendar.

###### Lat/Long

```cpp
gps_coord_t location(void);
```

This returns the 2-dimensional location as recorded from the last parse.

Altitude is not included in this structure, as altitude calculation is
frequently separate from the lat/long calculations and also has various
reference points. See `altitude()`.

NOTE: As described above, each member in this struct is a union of float32 and
int32. Access with care. This library stores coordinates as integers.

###### Horizontal Precision

```cpp
double hdop(void);
```

This returns the horizontal dilution of precision, as calculated by the module.

- A value less than 1 indicates ideal, highest-quality measurements.
- A value between 1 and 2 is high quality.
- A value between 2 and 5 is acceptable, but should be augmented by other data.
- A value between 5 and 10 is acceptable. The environment is interfering with
the antenna &ndash; get a more sensitive antenna or move to a clearer area.
- A value between 10 and 20 is significantly imprecise and should only be used
for rough estimations of precision.
- Values greater than 20 should just be discarded entirely.

###### Altitude

```cpp
double altitude(bool sea_or_wgs84 = false);
```

This method returns the altitude, either from sea level or the WGS84 reference
spheroid. If the parameter is `false`, it returns height from sea level; if
`true`, it returns the difference from the reference spheroid.

###### Velocity

```cpp
gps_velocity_t velocity(void);
```

The RMC sentence contains a computed velocity vector based on arithmetic of the
last few coordinates measured. It contains two `double` fields (see above for
spec). The `speed` field is land-speed in knots; the `heading` field is heading
in degrees from true North.

NOTE: Navigational degrees are clockwise from North (+Y). Mathematical degrees
are counter-clockwise from East (+X). Watch out for that.

###### Fix Quality Information

```cpp
gps_fix_t fix_info(void);
```

This returns an enumeration listing the type of the fix acquired. See above for
details.

###### Satellite Count

```cpp
uint8_t satellites(void);
```

This returns the count of satellites in active view of the antenna.

#### PROTECTED METHODS

###### Stream Availability

```cpp
bool available(void);
```

Wraps the underlying Serial device's `available()` method. Used internally to
govern the shunt of data from the Serial device buffer to internal storage.

For public usage, calling `store_stream()` is sufficient. That is a blocking
call that will fully empty the Serial buffer into storage and notify the caller
if a sentence transmission is complete.

###### Character Reader

```cpp
char GPS::read(void);
```

Reads a single character from the head of the Serial device's queue and returns
it directly. Returns -1 if the GPS module is asleep or no characters are
available from the Serial device.

###### Character Accumulator

```cpp
gps_err_t GPS::store(char inbound);
```

This method takes a single character from the Serial stream and stores it in the
instance's buffers. It automatically discards -1, as that is a known error
value, and upon detecting end-of-line, raises a new-sentence flag and switches
out the buffers so the previous sentence can be read while the current is being
stored. It also ensures that buffers are re-initialized to full zeros to ensure
that old, long sentences do not crop up in new, short ones.

Returns a `msg_ready` signal as well as raising the instance flag when a new
sentence is completed. Returns `err_none` otherwise, as it is impossible for
this method to fail. If an absurdly long sentence overruns the buffer space, the
end buffer cell is continually overwritten in place until the sentence
terminates. The parser will simply discard the broken sentence, and continue on.

###### Checksum Validator

```cpp
gps_err_t validate_checksum(char* sentence);
```

This inspects an NMEA sentence for the hexadecimal XOR checksum footer and
ensures that the contents match the checksum. Returns `nocsum` or `badcsum` on
error.

##### PARSERS

```cpp
gps_err_t parse_gga(char* sentence);
gps_err_t parse_rmc(char* sentence);
```

These methods parse specific NMEA sentence types and store their relevant data
in the instance variables. The master `parse()` method routes sentences to the
appropriate method. They should not be called directly.

These are marked as protected in the event that you want to alter their behavior
in some way, such as storing coordinates as floating-point rather than integral.

```cpp
gps_err_t parse_time(char* fragment);
gps_err_t parse_date(char* fragment);
gps_err_t parse_coord(char* fragment);
gps_err_t parse_double(double* store, char* fragment, uint8_t precision);
```

These methods are used for parsing specific data fields of a sentence. NMEA
standards are very regular, and processing text into numbers is a relatively
simple, though tedious, matter. Since RMC and GGA have some common fields, it
makes sense to pull out individual field parsing into common subroutines.
Again, *looking at you, Adafruit*.

#### Debugging

```cpp
void debug(void);
```

Dumps all the parsed information to `Serial` for viewing on a text console. This
method only exists if the preprocessor is told to `#define DEVEL`.

## Appendix

This contains miscellaneous information related to the operation of the GPS
module.

### NMEA Sentences

The module is capable of emitting various NMEA sentences. This library provides
semantic parsing for some of them. The most recent parsed information is stored
in the respective instance variable. All fields are separated by commas. No
comma precedes the checksum.

#### GGA

General Fix Information. This sentence contains a date/time stamp, location,
altitude measurements, and information about the GPS information set. We do not
parse the DGPS information fields at this time, but the rest can be found in
instance variables.

###### Fields

1. `$GPGGA` -- Sentence header.
2. `/[0-9]{6}\.[0-9]+/` -- UTC timestamp in HHMMSS.sss.
3. `/[0-9]{4}\.[0-9]{4}/` -- Latitude in DDMM.mmmm.
4. `/[NS]/` -- Hemisphere. N for North, S for South.
5. `/[0-9]{5}\.[0-9]{4}/` -- Longitude in DDDMM.mmmm.
6. `/[EW]/` -- Hemisphere. E for East, W for West.
7. `/[012]/` -- Fix quality information (see `gps_fix_t`).
8. `/[0-9]+/` -- Count of satellites in view.
9. `/[0-9]+\.[0-9]/` -- HDOP.
10. `/\-?[0-9]+\.[0-9]/` -- Altitude from sea level.
11. `M` -- Flag for altitude.
12. `/\-?[0-9]+\.[0-9]/` -- Altitude from WGS84 geoid.
13. `M` -- Flag for altitude.
14. `/[0-9]+\.[0-9]+/` -- Age of Differential GPS information.
15. `/[0-9]+/` -- DGPS station identifier.
16. `/\*[0-9A-F]{2}/` -- XOR checksum of all bytes between `$` and `*`,
non-inclusive.

#### RMC

Recommended Minimum Certainty. This sentence contains a date/time stamp,
location, and calculated speed and heading, as well as some extra information.
The magnetic variation field is optional. We do not parse it at this time.

###### Fields

1. `$GPRMC` -- Sentence header.
2. `/[0-9]{6}\.[0-9]+/` -- UTC timestamp in HHMMSS.sss.
3. `/[AV]/` -- Fix status. V for Void (no fix), A for Active (fix).
4. `/[0-9]{4}\.[0-9]{4}/` -- Latitude in DDMM.mmmm.
5. `/[NS]/` -- Hemisphere. N for North, S for South.
6. `/[0-9]{5}\.[0-9]{4}/` -- Longitude in DDDMM.mmmm.
7. `/[EW]/` -- Hemisphere. E for East, W for West.
8. `/[0-9]+\.[0-9]{2}/` -- Speed in knots.
9. `/[0-9]+\.[0-9]{2}/` -- Heading in degrees true.
10. `/[0-9]{6}/` -- UTC datestamp in DDMMYY.
11. `/[0-9]+\.[0-9]/` -- Magnetic variation (optional).
12. `/[EW]/` -- Magnetic variation direction (optional).
13. `*[0-9A-F]{2}` -- XOR checksum of all bytes between `$` and `*`,
non-inclusive.

### Message Buffers

Any linear buffer with overflow protection will work. I use the `RingBuffer_gps`
class provided by my `RingBuffer` library, with room for 96 characters of GPS
message. As the buffers are not used in cyclical mode, the extra functionality
is unnecessary; all the buffer needs is a `write()` method that won't overflow
and a publicly available pointer to its head.

The GPS instance stores two pointers, named `active` and `stable`, to two
buffers. When a message is completely received and stored, `stable` is set to
point at the full message and `active` is set to point at the buffer currently
being filled. Only the most recent complete sentence is stored; as long as the
sentence is parsed before the next one finishes arriving, that's all that's
really necessary anyway.

Parsing is pretty fast, and sentences will only arrive every 100ms, max.

TODO: Implement a simple linear buffer internally to make this a standalone
library, and then publish so that people stop using Adafruit's GPS library.
