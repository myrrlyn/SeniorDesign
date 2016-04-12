#include "RingBuffer.hpp"

RingBuffer_us::RingBuffer_us(bool state) {
	//  Initialize both pointers to the first cell.
	head = buf;
	tail = buf;
	//  Set overwrite status.
	overwrite = state;
}

void RingBuffer_us::write(bool val) {
	//  If overwrite is set, we don't care about fullness.
	//  If overwrite is off, is_full must return false to continue.
	if (overwrite || !is_full()) {
		//  Store val in tail
		*tail = val;
		//  Bump tail to point at the next slot (or wrap).
		//  Wrap happens when tail points at buf[len], which does not exist.
		++tail;
		if (tail >= buf + len) {
			tail = buf;
		}
	}
}

bool RingBuffer_us::read() {
	if (!is_empty()) {
		bool tmp = *head;
		++head;
		if (head >= buf + len) {
			head = buf;
		}
		return tmp;
	}
	return false;
}

//  This permits us to smash all the stored values together using the given
//  operation and return the result.
bool RingBuffer_us::read_all(char op) {
	if (!overwrite && is_empty()) {
		return false;
	}
	bool tmp =  buf[0];
	uint8_t sum = (uint8_t)buf[0];
	for (uint8_t idx = 1; idx < len; ++idx) {
		switch (op) {
			case '+': sum += (uint8_t)buf[idx]; return (sum > len / 2); break;
			case '^': tmp ^= buf[idx]; break;
			case '|': tmp |= buf[idx]; break;
			case '&':
			default:  tmp &= buf[idx]; break;
		}
	}
	return tmp;
}

void RingBuffer_us::set_overwrite(bool state) {
	overwrite = state;
}

bool RingBuffer_us::is_full() {
	//  head points to [0] and tail points to [len - 1], or [*][tail][head][*]
	if (tail + 1 == head || (head == buf && tail == buf + len)) {
		return true;
	}
	return false;
}

bool RingBuffer_us::is_empty() {
	return (head == tail);
}

RingBuffer_gps::RingBuffer_gps(bool state) {
	head = buf;
	tail = buf;
	overwrite = state;
	wipe();
}

void RingBuffer_gps::write(char val) {
	*tail = val;
	if (overwrite || !is_full()) {
		++tail;
		if (tail >= buf + len) {
			tail = buf;
		}
	}
}

char RingBuffer_gps::read() {
	if (!is_empty()) {
		char tmp = *head;
		++head;
		if (head >= buf + len) {
			head = buf;
		}
		return tmp;
	}
	return 0x00;
}

char* RingBuffer_gps::read_all() {
	return head;
}

void RingBuffer_gps::set_overwrite(bool state) {
	overwrite = state;
}

void RingBuffer_gps::wipe() {
	for (uint8_t idx = 0; idx < len; ++idx) {
		buf[idx] = 0x00;
	}
	head = buf;
	tail = buf;
}

bool RingBuffer_gps::is_full() {
	if (tail + 1 == head || (head == buf && tail == buf + len)) {
		return true;
	}
	return false;
}

bool RingBuffer_gps::is_empty() {
	return (head == tail);
}
