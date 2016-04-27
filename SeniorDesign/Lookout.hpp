#include <Ultrasonic.hpp>

extern Ultrasonic* us_all[];

extern void us_scan_all(void);
extern void us_scan_head(void);
extern void us_scan_tail(void);

#ifdef DEVEL
extern void us_debug_all(void);
#endif
