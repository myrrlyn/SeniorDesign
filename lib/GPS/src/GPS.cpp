#include "GPS.hpp"

#include <Utility.hpp>

GPS::GPS(HardwareSerial* hwser) {
	_hwser = hwser;
}

GPS::GPS(SoftwareSerial* swser) {
	_swser = swser;
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

bool GPS::available() {
	if (_hwser != NULL) {
		return _hwser->available();
	}
	else if (_swser != NULL) {
		return _swser->available();
	}
	else {
		return false;
	}
}

char GPS::read() {
	register char c = -1;
	if (is_asleep || !available()) {
		return c;
	}
	if (_hwser != NULL) {
		c = _hwser->read();
	}
	else if (_swser != NULL) {
		c = _swser->read();
	}
	return c;
}

gps_err_t GPS::store(char inbound) {
	if (inbound == -1) {
		return gps_err_nochar;
	}
	//  If the buffer is full (['*']['A']['B'][0x00]), the last cell (['B'])
	//  will silently be overwritten until the stream stops. Since we can't
	//  allocate extra memory on the fly, we give up on that sentence and wait
	//  for a NULL. The parser will catch it and throw it away.
	buf_active->write(inbound);
	// UDR0 = inbound;
	if (inbound == '\n') {
		//  Sentence was completed; swap buffer roles and set flag.
		// Serial.print((uint16_t)buf_active, HEX);
		// Serial.print(":\t");
		// Serial.println(buf_active->read_all());
		RingBuffer_gps* tmp = buf_active;
		buf_active = buf_second;
		buf_second = tmp;
		is_sentence_ready = true;
		buf_active->wipe();
		return gps_msg_ready;
	}

	return gps_err_none;
}

bool GPS::sentence_ready() {
	return is_sentence_ready;
}

char* GPS::latest_sentence() {
	is_sentence_ready = false;
	return buf_second->read_all();
}

gps_err_t GPS::parse(char* sentence) {
	gps_err_t err;
	//  Validate checksum
	err = validate_checksum(sentence);
	if (err != gps_err_none) {
		return err;
	}
	if (strstr(sentence, "$GPGGA")) {
		return parse_gga(sentence);
	}
	if (strstr(sentence, "$GPRMC")) {
		return parse_rmc(sentence);
	}
	return gps_err_noparse;
}

gps_err_t GPS::command(const char* sentence) {
	if (_hwser != NULL) {
		_hwser->println(sentence);
	}
	else if (_swser != NULL) {
		_swser->println(sentence);
	}
	else {
		return gps_err_nocomm;
	}
	return gps_err_none;
}

gps_err_t GPS::await_response(const char* sentence, uint8_t timeout) {
	//  This is being used as a linear buffer
	RingBuffer_gps buf(false);
	bool sentence_complete = false;
	//  Tracks /sentences received/ not time.
	uint8_t idx = 0;
	while (idx < timeout) {
		//  Accumulate sentence characters...
		if (available()) {
			char c = read();
			buf.write(c);
			if (c == 0x00) {
				++idx;
				sentence_complete = true;
			}
		}
		//  If a full sentence has arrived, judge it
		if (sentence_complete) {
			sentence_complete = false;
			//  read_all() in this case just returns head, so that strstr can
			//  walk the buffer normally. Note that strstr will not obey ring
			//  behavior, so this buffer must be used linearly.
			//  If it matches our target sentence, we can exit.
			if (strstr(buf.read_all(), sentence)) {
				return gps_err_none;
			}
			//  Otherwise, reset and try try again.
			buf.wipe();
		}
	}
	//  Eventually, give up.
	return gps_err_timeout;
}

gps_err_t GPS::pause(bool status) {
	//  We should only act if the new status differs from the old
	if (is_asleep != status) {
		return gps_err_none;
	}
	is_asleep = status;
	if (is_asleep) {
		return command(GPS_SLEEP);
	}
	else {
		//  Sending an empty message suffices to wake the device.
		command("");
		//  Await success response
		return await_response(GPS_AWAKE);
	}
}

gps_time_t GPS::timestamp() {
	return _timestamp;
}

gps_coord_t GPS::location() {
	return _location;
}

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

gps_err_t GPS::parse_gga(char* sentence) {
	char *p = sentence;

	//  First comes timestamp
	p = strchr(p, ',');
	if (p[1] == ',') {
		return gps_err_nofix;
	}
	parse_time(p);

	//  Next are latitude and longitude
	p = strchr(p + 1, ',');
	if (p[1] == ',') {
		return gps_err_nofix;
	}
	parse_coord(p);
	//  Skipping the hemisphere field...
	p = strchr(p + 1, ',');

	//  Now seek to longitude
	p = strchr(p + 1, ',');
	parse_coord(p);
	//  Skipping the hemisphere field...
	p = strchr(p + 1, ',');

	//  Fix information
	p = strchr(p + 1, ',');
	if (p[1] == ',') {
		return gps_err_nofix;
	}
	fix_info = (gps_fix_t)(p[1] - '0');

	//  Satellite count
	p = strchr(p + 1, ',');
	if (p[1] == ',') {
		return gps_err_nofix;
	}
	satellite_count = 0;
	for (uint8_t idx = 1; p[idx] != ','; ++idx) {
		satellite_count *= 10;
		satellite_count += p[idx] - '0';
	}

	//  Horizontal Dilution of Precision
	p = strchr(p + 1, ',');
	if (p[1] == ',') {
		return gps_err_nofix;
	}
	hdop = 0.0;
	parse_double(&hdop, p, 2);

	//  Altitude
	p = strchr(p + 1, ',');
	if (p[1] == ',') {
		return gps_err_nofix;
	}
	parse_double(&alt_sea, p, 1);
	//  Skip the M
	p = strchr(p + 1, ',');
	//  WGS84 altitude
	p = strchr(p + 1, ',');
	if (p[1] == ',') {
		return gps_err_nofix;
	}
	parse_double(&alt_wgs84, p, 1);
	//  Skip the M
	p = strchr(p + 1, ',');

	//  We don't need the DGPS information here.

	return gps_err_none;
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
	parse_double(&velocity.speed, p);

	//  Seek to the next field (heading)
	p = strchr(p + 1, ',');
	if (p[1] == ',') {
		return gps_err_nofix;
	}
	parse_double(&velocity.heading, p);

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
	_timestamp.hour         = (fragment[1] - '0') * 10;
	_timestamp.hour        += (fragment[2] - '0');
	_timestamp.minute       = (fragment[3] - '0') * 10;
	_timestamp.minute      += (fragment[4] - '0');
	_timestamp.second       = (fragment[5] - '0') * 10;
	_timestamp.second      += (fragment[6] - '0');
	_timestamp.millisecond  = (fragment[8] - '0') * 100;
	_timestamp.millisecond += (fragment[9] - '0') * 10;
	_timestamp.millisecond += (fragment[10] - '0');

	return gps_err_none;
}

gps_err_t GPS::parse_date(char* fragment) {
	_timestamp.day    = (fragment[1] - '0') * 10;
	_timestamp.day   += (fragment[2] - '0');
	_timestamp.month  = (fragment[3] - '0') * 10;
	_timestamp.month += (fragment[4] - '0');
	_timestamp.year   = (fragment[5] - '0') * 10;
	_timestamp.year  += (fragment[6] - '0');

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
		case 'N': _location.latitude.i = coord.i; break;
		case 'E': _location.longitude.i = coord.i; break;
		case 'S': _location.latitude.i = -coord.i; break;
		case 'W': _location.longitude.i = -coord.i; break;
		case ',': return gps_err_noparse; break;
		default:  return gps_err_nofix; break;
	}

	return gps_err_none;
}

