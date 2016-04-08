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

## Appendix

This contains miscellaneous information related to the operation of the GPS
module.

### NMEA Sentences

The module is capable of emitting various NMEA sentences. This library provides
semantic parsing for some of them. The most recent parsed information is stored
in the respective instance variable. All fields are separated by commas. No
comma precedes the checksum.

#### RMC

Recommended Minimum Certainty. This sentence contains a date/time stamp,
location, and calculated speed and heading, as well as some extra information.
The magnetic variation field is optional. We do not parse it at this time.

###### Fields

1. `$GPRMC` -- Sentence header.
2. `/[0-9]{6}\.[0-9]{3}/` -- UTC timestamp in HHMMSS.sss.
3. `/[AV]/` -- Fix status. V for Void (no fix), A for Active (fix).
4. `/[0-9]{4}\.[0-9]{4}/` -- Latitude in HHMM.mmmm.
5. `/[NS]/` -- Hemisphere. N for North, S for South.
6. `/[0-9]{5}\.[0-9]{4}/` -- Longitude in HHHMM.mmmm.
7. `/[EW]/` -- Hemisphere. E for East, W for West.
8. `/[0-9]+\.[0-9]{2}/` -- Speed in knots.
9. `/[0-9]+\.[0-9]{2}/` -- Heading in degrees true.
10. `/[0-9]{6}/` -- UTC datestamp in DDMMYY.
11. `/[0-9]+\.[0-9]/` -- Magnetic variation (optional).
12. `/[EW]/` -- Magnetic variation direction (optional).
13. `*[0-9A-F]{2}` -- XOR checksum of all bytes between `$` and `*`,
non-inclusive.
