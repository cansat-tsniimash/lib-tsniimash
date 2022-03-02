/*
 * nmea_gps.h
 *
 *  Created on: Feb 12, 2022
 *      Author: Install
 */

#ifndef NMEA_GPS_H_
#define NMEA_GPS_H_

#include <stdint.h>

int gps_init();

int gps_push_byte(uint8_t byte);


int gps_work();


int gps_get_coords(int64_t * cookie, float * lat_, float * lon_, float * alt_);
int gps_get_alt(int64_t * cookie, float * alt);
int gps_get_time(int64_t * cookie, uint64_t * time_s, uint32_t * time_us);








#endif /* NMEA_GPS_H_ */
