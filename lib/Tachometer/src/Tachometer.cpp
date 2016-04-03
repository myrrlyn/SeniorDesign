#include "Tachometer.hpp"

Tachometer::Tachometer(gpio_t pin, double magic_number;) {
	this->pin = pin;
	this->magic_number = magic_number;
	pinMode(pin, INPUT);
	this->state = false;
}

void Tachometer::begin() {
	reset();
	this->state = true;
}

bool Tachometer::read() {
	if (this->state == true) {
		if (this->mag_pole != (mag_pole_t)digitalRead(this->pin)) {
			this->delta = millis() - this->time;
			this->current_rpm = calc_rpm(this->delta);
			reset();
			return true;
		}
	}
	return false;
}

uint32_t Tachometer::interval() {
	if (this->state == true) {
		return this->delta;
	}
	return false;
}

double Tachometer::rpm() {
	if (this->state == true) {
		return this->current_rpm;
	}
	return 0.0;
}

void Tachometer::reset() {
	this->mag_pole = digitalRead(this->pin);
	this->time = millis();
}

double calc_rpm(uint32_t interval) {
	if (this->state == true) {
		return this->magic_number / (double)delta;
	}
	return 0.0;
}
