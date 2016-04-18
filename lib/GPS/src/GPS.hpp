#ifndef __APPAYNE_SENIORDESIGN_GPS_H
#define __APPAYNE_SENIORDESIGN_GPS_H

#include <HardwareSerial.h>
#ifdef __AVR__
#include <SoftwareSerial.h>
#endif

#include <RingBuffer.hpp>

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
	gps_err_noparse = 0xFB,
	gps_err_nochar  = 0xFA,
	gps_err_timeout = 0xEF,
	gps_msg_ready   = 0x9F,
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
} gps_coord_u;

typedef struct {
	gps_coord_u latitude;
	gps_coord_u longitude;
} gps_coord_t;

typedef struct {
	double speed;
	double heading;
} gps_velocity_t;

typedef enum : uint8_t {
	gps_fix_invalid = 0,
	gps_fix_gps     = 1,
	gps_fix_dgps    = 2,
} gps_fix_t;

class GPS {
public:
	GPS(HardwareSerial* hwser);
#ifdef __AVR__
	GPS(SoftwareSerial* swser);
#endif
	gps_err_t begin(uint16_t baud = 9600);
	gps_err_t store_stream(void);
	bool sentence_ready(void);
	char* latest_sentence(void);
	gps_err_t parse(char* sentence);

	gps_err_t command(const char* sentence);
	gps_err_t await_response(const char* sentence, uint8_t timeout = 5);
	gps_err_t pause(bool status);

	gps_time_t timestamp(void);
	gps_coord_t location(void);
	double hdop(void);
	double altitude(bool sea_or_wgs84 = false);
	gps_velocity_t velocity(void);
	gps_fix_t fix_info(void);
	uint8_t satellites(void);
#ifdef DEVEL
	void debug(void);
#endif
protected:
	bool available(void);
	char read(void);
	gps_err_t store(char inbound);
	gps_err_t validate_checksum(char* sentence);
	gps_err_t parse_gga(char* sentence);
	gps_err_t parse_rmc(char* sentence);
	gps_err_t parse_time(char* fragment);
	gps_err_t parse_date(char* fragment);
	gps_err_t parse_coord(char* fragment);
	gps_err_t parse_double(double* store, char* fragment);
private:
	HardwareSerial* _hwser = NULL;
#ifdef __AVR__
	SoftwareSerial* _swser = NULL;
#endif

	RingBuffer_gps buf_0;
	RingBuffer_gps buf_1;

	gps_time_t _timestamp = {
		.year = 0,
		.month = 0,
		.day = 0,
		.hour = 0,
		.minute = 0,
		.second = 0,
	};
	gps_coord_t _location = {
		.latitude  = { .i = 0 },
		.longitude = { .i = 0 },
	};
	double _hdop = 0.0;
	double alt_sea = 0.0;
	double alt_wgs84 = 0.0;
	gps_velocity_t _velocity = {
		.speed = 0.0,
		.heading = 0.0,
	};
	gps_fix_t _fix_info = gps_fix_invalid;
	uint8_t _satellite_count = 0;

	bool is_asleep = false;
	bool is_sentence_ready = false;
	RingBuffer_gps* buf_active = &buf_0;
	RingBuffer_gps* buf_stable = &buf_1;
};

#endif//__APPAYNE_SENIORDESIGN_GPS_H
