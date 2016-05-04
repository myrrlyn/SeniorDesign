#include <Ultrasonic.hpp>

extern void us_scan_all(void);
extern void us_scan_head(void);
extern void us_scan_tail(void);

#ifdef DEVEL
extern void us_debug(uint8_t num);
extern void us_debug_all(void);
#endif
