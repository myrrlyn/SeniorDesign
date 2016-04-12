#include <RingBuffer.hpp>
#include <Ultrasonic.hpp>

#include "Pilot.hpp"

#define US_HEAD 0
#define US_TAIL 6

//  Magic numbers setting the critical range of the sensors. Values inside this
//  range indicate an obstruction.
#define US_DISTANCE_MAX 3000
#define US_DISTANCE_MIN  500
#define PILOT_SPEED (64 * 3)

Ultrasonic us00({ .tx = 24, .rx = 25 });
Ultrasonic us01({ .tx = 26, .rx = 27 });
Ultrasonic us02({ .tx = 28, .rx = 29 });
Ultrasonic us03({ .tx = 30, .rx = 31 });
Ultrasonic us04({ .tx = 32, .rx = 33 });
Ultrasonic us05({ .tx = 34, .rx = 35 });

Ultrasonic us06({ .tx = 38, .rx = 39 });
Ultrasonic us07({ .tx = 40, .rx = 41 });
Ultrasonic us08({ .tx = 42, .rx = 43 });
Ultrasonic us09({ .tx = 44, .rx = 45 });
Ultrasonic us10({ .tx = 46, .rx = 47 });
Ultrasonic us11({ .tx = 48, .rx = 49 });

RingBuffer_us buf00(true);
RingBuffer_us buf01(true);
RingBuffer_us buf02(true);
RingBuffer_us buf03(true);
RingBuffer_us buf04(true);
RingBuffer_us buf05(true);

RingBuffer_us buf06(true);
RingBuffer_us buf07(true);
RingBuffer_us buf08(true);
RingBuffer_us buf09(true);
RingBuffer_us buf10(true);
RingBuffer_us buf11(true);

Ultrasonic* us_all[] = {
	&us00, &us01, &us02, &us03, &us04, &us05,
	&us06, &us07, &us08, &us09, &us10, &us11,
};

RingBuffer_us* bufs_all[] = {
	&buf00, &buf01, &buf02, &buf03, &buf04, &buf05,
	&buf06, &buf07, &buf08, &buf09, &buf10, &buf11,
};

RingBuffer_us buf_head(true);

void us_debug(uint8_t num) {
	Serial.print("Sensor #");
	Serial.print(num + 1);
	Serial.print(": ");
	Serial.print(us_all[num]->measure());
	Serial.print(" ");
	Serial.print(us_all[num]->measure_inches());
	Serial.println(" inches.");
}

void us_scan_head() {
	register bool tmp;
	for (uint16_t idx = 0; idx < 6; ++idx) {
		uint32_t distance = us_all[US_HEAD + idx]->measure();
		Serial.print("Sensor #");
		Serial.print(idx + 1);
		Serial.print(": ");
		if (distance < US_DISTANCE_MAX &&
distance > US_DISTANCE_MIN) {
			Serial.print("bad  ");
			bufs_all[idx]->write(false);
			buf_head.write(bufs_all[idx]->read_all('+'));
		}
		else {
			Serial.print("good ");
			bufs_all[idx]->write(true);
			buf_head.write(bufs_all[idx]->read_all('+'));
		}
		Serial.println(distance);
	}
	tmp = buf_head.read_all('&');
	if (tmp) {
		pilot.start();
		pilot.set_speed(PILOT_SPEED);
	}
	else {
		pilot.full_stop();
	}
}

void us_scan_tail() {
	for (uint8_t idx = 0; idx < 6; ++idx) {
		us_debug(US_TAIL + idx);
	}
}

void us_scan_all() {
	us_scan_head();
	us_scan_tail();
}
