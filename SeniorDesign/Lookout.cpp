#include <Ultrasonic.hpp>

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


Ultrasonic* us_all[12] = {
	&us00, &us01, &us02, &us03, &us04, &us05,
	&us06, &us07, &us08, &us09, &us10, &us11,
};
Ultrasonic* us_head[1] = { &us00, };
Ultrasonic* us_tail[1] = { &us06, };

void us_debug(uint8_t num) {
	Serial.print("Sensor #");
	Serial.print(num + 1);
	Serial.print(": ");
	Serial.println(us_all[num]->measure());
}

void us_scan_head() {
	for (uint8_t idx = 0; idx < 6; ++idx) {
		us_debug(idx);
	}
}

void us_scan_tail() {
	for (uint8_t idx = 6; idx < 12; ++idx) {
		us_debug(idx);
	}
}

void us_scan_all() {
	us_scan_head();
	us_scan_tail();
}
