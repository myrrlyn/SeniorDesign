# `Magnetometer` Driver

This class provides a driver/API for a Honeywell HMC5883L 3-axis magnetometer.

The HMC5883L uses the I2C serial communication interface and has a hard-coded
address of `0x1E`. It *may* be possible to alter the I2C address of an
HMC5883L, but I do not know how offhand. As I am only using one, I don't really
care.

## Hardware

The device is printed on a rectangular board, and has three I2C buses. Two are
male pins in plastic housing, designed to accept female 4-wire cables and to
facilitate daisy-chaining this device in series with other I2C devices. It also
has four collinear holes to receive header pins, for mounting to a breadboard.

The measurement axes are defined such that X is along the long axis of the
board, Y is along the short axis, and Z extends outwards from the plane. The top
of the board (with the components, labels, and bus enclosures) is positive Z.

## Operation

For use as a compass, keep it as flat with respect to the earth's surface as
possible, and away from other magnetic fields.

## Software

Use the I2C driver to write to and request reads from the device. It uses a
character stream (like the GPS) rather than simple binary logic (like the
Ultrasonic).

## API

The sensor uses the I2C lines of SDA and SCL. The microcontroller's I2C
controller needs to be initialized before this device can be used. Once that
is done, simply writing from and reading to the I2C controller will suffice for
data transfer. Library functions will produce meaningful results from the
measured data.

All instance methods return a status code. Any methods that retrieve data take
pointers to appropriately-typed storage.

### Types

Four enumerations are defined and contain valid values for interacting with the
device. These enumerations are all of `uint8_t` underlying type.

###### `mag_err_t`

Lists the various error codes that methods may return.

0. `mag_err_none` -- Success
1. `mag_err_noinit` -- The instance was not properly initialized.
2. `mag_err_bad_ident` -- The device did not respond with the expected magic
values.
3. `mag_err_i2c_read` -- Reading from the I2C line failed.
4. `mag_err_i2c_write` -- Writing to the I2C line failed.
5. `mag_err_badentry` -- The user tried to use a `uint8_t` literal and chose one
not in the appropriate enumeration. This is a user fault, not a device fault.
6. `mag_err_wronggain` -- The user tried to set a gain that is invalid. This is
a subtype of `mag_err_badentry`.
7. `mag_err_overflow` -- The sensors were overloaded; increase gain and try
again.

###### `mag_reg_t`

Holds the register addresses and magic numbers used.

0. `mag_reg_cfga` -- The A configuration register.
1. `mag_reg_cfgb` -- The B configuration register.
2. `mag_reg_mode` -- The mode register.
3. `mag_reg_data` -- The register which stores measured data.
4. `mag_reg_ida`  -- The A identification register.
5. `mag_reg_idb`  -- The B identification register.
6. `mag_reg_idc`  -- The C identification register.
7. `mag_reg_idav` -- The expected value of `ida`.
8. `mag_reg_idbv` -- The expected value of `idb`.
9. `mag_reg_idcv` -- The expected value of `idc`.

###### `mag_gain_t`

Selects the different gains against which the magnetometer takes measurements.

0. `mag_gain_1370` -- The magnetometer has a range of ±0.88 Gauss. This maps to
a resolution of 0.73 milliGauss per LSB.
1. `mag_gain_1090` -- Range of ±1.3 Gauss; resolution of 0.92 milliGauss.
2. `mag_gain_0820` -- Range of ±1.9Ga; resolution of 1.22mGa.
3. `mag_gain_0660` -- Range of ±2.5Ga; resolution of 1.52mGa.
4. `mag_gain_0440` -- Range of ±4.0Ga; resolution of 2.27mGa.
5. `mag_gain_0390` -- Range of ±4.7Ga; resolution of 2.56mGa.
6. `mag_gain_0330` -- Range of ±5.6Ga; resolution of 3.03mGa.
7. `mag_gain_0230` -- Range of ±8.1Ga; resolution of 4.35mGa.

###### `mag_axes_t`

Allows the user to select which axes are to be used for computing compass
direction.

