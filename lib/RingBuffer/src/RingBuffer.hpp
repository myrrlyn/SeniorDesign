#ifndef __APPAYNE_SENIORDESIGN_RINGBUFFER_H
#define __APPAYNE_SENIORDESIGN_RINGBUFFER_H

#include <stdint.h>

class RingBuffer_us {
public:
	RingBuffer_us(bool state = false);
	void write(bool val);
	bool read(void);
	bool read_all(char op);
	void set_overwrite(bool state = true);
protected:
	//  Magic number of storage elements. The extra cell is unused and allows
	//  full/empty differentiation.
	static const uint8_t len = 6 + 1;
	bool buf[len];
	bool* head;
	bool* tail;
	bool overwrite;
private:
	bool is_full(void);
	bool is_empty(void);
};

#endif//__APPAYNE_SENIORDESIGN_RINGBUFFER_H
