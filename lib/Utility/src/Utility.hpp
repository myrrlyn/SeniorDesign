#ifndef __APPAYNE_SENIORDESIGN_UTILITY_H
#define __APPAYNE_SENIORDESIGN_UTILITY_H

#include <stdint.h>
#include <Arduino.h>

typedef uint8_t gpio_t;

typedef struct {
	gpio_t tx;
	gpio_t rx;
} iopair_t;

void pinsMode(iopair_t pair);

int16_t uint8x2_to_int16(uint8_t h, uint8_t l);

uint8_t parse_hex(char c);
uint8_t parse_hex(char h, char l);

#ifdef  DEVEL

#define DEBUG_SIZE_1( x ) Serial.println(sizeof(x));
#define DEBUG_SIZE_2(x,y) Serial.println(sizeof(x) - sizeof(y));

#endif//DEVEL

#endif//__APPAYNE_SENIORDESIGN_UTILITY_H
