# `GPS` Class

This class provides a driver for the Adafruit Ultimate GPS Breakout v3 module.
The GPS module used on this is the MTK3339 chipset. The module is also capable
of mounting a coin cell battery to its ventral face for use as a power supply.
Doing so will permit the module to acquire and maintain a fix without drawing
microcontroller power, and reduces time needed to resume operations after
microcontroller sleep.

## Hardware

The module is a small PCB with the MTK3339 taking up most of the space. It has
nine pins and a u.FL external antenna port.

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

## API

The `GPS` class *wraps* a Serial communication instance, but is **not** a subclass of them. The shared API elements is merely a formality.

The GPS instance must be constructed with the address of a pre-existing Serial
instance. It can accept either built in `HardwareSerial` objects (such as
`Serial1`) or specialty `SoftwareSerial` instances. The GPS constructor does not
set pin modes or even know what pins are in use -- this is the responsibility of
the Serial device.

Once constructed, the GPS instance must be initialized in `setup()` by calling
`begin()` with an optional baud rate. This defaults to 9600 baud. The
microcontroller's perception of the baud rate should be changed only AFTER
sending a new baud instruction to the GPS unit. Predefined messages for this
purpose are in the header ready for use.

Once initialized, the GPS module will automatically begin its work and
communicate with the microcontroller according to its print update settings.
The microcontroller needs to monitor the Serial device and store GPS traffic,
either by using a timer or change interrupt or in the foreground.

The `available()` method can be used to abort a read if there is nothing ready.
The `read()` method automatically calls it, returning -1 if it fails. The
`store()` method automatically discards -1 values, so it is perfectly safe to
leave `gps.store(gps.read())` in an infinite loop or an ISR.

The `sentence_ready()` flag goes up when a GPS sentence is fully stored. At this
time, the buffers switch roles, so the stable sentence is in `buf_second`.
The `latest_sentence()` method will permit reading it as a standard C string
with `NULL` termination.

The `parse()` method takes in a C string and sets instance variables. At present
these are only exposed via the debug() method; specific getters will be
forthcoming.

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
	example.store(example.read());
}

void loop(void) {
	if (example.sentence_ready()) {
		Serial.println(example.latest_sentence());
		example.parse(example.latest_sentence());
		example.debug();
	}
}
```

###### Constuctors

```cpp
GPS::GPS(HardwareSerial* hwser);
GPS::GPS(SoftwareSerial* swser);
```

Constructs a GPS instance with a given Serial device, which must be instantiated
prior to this.

```cpp
SoftwareSerial ss(52, 53);
GPS sgps(&ss);
GPS hgps(&Serial1);
```

###### Initializer

```cpp
gps_err_t GPS::begin(uint16_t baud = 9600);
```

Initializes the Serial device at the specified baud rate. Will fail with a
NOCOMM error if you somehow manage to make a GPS instance that doesn't have a
Serial device attached.

```cpp
hgps.begin();  //  optional argument, defaults to 9600
```

###### Stream Availability

```cpp
bool GPS::available(void);
```

This is a **DEPRECATED** function. Currently, it exposes the `available()` state
of the underlying Serial device. This is unnecessary and breaks encapsulation.
The read method envelops its functionality.

```cpp
if (hgps.available()) {
	UDR0 = hgps.read();
}
```

###### Character Reader

```cpp
char GPS::read(void);
```

Reads a single character from the head of the Serial device's queue and returns
it directly. Returns -1 if the GPS module is asleep or no characters are
available from the Serial device.

```cpp
UDR0 = hgps.read();
```

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
sentence is completed. Returns `err_none` otherwise, as it is impossible for this
method to fail. If an absurdly long sentence overruns the buffer space, the end
buffer cell is continually overwritten in place until the sentence terminates.
The parser will simply discard the broken sentence, and continue on.

```cpp
hgps.store(hgps.read());
```

###### Sentence Availability Flag

```cpp
bool GPS::sentence_ready(void);
```

Returns true when a complete sentence has been stored and made ready for access.
Returns false otherwise.

A sentence is considered complete when the '\n' glyph arrives and is stored.

```cpp
if (hgps.sentence_ready()) {
	Serial.println("New sentence!");
}
```

###### Sentence Accessor

```cpp
char* GPS::latest_sentence(void);
```

This method yields a pointer to the first cell in the off-duty buffer,
permitting standard C-style iteration across the buffer contents. All unused
buffer cells are set to `0x00`, so the strings are `NULL`-terminated as expected
by C idiom. At this time, plans to stow length alongside data are planned but
on low priority.

```cpp
Serial.println(hgps.latest_sentence());
//  Prints an NMEA sentence to console.
```

###### PARSER

```cpp
gps_err_t GPS::parse(char* sentence);
```

This is the meat of the class. It verifies the checksum of the sentence it is
given, scans it for known sentence types, and hands it off to the appropriate
worker function.

Parser functions are essentially `void`-returning. They will fail upon
encountering malformed sentences, but will attempt to process as much of the
data as they can. They do NOT perform atomic update operations on the instance
variables -- a failure midway through a parse will not prevent earlier parsed
elements from being stored, nor roll them back. However, they only clobber data
when updating it; a failure does not destroy old information either.

Returns `noparse` if it could not find a suitable delegate, or curries the
return code from the specific parser used.

```cpp
gps_err_t err;
err = hgps.parse(hgps.latest_sentence());
//  hgps' instance variables are updated, if the sentence was parseable.
```

###### Module Command

```cpp
gps_err_t GPS::command(const char* sentence);
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
gps_err_t GPS::await_response(const char* sentence, uint8_t timeout = 5);
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
gps_err_t GPS::pause(bool status);
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

#### PROTECTED METHODS

###### Checksum Validator

```cpp
gps_err_t GPS::validate_checksum(char* sentence);
```

This inspects an NMEA sentence for the hexadecimal XOR checksum footer and
ensures that the contents match the checksum. Returns `nocsum` or `badcsum` on error.

##### PARSERS

```cpp
gps_err_t GPS::parse_gga(char* sentence);
gps_err_t GPS::parse_rmc(char* sentence);
```

These methods parse specific NMEA sentence types and store their relevant data
in the instance variables. The master `parse()` method routes sentences to the
appropriate method. They should not be called directly.

These are marked as protected in the event that you want to alter their behavior
in some way, such as storing coordinates as floating-point rather than integral.

```cpp
gps_err_t GPS::parse_time(char* fragment);
gps_err_t GPS::parse_date(char* fragment);
gps_err_t GPS::parse_coord(char* fragment);
gps_err_t GPS::parse_double(double* store, char* fragment, uint8_t precision);
```

These methods are used for parsing specific data fields of a sentence. NMEA
standards are very regular, and processing text into numbers is a relatively
simple, though tedious, matter. Since RMC and GGA have some common fields, it
makes sense to pull out individual field parsing into common subroutines.
Again, *looking at you, Adafruit*.

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
