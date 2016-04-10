#ifndef __APPAYNE_SENIORDESIGN_PILOT_H
#define __APPAYNE_SENIORDESIGN_PILOT_H

#include <stdint.h>

extern void start(void);
extern void full_stop(void);
extern void set_speed(uint8_t new_speed);

extern void debug_motors(void);

#endif//__APPAYNE_SENIORDESIGN_PILOT_H
