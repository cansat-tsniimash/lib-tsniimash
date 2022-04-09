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
/*
 * принимает байт с гпски и записывает во внутренний буфер
 */
int gps_push_byte(uint8_t byte);

/*
 * обрабатывает внутренний буфер и парсит его
 */
int gps_work();
/*
 *   функция записывает координаты в переменные по указателю lat_, lon_ и alt_ и возвращает  0
 *   cookie - количество измерений
 */
int gps_get_coords(int64_t * cookie, float * lat_, float * lon_, float * alt_);
/*
 * функция записывает высоту в переменную по указателю alt и возвращает 0
 * cookie - количество измерений
 */
int gps_get_alt(int64_t * cookie, float * alt);
/*
 *  функция записывает время в секундах(по указателю в time_s) и в микросекундах(по указателю в time_us) и возвращает 0
 *  cookie - количество измерений
 */
int gps_get_time(int64_t * cookie, uint64_t * time_s, uint32_t * time_us);








#endif /* NMEA_GPS_H_ */
