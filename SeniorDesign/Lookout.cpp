#include <RingBuffer.hpp>
#include <Ultrasonic.hpp>

#include "Pilot.hpp"

#define US_HEAD 0
#define US_TAIL 6
#define US_ALL 12

//  Magic numbers setting the critical range of the sensors. Values inside this
//  range indicate an obstruction.
#define US_DISTANCE_MAX 5000
#define US_DISTANCE_MIN  500
#define US_DISTANCE_MAX_IN 72
#define US_DISTANCE_MIN_IN  8
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

#ifdef DEVEL
void us_debug(uint8_t num) {
	Serial.print("Sensor #");
	Serial.print(num + 1);
	Serial.print(": ");
	Serial.print(us_all[num]->measure());
	Serial.print(" ");
	Serial.print(us_all[num]->measure_inches());
	Serial.println(" inches.");
}

void us_debug_all() {
	Serial.println(buf_head.read_all('&') ? "Sensors clear" : "Sensors blocked");
}
#endif

void us_scan(uint8_t sensor, bool* imm) {
	uint32_t distance = us_all[sensor]->measure();
	if (distance < US_DISTANCE_MAX && distance > US_DISTANCE_MIN) {
		*imm = false;
	}
	else {
		*imm = true;
	}
	bufs_all[sensor]->write(*imm);
	buf_head.write(bufs_all[sensor]->read_all('+'));
#ifdef DEVEL
	// us_debug(sensor);
#endif
}

uint8_t head_tracker = 0;

void us_scan_head() {
	register bool immediate;
	us_scan(US_HEAD + head_tracker, &immediate);
	if (buf_head.read_all('&')) {
		pilot.start();
	}
	else {
		pilot.halt();
	}
	++head_tracker;
	head_tracker %= US_TAIL;
}

uint8_t tail_tracker = 0;

void us_scan_tail() {
	register bool immediate;
	// us_debug(US_TAIL + tail_tracker);
	us_scan(US_TAIL + tail_tracker, &immediate);
	++tail_tracker;
	tail_tracker %= (US_ALL - US_TAIL);
}

void us_scan_all() {
	us_scan_head();
	us_scan_tail();
}
