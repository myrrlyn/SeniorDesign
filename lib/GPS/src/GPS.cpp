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
	err = validate_checksum(sentence);
	if (err != gps_err_none) {
		return err;
	}
	if (strstr(sentence, "$GPRMC")) {
		return parse_rmc(sentence);
	}
	return gps_err_noparse;
}

//  Must take a string ending in \r\n\0
gps_err_t GPS::validate_checksum(char* sentence) {
	char* p = strchr(sentence, '*');
	if (p != NULL) {
		uint8_t csum = parse_hex(p[1], p[2]);
		for (uint8_t idx = 1; sentence[idx] != '*'; ++idx) {
			csum ^= sentence[idx];
		}
		if (csum != 0x00) {
			return gps_err_badcsum;
		}
		return gps_err_none;
	}
	return gps_err_nocsum;
}

gps_err_t GPS::parse_rmc(char* sentence) {
	//  Track progress through the sentence
	char* p = sentence;

	//  First up is the timestamp
	p = strchr(p, ',');
	//  There's (almost) always a timestamp, but, if not, fail.
	if (p[1] == ',') {
		return gps_err_nofix;
	}
	parse_time(p);

	//  Seek to next field (active/void fix)
	p = strchr(p + 1, ',');
	if (p[1] == ',' || p[1] == 'V' || p[1] != 'A') {
		return gps_err_nofix;
	}

	//  Seek to next field (latitude)
	p = strchr(p + 1, ',');
	if (p[1] == ',') {
		return gps_err_nofix;
	}
	parse_coord(p);

	//  Skip the hemisphere field (parsed previously)
	p = strchr(p + 1, ',');
	//  Seek to the next field (longitude)
	p = strchr(p + 1, ',');
	if (p[1] == ',') {
		return gps_err_nofix;
	}
	parse_coord(p);

	//  Skip the hemisphere field (parsed previously)
	p = strchr(p + 1, ',');

	//  Seek to the next field (speed)
	p = strchr(p + 1, ',');
	if (p[1] == ',') {
		return gps_err_nofix;
	}
	parse_velocity(p);

	//  Skip the heading field (parsed previously)
	p = strchr(p + 1, ',');

	//  Seek to the next field (date)
	p = strchr(p + 1, ',');
	if (p[1] == ',') {
		return gps_err_nofix;
	}
	parse_date(p);

	//  We do not care about the magnetic variation, and it's optional anyway.

	return gps_err_none;
}

gps_err_t GPS::parse_time(char* fragment) {
	//  This is not easily looped. Hopefully the compiler will optimize out the
	//  redundant stores.
	timestamp.hour         = (fragment[1] - '0') * 10;
	timestamp.hour        += (fragment[2] - '0');
	timestamp.minute       = (fragment[3] - '0') * 10;
	timestamp.minute      += (fragment[4] - '0');
	timestamp.second       = (fragment[5] - '0') * 10;
	timestamp.second      += (fragment[6] - '0');
	timestamp.millisecond  = (fragment[8] - '0') * 100;
	timestamp.millisecond += (fragment[9] - '0') * 10;
	timestamp.millisecond += (fragment[10] - '0');

	return gps_err_none;
}

gps_err_t GPS::parse_date(char* fragment) {
	timestamp.day    = (fragment[1] - '0') * 10;
	timestamp.day   += (fragment[2] - '0');
	timestamp.month  = (fragment[3] - '0') * 10;
	timestamp.month += (fragment[4] - '0');
	timestamp.year   = (fragment[5] - '0') * 10;
	timestamp.year  += (fragment[6] - '0');

	return gps_err_none;
}

gps_err_t GPS::parse_coord(char* fragment) {
	gps_coord_u coord;
	coord.i = 0;

	for (uint8_t idx = 1; fragment[idx] != ','; ++idx) {
		if (fragment[idx] == '.') {
			continue;
		}
		coord.i *= (int32_t)10;
		coord.i += (int32_t)(fragment[idx] - '0');
	}

	//  Seek to the hemisphere field
	fragment = strchr(fragment + 1, ',');

	switch (fragment[1]) {
		case 'N': location.latitude.i = coord.i; break;
		case 'E': location.longitude.i = coord.i; break;
		case 'S': location.latitude.i = -coord.i; break;
		case 'W': location.longitude.i = -coord.i; break;
		case ',': return gps_err_noparse; break;
		default:  return gps_err_nofix; break;
	}

	return gps_err_none;
}

gps_err_t GPS::parse_velocity(char *fragment) {
	velocity.speed = 0.0;
	for (uint8_t idx = 1; fragment[idx] != ','; ++idx) {
		if (fragment[idx] == '.') {
			continue;
		}
		velocity.speed *= 10.0;
		velocity.speed += (double)(fragment[idx] - '0');
	}
	//  GPS module provides two degrees of precision.
	velocity.speed /= 100.0;

	//  Seek to the heading field
	fragment = strchr(fragment + 1, ',');

	velocity.heading = 0.0;
	for (uint8_t idx = 1; fragment[idx] != ','; ++idx) {
		if (fragment[idx] == '.') {
			continue;
		}
		velocity.heading *= 10.0;
		velocity.heading += (double)(fragment[idx] - '0');
	}
	//  GPS module provides two degrees of precision.
	velocity.heading /= 100.0;

	return gps_err_none;
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
