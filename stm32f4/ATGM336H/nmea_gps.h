/*
Этот драйвер упрощает работу с NMEA протоколом:
1) Парсит пакеты NMEA
2) Сохраняет полученные значения координат и времени внутри драйвера,
которые можно забрать в любое время


Основная идея этого драйвера заключается в том, что пользователь
получает поток байтов из какого-нибудь канала и закидывает байтики в драйвер
с помощью gps_push_byte. Эти байтики накапливаются во внутреннем кольцевом
буфере. Чтобы он не переполнился, время от времени должна вызываться gps_work,
которая парсит пакеты NMEA и сохраняет данные от GNSS внутри драйвера. Эти
данные можно получить через gps_get_coords, gps_get_alt и gps_get_time.

Вот условный пример работы:

...

gps_init();

...

while (1) {
    while (uart_has_data()) {
        uint8_t byte = uart_get_byte();
        gps_push_byte(byte);
    }
    gps_work();

    ...

    float lon = 0, lat = 0, alt = 0;
    int64_t cookie = 0;
    gps_get_coords(&cookie, &lon, &lat, &alt);

    printf("Update count: %d Lon: %f Lat: %f Alt: %f", cookie, lon, lat, alt);

    ...

 */

#ifndef NMEA_GPS_H_
#define NMEA_GPS_H_

#include <stdint.h>
#include "minmea/minmea.h"

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
int gps_get_coords(int64_t * cookie, float * lat_, float * lon_, float * alt_, int *fix_);
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
/*
 * функция возвращает сам gga sentence
 *
 */
int gps_get_gga(int64_t * cookie, struct minmea_sentence_gga * gga_);




#endif /* NMEA_GPS_H_ */
