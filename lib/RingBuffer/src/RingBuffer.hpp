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
	bool is_full(void);
	bool is_empty(void);
private:
	//  Magic number of storage elements. The extra cell is unused and allows
	//  full/empty differentiation.
	static const uint8_t len = 6;
	bool buf[len + 1];
	bool* head;
	bool* tail;
	bool overwrite;
};

class RingBuffer_gps {
public:
	RingBuffer_gps(bool state = false);
	void write(char val);
	char read(void);
	char* read_all(void);
	void set_overwrite(bool state = true);
	void wipe(void);
protected:
	bool is_full(void);
	bool is_empty(void);
private:
	static const uint8_t len = 96;
	char buf[len + 1];
	char* head;
	char* tail;
	bool overwrite;
};

#endif//__APPAYNE_SENIORDESIGN_RINGBUFFER_H
