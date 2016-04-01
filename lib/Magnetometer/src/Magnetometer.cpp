#include "Magnetometer.hpp"

#include <math.h>
#include <Wire.h>
#include <Utility.hpp>

#define MAGNETOMETER_I2C_ADDR 0x1E

//  π is the wrong circle constant. Use τ instead. Be the change you want to see
//  in the world. <tau-day.com>
#ifndef M_TAU
#define APPAYNE_M_TAU
#define M_TAU (2 * M_PI);
#endif//M_TAU

Magnetometer::Magnetometer() {
	//  Mark the instance as non-initialized.
	i2c_addr = 0x00;
}

mag_err_t Magnetometer::begin() {
	register mag_err_t err;
	uint8_t buf[3];

	//  Join the Wire bus as a master device.
	Wire.begin();

	//  Check the three identity registers against known truths.
	//  The three identity registers are known to be in sequence, so this works.
	err = i2c_read(mag_reg_ida, buf, 3);
	//  Single checks like this have an appropriate native instruction. It helps
	//  that the success code is conveniently set to be 0x00. JNZ only performs
	//  a jump if err is not 0x00, otherwise instruction continues unaffected.
	if (err != mag_err_none) {
		return err;
	}
	//  In the majority of cases, this should result in three failed checks and
	//  then a missed branch. Since this is specific comparison, not a JNZ, I'm
	//  not sure if it will be optimized or not.
	if ((buf[0] != mag_reg_ida_id)
	||  (buf[1] != mag_reg_idb_id)
	||  (buf[2] != mag_reg_idc_id)) {
		return mag_err_bad_ident;
	}

	//  Set the CFGA register. See the datasheet.
	//  0 01 110 00
	err = i2c_write(mag_reg_cfga, 0x38);
	if (err != mag_err_none) {
		return err;
	}

	//  Set the CFGB register. See the datasheet.
	//  001 00000
	err = i2c_write(mag_reg_cfgb, mag_gain_1090);
	if (err != mag_err_none) {
		return err;
	}

	//  Set the mode register. See the datasheet.
	//  000000 00
	err = i2c_write(mag_reg_mode, 0x00);
	if (err != mag_err_none) {
		return err;
	}

	//  Set the flag to indicate successful initialization.
	this->i2c_addr = MAGNETOMETER_I2C_ADDR;

	return err;
}

mag_err_t Magnetometer::set_gain(mag_gain_t gain) {
	register mag_err_t err;

	//  Check for initialization
	if (this->i2c_addr != MAGNETOMETER_I2C_ADDR) {
		return mag_err_noinit;
	}

	//  Check argument validity
	if ((gain & 0xE0) != gain) {
		return mag_err_wronggain;
	}

	err = i2c_write(mag_reg_cfgb, gain);
	if (err != mag_err_none) {
		return err;
	}
	this->gain = gain;

	//  The hardware requires a new read to set the new gain.
	int16_t tmp;
	err = read_raw(&tmp, &tmp, &tmp);
	if (err != mag_err_none) {
		return err;
	}

	return err;
}

mag_err_t Magnetometer::read_raw(int16_t* x, int16_t* y, int16_t* z) {
	register mag_err_t err;

	//  Check for initialization
	if (this->i2c_addr != MAGNETOMETER_I2C_ADDR) {
		return mag_err_noinit;
	}

	//  Annoying thing about C -- the [] secretly makes tmp a pointer, not a
	//  uint8_t. So we don't use &tmp, we just use tmp.
	uint8_t tmp[6];
	err = i2c_read(mag_reg_data, tmp, 6);
	if (err != mag_err_none) {
		return err;
	}

	//  The hardware is in big-endian. We are in little-endian. As such, byte
	//  pairs in tmp are backwards. Use the functions from Utility to properly
	//  store the retrieved values in the given storage. The Utility function is
	//  in little-endian parameter order, also.
	*x = uint8x2_to_int16(tmp[0], tmp[1]);
	*z = uint8x2_to_int16(tmp[2], tmp[3]);
	*y = uint8x2_to_int16(tmp[4], tmp[5]);

	//  Check the freshly formatted values for overflow failure.
	if (*x == -4096
	||  *y == -4096
	||  *z == -4096) {
		return mag_err_overflow;
	}

	return err;
}