0. `mag_axes_xy` -- X is East, Y is North, Z is unused.
1. `mag_axes_xz` -- X is East, Z is North, Y is unused.
2. `mag_axes_yz` -- Y is East, Z is North, X is unused.
4. `mag_axes_yx` -- Y is East, X is North, Z is unused.
4. `mag_axes_zx` -- Z is East, X is North, Y is unused.
5. `mag_axes_zy` -- Z is East, Y is North, X is Unused.

### Public APIs

###### Constructor

```cpp
Magnetometer(void);
```

Constructs a new `Magnetometer` instance. This does NOT initialize the Wire bus
for use, as that must be done at runtime.

```cpp
Magnetometer mag;
mag_err_t mag_err;
```

###### Initializer

```cpp
mag_err_t begin(void);
```

This function must be called during `setup()` and marks the instance as fully
initialized. All instance methods will fail until this is called successfully.

```cpp
mag_err = mag.begin();
//  Will return failure if I2C cannot be properly initialized or if the device
//  responds inappropriately.
```

###### Gain Setting

```cpp
mag_err_t set_gain(mag_gain_t gain);
```

Sets the gain level of the magnetometer and performs a throwaway read as
required by hardware.

```cpp
mag_err = mag.set_gain(mag_gain_0820);
//  Should pretty much always return success.
```

###### Reading Raw Bits

```cpp
mag_err_t read_raw(int16_t* x, int16_t* y, int16_t* z);
```

Retrieves the raw, unaltered bits measured by the device. If any of the values
are -4096, then that sensor was overloaded and the gain should be adjusted
before reading again. The measured values are scaled by the gain setting, so raw
reads will need to normalize the results themselves.

```cpp
int16_t rx, ry, rz;
mag_err = mag.read_raw(&rx, &ry, &rz);
//  Should return success in almost all cases.
//  rx, ry, and rz now store the raw bits reported by the respective sensors.
```

###### Reading Gauss

```cpp
mag_err_t read_gauss(double* x, double* y, double* z);
```

Retrieves the raw measurements from the device and then converts to
floating-point according to the gain value. This performs the normalization
required for raw data processing.

```cpp
double dx, dy, dz;
mag_err = mag.read_gauss(&dx, &dy, &dz);
//  Should return success in almost all cases.
//  Will fail if you messed up the gain setting, but, so does setting invalid
//  gains.
//  dx, dy, and dz now hold the normalized Gauss values from each sensor.
```

###### Compass

```cpp
mag_err_t compass(double* heading, mag_axes_t axes = mag_axes_xy);
```

Provides a value in 0..360 that is the **navigation** heading of the sensor.
This function takes an optional value that allows for the device to be
positioned in one of six orientations for measurement. If the positive halves of
your chosen measurement axes do not point north and east, you're on your own.

```cpp
double nav;
mag_err = mag.compass(&nav);
//  Returns success unless the device cannot respond
//  nav is now a floating-point number between 0 and 360, where 0 means Y is
//  facing North and X is facing East.
mag_err = mag.compass(&nav, mag_axes_yz);
//  nav is now calculated with Z facing North and Y facing East.
```

### Protected APIs

Protected functions for reading and writing on the I2C lines provide relatively
safe means of communicating with the device.

###### Reader

```cpp
mag_err_t i2c_read(mag_reg_t r, uint8_t* d, uint8_t n = 1);
```

Reads `n` number of bytes from register `r` into storage `*d`. Defaults to
reading only a single byte, to try and prevent idiocy such as in the example.

The device will automatically select the next data register after a read, so
simply issuing successive calls to read from I2C will return the proper data
sequence.

```cpp
uint8_t store;
mag_err = mag.i2c_read_bytes(mag_reg_data, &store, 6);
//  This is the magic part of read_raw()
//  This is ALSO a buffer overrun. No memory management, no segfaults. BEHAVE.
```

###### Writer

```c
mag_err_t i2c_write(mag_reg_t r, uint8_t data);
```

Writes into register `r` a single byte of `data`. The writable registers are all
single-byte, so this does not need to accommodate variable-length data.

```c
mag_err_t mag.i2c_write_byte(mag_reg_cfga, 0x00);
//  This does exactly what it looks like.
//  Don't mess with this directly unless you've read the datasheet and are
//  sure you know what it means.
```
