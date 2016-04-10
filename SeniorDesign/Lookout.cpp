#include <RingBuffer.hpp>
#include <Ultrasonic.hpp>

#include "Pilot.hpp"

#define US_HEAD 0
#define US_TAIL 6

//  Magic numbers setting the critical range of the sensors. Values inside this
//  range indicate an obstruction.
#define US_DISTANCE_MAX 2500
#define US_DISTANCE_MIN  500

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

Ultrasonic* us_all[] = {
	&us00, &us01, &us02, &us03, &us04, &us05,
	&us06, &us07, &us08, &us09, &us10, &us11,
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
		if ((distance < 4000 && distance > 900)) {
			Serial.print("bad  ");
			buf_head.write(false);
		}
		else {
			Serial.print("good ");
			buf_head.write(true);
		}
		Serial.println(distance);
	}
	tmp = buf_head.read_all('&');
	if (tmp) {
		start();
		set_speed(127);
	}
	else {
		full_stop();
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
