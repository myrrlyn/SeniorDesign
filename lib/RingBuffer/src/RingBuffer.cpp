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
	for (uint8_t idx = 1; idx < len; ++idx) {
		switch (op) {
			case '^': tmp ^= buf[idx]; break;
			case '|': tmp |= buf[idx]; break;
			case '&':
			default:  tmp &= buf[idx]; break;
		}
	}
	head = buf;
	tail = buf;
	return tmp;
}

void RingBuffer_us::set_overwrite(bool state) {
	overwrite = state;
}

bool RingBuffer_us::is_full() {
	//  head points to [0] and tail points to [len - 1], or [*][tail][head][*]
	if (tail + 1 == head || (head == buf && tail == buf + len - 1)) {
		return true;
	}
	return false;
}

bool RingBuffer_us::is_empty() {
	return (head == tail);
}
