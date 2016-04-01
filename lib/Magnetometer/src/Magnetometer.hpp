#ifndef __APPAYNE_SENIORDESIGN_MAGNETOMETER_H
#define __APPAYNE_SENIORDESIGN_MAGNETOMETER_H

#include <stdint.h>
//  I don't know why, but PlatformIO doesn't know where to find <Wire.h> while
//  compiling the cpp file, but DOES while compiling this header. Commenting out
//  this #include causes a failure, despite the fact that the sister source file
//  has this exact same line in it. Leaving this line in the cpp file doesn't
//  cause a failure when the #include is present HERE, which makes even less
//  sense. I don't get C/C++ build environments. Go away, 1970s systems.
#include <Wire.h>

typedef enum : uint8_t {
	mag_err_none      = 0x00,
	mag_err_noinit    = 0x82,
	mag_err_bad_ident = 0x81,
	mag_err_i2c_read  = 0x42,
	mag_err_i2c_write = 0x41,
	mag_err_badentry  = 0x22,
	mag_err_wronggain = 0x21,
	mag_err_overflow  = 0x10,
} mag_err_t;

typedef enum : uint8_t {
	mag_gain_1370 = 0x00,  //  range ±0.88Ga, 0.73mGa/LSB
	mag_gain_1090 = 0x20,  //  range ±1.3Ga,  0.92mGa/LSB
	mag_gain_0820 = 0x40,  //  range ±1.9Ga,  1.22mGa/LSB
	mag_gain_0660 = 0x60,  //  range ±2.5Ga,  1.52mGa/LSB
	mag_gain_0440 = 0x80,  //  range ±4.0Ga,  2.27mGa/LSB
	mag_gain_0390 = 0xA0,  //  range ±4.7Ga,  2.56mGa/LSB
	mag_gain_0330 = 0xC0,  //  range ±5.6Ga,  3.03mGa/LSB
	mag_gain_0230 = 0xE0,  //  range ±8.1Ga,  4.35mGa/LSB
} mag_gain_t;

typedef enum : uint8_t {
	mag_reg_cfga   = 0x00,
	mag_reg_cfgb   = 0x01,
	mag_reg_mode   = 0x02,
	mag_reg_data   = 0x03,
	mag_reg_ida    = 0x0A,
	mag_reg_idb    = 0x0B,
	mag_reg_idc    = 0x0C,
	mag_reg_ida_id = 0x48,
	mag_reg_idb_id = 0x34,
	mag_reg_idc_id = 0x33,
} mag_reg_t;

typedef enum : uint8_t {
	mag_axes_xy,
	mag_axes_xz,
	mag_axes_yz,
	mag_axes_yx,
	mag_axes_zx,
	mag_axes_zy,
} mag_axes_t;

class Magnetometer {
public:
	Magnetometer(void);
	mag_err_t begin(void);
	mag_err_t set_gain(mag_gain_t gain);
	mag_err_t read_raw(int16_t* x, int16_t* y, int16_t* z);
	mag_err_t read_gauss(double* x, double* y, double* z);
	mag_err_t compass(double* heading, mag_axes_t axes = mag_axes_yx);

protected:
	mag_err_t i2c_read(mag_reg_t reg, uint8_t* data, uint8_t len = 1);
	mag_err_t i2c_write(mag_reg_t reg, uint8_t data);

private:
	uint8_t i2c_addr;
	mag_gain_t gain;
};

#endif//__APPAYNE_SENIORDESIGN_MAGNETOMETER_H
