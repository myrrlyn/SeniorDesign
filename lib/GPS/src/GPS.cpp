#include "GPS.hpp"

#include <Utility.hpp>

GPS::GPS(HardwareSerial* comm) {
	_hwser = comm;
}

GPS::GPS(SoftwareSerial* comm) {
	_swser = comm;
}

gps_err_t GPS::begin(uint16_t baud) {
	if (_hwser != NULL) {
		_hwser->begin(baud);
	}
	else if (_swser != NULL) {
		_swser->begin(baud);
	}
	else {
		return gps_err_nocomm;
	}
	return gps_err_none;
}

char GPS::read() {
	return _hwser->read();
}

gps_err_t GPS::parse(char* sentence) {
	gps_err_t err;
	//  Validate checksum
	uint8_t len = strlen(sentence);
	err = validate_checksum(len, sentence);
	if (err != gps_err_none) {
		return err;
	}
}

void GPS::debug() {
	Serial.println("Communicators:");
	Serial.print("_hwser: ");
	Serial.println((uint16_t)_hwser);
	Serial.print("_swser: ");
	Serial.println((uint16_t)_swser);

	Serial.println("Date/Time:");
	if (timestamp.year < 10) {
		Serial.print('0');
	}
	Serial.print(timestamp.year);
	Serial.print('-');
	if (timestamp.month < 10) {
		Serial.print('0');
	}
	Serial.print(timestamp.month);
	Serial.print('-');
	if (timestamp.day < 10) {
		Serial.print('0');
	}
	Serial.print(timestamp.day);
	Serial.print('T');
	if (timestamp.hour < 10) {
		Serial.print('0');
	}
	Serial.print(timestamp.hour);
	Serial.print(':');
	if (timestamp.minute < 10) {
		Serial.print('0');
	}
	Serial.print(timestamp.minute);
	Serial.print(':');
	if (timestamp.second < 10) {
		Serial.print('0');
	}
	Serial.print(timestamp.second);
	Serial.print('.');
	if (timestamp.millisecond < 100) {
		Serial.print('0');
	}
	if (timestamp.millisecond < 10) {
		Serial.print('0');
	}
	Serial.println(timestamp.millisecond);

	Serial.println("Latitude/Longitude:");
	Serial.println(location.latitude.i);
	Serial.println(location.longitude.i);

	Serial.println("Speed/Heading:");
	Serial.println(velocity.speed);
	Serial.println(velocity.heading);

	Serial.println();
}

//  Must take a string ending in \r\n
gps_err_t GPS::validate_checksum(uint8_t len, char* sentence) {
	return gps_err_nocsum;
}

gps_err_t GPS::parse_rmc(uint8_t len, char* sentence) {
	return gps_err_none;
}
