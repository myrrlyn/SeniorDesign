#include "Ultrasonic.hpp"

Ultrasonic::Ultrasonic(iopair_t pins) {
	_pins = pins;
	pinsMode(pins);
}

uint32_t Ultrasonic::measure() {
//  I am assuming that these will be much more often enabled than not, so to
//  optimize branch prediction, I am making enabled the default assumption so
//  that the code path will make a comparison, succeed, and continue apace. The
//  failure and jump will occur only rarely, and so the jump cost will not be
//  very significant.
	if (_status) {
		digitalWrite(_pins.tx, LOW);
		delayMicroseconds(10);
		digitalWrite(_pins.tx, HIGH);
		delayMicroseconds(10);
		digitalWrite(_pins.tx, LOW);
		return pulseIn(_pins.rx, HIGH);
	}
	else {
		return 0x00000000;
	}
}

void Ultrasonic::status(bool status) {
	_status = status;
}

bool Ultrasonic::status() {
	return _status;
}