gps_err_t GPS::parse_double(double* store, char* fragment, uint8_t precision) {
	*store = 0.0;
	bool neg = false;
	for (uint8_t idx = 1; fragment[idx] != ','; ++idx) {
		switch (fragment[idx]) {
			case '-': neg = ~neg;  //  Intentional fallthrough
			case '.': continue;
		}
		*store *= 10.0;
		*store += fragment[idx] - '0';
	}
	for (uint8_t idx = 0; idx < precision; ++idx) {
		*store /= 10.0;
	}
	if (neg) {
		*store *= -1.0;
	}

	return gps_err_none;
}

void GPS::debug() {
	Serial.println("Date/Time:");
	if (_timestamp.year < 10) {
		Serial.print('0');
	}
	Serial.print(_timestamp.year);
	Serial.print('-');
	if (_timestamp.month < 10) {
		Serial.print('0');
	}
	Serial.print(_timestamp.month);
	Serial.print('-');
	if (_timestamp.day < 10) {
		Serial.print('0');
	}
	Serial.print(_timestamp.day);
	Serial.print('T');
	if (_timestamp.hour < 10) {
		Serial.print('0');
	}
	Serial.print(_timestamp.hour);
	Serial.print(':');
	if (_timestamp.minute < 10) {
		Serial.print('0');
	}
	Serial.print(_timestamp.minute);
	Serial.print(':');
	if (_timestamp.second < 10) {
		Serial.print('0');
	}
	Serial.print(_timestamp.second);
	Serial.print('.');
	if (_timestamp.millisecond < 100) {
		Serial.print('0');
	}
	if (_timestamp.millisecond < 10) {
		Serial.print('0');
	}
	Serial.println(_timestamp.millisecond);

	Serial.println("Latitude/Longitude:");
	Serial.println((double)_location.latitude.i / 100.0);
	Serial.println((double)_location.longitude.i / 100.0);

	Serial.println("Altitudes (Sea, WGS84):");
	Serial.println(alt_sea);
	Serial.println(alt_wgs84);

	Serial.println("Horizontal Dilution of Precision:");
	Serial.println(hdop);

	Serial.println("Speed/Heading:");
	Serial.println(velocity.speed);
	Serial.println(velocity.heading);

	Serial.println("Fix Quality:");
	Serial.println(fix_info);

	Serial.println("Satellites:");
	Serial.println(satellite_count);

	Serial.println();
}
