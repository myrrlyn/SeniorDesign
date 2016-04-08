#ifndef __APPAYNE_SENIORDESIGN_GPS_H
#define __APPAYNE_SENIORDESIGN_GPS_H

#include <HardwareSerial.h>
#include <SoftwareSerial.h>

/*  These items are defined as preprocessor macros, rather than as C items, to
 *  ensure they are not present in the compiled result.
 */

//  Defines the interval at which the GPS module prints on TX.
#define GPS_PRINT_10000 "$PMTK220,10000*2F"
#define GPS_PRINT_5000  "$PMTK220,5000*1B"
#define GPS_PRINT_2000  "$PMTK220,2000*1C"
#define GPS_PRINT_1000  "$PMTK220,1000*1F"
#define GPS_PRINT_500   "$PMTK220,500*2B"
#define GPS_PRINT_200   "$PMTK220,200*2C"
#define GPS_PRINT_100   "$PMTK220,100*2F"

//  Defines the interval at which the GPS module performs fix calculations.
#define GPS_FIX_UPDATE_10000 "$PMTK300,10000,0,0,0,0*2C"
#define GPS_FIX_UPDATE_5000  "$PMTK300,5000,0,0,0,0*18"
#define GPS_FIX_UPDATE_2000  "$PMTK300,2000,0,0,0,0*1F"
#define GPS_FIX_UPDATE_1000  "$PMTK300,1000,0,0,0,0*1C"
#define GPS_FIX_UPDATE_500   "$PMTK300,500,0,0,0,0*28"
#define GPS_FIX_UPDATE_200   "$PMTK300,200,0,0,0,0*2F"

//  Defines the baud rate at which the module communicates.
//  9600 is the default set on startup.
#define GPS_COMM_BAUD_57600 "$PMTK251,57600*2C"
#define GPS_COMM_BAUD_9600  "$PMTK251,9600*17"

//  Defines which sentence types are emitted.
#define GPS_OUTPUT_RMC    "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
#define GPS_OUTPUT_GGA    "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
#define GPS_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define GPS_OUTPUT_ALL    "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define GPS_OUTPUT_OFF    "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

//  Enables the use of GPS augmenters
#define GPS_AUGMENT_SBAS "$PMTK313,1*2E"
#define GPS_AUGMENT_WAAS "$PMTK301,2*2E"

//  Defines whether or not antenna debug messages appear
#define GPS_ANTENNA_DEBUG "$PGCMD,33,1*6C"
#define GPS_ANTENNA_QUIET "$PGCMD,33,0*6D"

//  Module firmware version -- shows up as an immediate response, not continuous
#define GPS_FIRMWARE_VER "$PMTK605*31"

//  Standby/wake
#define GPS_SLEEP "$PMTK161,0*28"
#define GPS_AWAKE "$PMTK010,002*2D"

//  Control the LOCUS log functionality
#define GPS_LOCUS_STARTLOG     "$PMTK185,0*22"
#define GPS_LOCUS_STOPLOG      "$PMTK185,1*23"
#define GPS_LOCUS_STARTSTOPACK "$PMTK001,185,3*3C"
#define GPS_LOCUS_QUERY_STATUS "$PMTK183*38"
#define GPS_LOCUS_ERASE_FLASH  "$PMTK184,1*22"
#define GPS_LOCUS_OVERLAP  0
#define GPS_LOCUS_FULLSTOP 1

typedef enum : uint8_t {
	gps_err_none    = 0x00,
	gps_err_nocomm  = 0xFF,
	gps_err_badcsum = 0xFE,
	gps_err_nocsum  = 0xFD,
	gps_err_nofix   = 0xFC,
} gps_err_t;

typedef struct {
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t millisecond;
} gps_time_t;

typedef union {
	double d;
	int32_t i;
} coord_u;

typedef struct {
	coord_u latitude;
	coord_u longitude;
} gps_coord_t;

typedef struct {
	double speed;
	double heading;
} gps_velocity_t;

class GPS {
public:
	GPS(HardwareSerial* comm);
	GPS(SoftwareSerial* comm);
	gps_err_t begin(uint16_t baud);
	char read(void);
	gps_err_t parse(char* sentence);
	void debug(void);

protected:
	gps_err_t validate_checksum(char* sentence);
	gps_err_t parse_rmc(char* sentence);

	HardwareSerial* _hwser = NULL;
	SoftwareSerial* _swser = NULL;

	gps_time_t timestamp;
	gps_coord_t location;
	gps_velocity_t velocity;
};

#endif//__APPAYNE_SENIORDESIGN_GPS_H
