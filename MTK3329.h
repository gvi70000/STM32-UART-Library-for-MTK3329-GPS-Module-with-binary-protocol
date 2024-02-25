#ifndef __MTK3329_H__
#define __MTK3329_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include <stdio.h>
#include "myDMA.h"
#include "myDelay.h"
	 
typedef struct
{
	int32_t Lattitude;//Latitude (in decimal degrees)Example:23.098572*(10^6)=23098572  
	int32_t Longitude;//Longitude (in decimal degrees)Example:120.2843832*(10^6)=120284383
	int32_t Altitude;//MSL Altitude (meter)Example:34.82*(10^2)=3482
	int32_t Speed;//Ground Speed(m/s)Example:0.324(km/hr) > 0.324*1000/3600 =0.09 (m/s) > 0.09*100=9
	int32_t Heading;//Heading(degrees)Example:123.12 > 123.12*(10^2)=12312
	uint8_t Sats;//Number of visible satelites
	uint8_t Fix;// 0x01 > GPS no fix 0x02 > GPS 2D fix 0x03 > GPS 3D fix
	uint32_t Date;//DDMMYY Example:2010/11/23 =>231110
	uint32_t UTC;//UTC Time Example: 03:35:23.101 > 33523.101 > 33523.101*(10^3) > 33523101
	int16_t HDOP;//Horizontal Dilution of Precision Example:1.15 > 1.15*(10^2)=115
}mtk3329;	 
	 
#define MTK_SET_BAUD_L			"$PMTK251,38400*27\r\n"//Change the baud rate to 115200 bps
#define MTK_SET_BAUD_H			"$PMTK251,115200*1F\r\n"//Change the baud rate to 115200 bps

#define MTK_SET_BINARY		  "$PGCMD,16,0,0,0,0,0*6A\r\n" 
#define MTK_SET_NMEA		    "$PGCMD,16,1,1,1,1,1*6B\r\n"
//reply when changed to binary $PGACK,16,1*68
//reply when changed to NMEA   $PGACK,16,3*6A
#define MTK_OUTPUT_1HZ		  "$PMTK220,1000*1F\r\n"
#define MTK_OUTPUT_2HZ		  "$PMTK220,500*2B\r\n"
#define MTK_OUTPUT_4HZ		  "$PMTK220,250*29\r\n"
#define MTK_OUTPUT_5HZ		  "$PMTK220,200*2C\r\n"
#define MTK_OUTPUT_10HZ		  "$PMTK220,100*2F\r\n"

//Send $PMTK220,200*2C
//Received $PMTK001,220,3*30
//Send $PMTK220,1000*1F
//Received $PMTK001,220,3*30

//#define MTK_BAUD_RATE_38400 "$PMTK251,38400*27\r\n"
//#define MTK_NAVTHRES_OFF 	  "$PMTK397,0*23\r\n"  // Set Nav Threshold (the minimum speed the GPS must be moving to update the position) to 0 m/s

#define MTK_SBAS_ON	        	  "$PMTK313,1*2E\r\n"
#define MTK_SBAS_OFF			      "$PMTK313,0*2F\r\n"
//$PMTK313,1*2E
//$PMTK001,313,3*31
//$PMTK313,0*2F
//$PMTK001,313,3*31
#define MTK_WAAS_ON         	  "$PMTK301,2*2E\r\n"
#define MTK_WAAS_OFF        	  "$PMTK301,0*2C\r\n"
//$PMTK301,2*2E
//$PMTK001,301,3*32
//$PMTK301,0*2C
//$PMTK001,301,3*32
#define PREAMBLE1_V16 0xd0
#define PREAMBLE1     0xdd
#define PREAMBLE2   	0x20
#define MTK_MSG_L   	32

int32_t Bytes2Long(unsigned char Buffer[]);

extern void mtk_init(void);
extern uint8_t mtk_read(mtk3329* GPS_Data);
//void gpsWait(uint8_t loops);

#ifdef __cplusplus
}
#endif

#endif // __MTK3329__
