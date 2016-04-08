#include "Utility.hpp"

void pinsMode(iopair_t pair) {
	pinMode(pair.tx, OUTPUT);
	pinMode(pair.rx, INPUT);
}

int16_t uint8x2_to_int16(uint8_t l, uint8_t h) {
	int16_t tmp = 0x0000;
	tmp |= l;
	tmp |= (h << 8);
	return tmp;
}

uint8_t parse_hex(char c) {
	register uint8_t tmp = 0x00;
	if (c >= '0' && c <= '9') {
		tmp = c - '0';
	}
	else if (c >= 'A' && c <= 'F') {
		tmp = c - 'A' + 10;
	}
	else if (c >= 'a' && c <= 'f') {
		tmp = c - 'a' + 10;
	}
	return tmp;
}

uint8_t parse_hex(char h, char l) {
//  Most multi-register variables have hardware protections to make atomic reads
//  in little-endian order (i.e. once the low register is read, no register can
//  be written until the high register is read). This code ensures that reads
//  will occur in the proper order, and encourages the compiler to keep the
//  intermediate results in the core rather than using RAM until finished.
	register uint8_t tmp = 0x00;
	tmp  = parse_hex(l);
	tmp |= parse_hex(h) << 4;
	return tmp;
}