mag_err_t Magnetometer::read_gauss(double* x, double* y, double* z) {
	register mag_err_t err;

	//  Check for initialization
	if (this->i2c_addr != MAGNETOMETER_I2C_ADDR) {
		return mag_err_noinit;
	}

	int16_t tmp[3];

	//  Remember how C is weird about arrays? [] goes the other way here.
	err = read_raw(&tmp[0], &tmp[1], &tmp[2]);
	if (err != mag_err_none) {
		return err;
	}

	double divisor;
	switch (this->gain) {
		case mag_gain_1370: divisor = 1370.0; break;
		case mag_gain_1090: divisor = 1090.0; break;
		case mag_gain_0820: divisor =  820.0; break;
		case mag_gain_0660: divisor =  660.0; break;
		case mag_gain_0440: divisor =  440.0; break;
		case mag_gain_0390: divisor =  390.0; break;
		case mag_gain_0330: divisor =  330.0; break;
		case mag_gain_0230: divisor =  230.0; break;
		default: return mag_err_wronggain;
	}

	*x = (double)tmp[0] / divisor;
	*y = (double)tmp[1] / divisor;
	*z = (double)tmp[2] / divisor;

	return err;
}

mag_err_t Magnetometer::compass(double* heading, mag_axes_t axes) {
	register mag_err_t err;

	//  Check for initialization
	if (this->i2c_addr != MAGNETOMETER_I2C_ADDR) {
		return mag_err_noinit;
	}

	int16_t tmp[3];
	double tanres;

	err = read_raw(&tmp[0], &tmp[1], &tmp[2]);
	if (err != mag_err_none) {
		return err;
	}

	switch (axes) {
		case mag_axes_xy: tanres = atan2(tmp[1], tmp[0]); break;
		case mag_axes_xz: tanres = atan2(tmp[2], tmp[0]); break;
		case mag_axes_yz: tanres = atan2(tmp[2], tmp[1]); break;
		case mag_axes_yx: tanres = atan2(tmp[0], tmp[1]); break;
		case mag_axes_zx: tanres = atan2(tmp[0], tmp[2]); break;
		case mag_axes_zy: tanres = atan2(tmp[1], tmp[2]); break;
	}

	//  Correct for negatives
	if (tanres < 0.0) {
		tanres += M_PI;
	}

	//  Convert from radians to degrees
	*heading = tanres * 360.0 / M_TAU;

	return err;
}

mag_err_t Magnetometer::i2c_read(mag_reg_t reg, uint8_t* data, uint8_t len) {
	//  Check for initialization
	if (this->i2c_addr != MAGNETOMETER_I2C_ADDR) {
		return mag_err_noinit;
	}

	//  Begin I2C transmission
	Wire.beginTransmission(this->i2c_addr);
	//  First byte out is the register address.
	Wire.write((uint8_t)reg);
	//  Read; no followup.
	Wire.endTransmission();
	//  Permit the device to begin transmitting.
	Wire.requestFrom(this->i2c_addr, len);

	//  Read len bytes into data
	for (uint8_t idx = 0; idx < len; ++idx) {
		if (Wire.available()) {
			data[idx] = Wire.read();
		}
		else {
			return mag_err_i2c_read;
		}
	}

	return mag_err_none;
}

mag_err_t Magnetometer::i2c_write(mag_reg_t reg, uint8_t data) {
	//  Check for initialization
	if (this->i2c_addr != MAGNETOMETER_I2C_ADDR) {
		return mag_err_noinit;
	}

	//  Begin I2C transmission
	Wire.beginTransmission(this->i2c_addr);
	//  First byte out is register address.
	Wire.write((uint8_t)reg);
	//  Next byte out is payload.
	Wire.write(data);
	Wire.endTransmission();

	return mag_err_none;
}

//  Clean up after ourselves. Ideology can be good, but pollution is only bad.
#ifdef  APPAYNE_M_TAU
#undef  APPAYNE_M_TAU
#undef  M_TAU
#endif//APPAYNE_M_TAU
