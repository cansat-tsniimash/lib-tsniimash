/*
 * nmea_gps.c
 *
 *  Created on: Feb 12, 2022
 *      Author: Install
 */

#include "../nmea_gps.h"
#include "../minmea/minmea.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

static int fix;
static uint8_t uart_buffer[1000] = { 0 };
static size_t uart_buffer_head = 0;
static size_t uart_buffer_tail = 0;
static int pavel_gps_sost = 0;
static char pavel_gps_buffer[300];
static int pavel_gps_carret = 0;

static int64_t pos_cookie;
static float lat;
static float lon;

static int64_t alt_cookie;
static float alt;

static int64_t time_cookie;
static uint64_t time_s;
static uint32_t time_us;

static struct minmea_sentence_gga gga;


static uint16_t schetchik = 0;


#define GPS_SYMBOLS_PER_ACTS 500

static bool cb_push(uint8_t byte)
{

	size_t next_head = uart_buffer_head + 1;
	if (next_head >= sizeof(uart_buffer) / sizeof(uart_buffer[0]))
		next_head = 0;

	if (next_head == uart_buffer_tail)
		return false;

	uart_buffer[uart_buffer_head] = byte;
	uart_buffer_head = next_head;
	return true;
}


static bool cb_pop(uint8_t *byte)
{
	if (uart_buffer_head == uart_buffer_tail)
		return false;

	*byte = uart_buffer[uart_buffer_tail];
	uart_buffer_tail += 1;
	if (uart_buffer_tail >= sizeof(uart_buffer) / sizeof(uart_buffer[0]))
		uart_buffer_tail = 0;

	return true;
}




static int gps_parse(uint8_t byte)
{
	if (pavel_gps_sost == 0)
	{
		if (byte == '$')
		{
			pavel_gps_buffer[0]='$';
			pavel_gps_sost = 1;
			pavel_gps_carret = 1;
		}
	}
	else
	{
		if (pavel_gps_carret >= sizeof(pavel_gps_buffer) / sizeof(pavel_gps_buffer[0]))
		{
			pavel_gps_sost = 0;
			return pavel_gps_sost;
		}

		pavel_gps_buffer[pavel_gps_carret] = byte;
		pavel_gps_carret = pavel_gps_carret + 1;
		if (byte == '\n' )
		{
			pavel_gps_sost = 2;
			pavel_gps_buffer[pavel_gps_carret] =0;
		}
	}

	return pavel_gps_sost;
}


int gps_init()
{
	memset(uart_buffer, 0x00, sizeof(uart_buffer));
	uart_buffer_head = 0;
	uart_buffer_tail = 0;
	pavel_gps_sost = 0;

	pavel_gps_carret = 0;
	return 0;
}


int gps_push_byte(uint8_t byte)
{
	cb_push(byte);
	return 0;
}

int on_gps_byte(uint32_t byte)
{
	if (pavel_gps_sost == 0)
		{
			if (byte == '$')
			{
				pavel_gps_buffer[0]='$';
				pavel_gps_sost = 1;
				pavel_gps_carret = 1;
			}
		}
		else
		{
			if (pavel_gps_carret >= sizeof(pavel_gps_buffer) / sizeof(pavel_gps_buffer[0]))
			{
				pavel_gps_sost = 0;
				return pavel_gps_sost;
			}

			pavel_gps_buffer[pavel_gps_carret] = byte;
			pavel_gps_carret = pavel_gps_carret + 1;
			if (byte == '\n' )
			{
				pavel_gps_sost = 2;
				pavel_gps_buffer[pavel_gps_carret] =0;
			}
		}

		return pavel_gps_sost;
}


int gps_work()
{
	uint8_t byte;
	for (schetchik = 0; schetchik < GPS_SYMBOLS_PER_ACTS; schetchik++)
	{
		if (cb_pop(&byte) == false)
		{
			break;
		}
		gps_parse(byte);
		if (pavel_gps_sost == 2)
		{
			// Мы накопили сообщение. Можно его разбирать
			pavel_gps_sost = 0;
			enum minmea_sentence_id id;
			id = minmea_sentence_id(pavel_gps_buffer);
			if (id != MINMEA_SENTENCE_GGA)
			{
				continue;
			}

			struct minmea_sentence_gga gga;
			bool succes = minmea_parse_gga(&gga, pavel_gps_buffer);
			if (!succes)
			{
				continue;
			}
			time_us = gga.time.microseconds;
			time_s = 3600*gga.time.hours + 60*gga.time.minutes + gga.time.seconds;
			lat = minmea_tocoord(&gga.latitude);
			lon = minmea_tocoord(&gga.longitude);
			alt = minmea_tofloat(&gga.altitude);
			fix = gga.fix_quality;
			pos_cookie++;
			alt_cookie++;
			time_cookie++;

		}
	}
	return 0;
}



int gps_get_coords(int64_t * cookie, float * lat_, float * lon_, float * alt_, int *fix_)
{
	*cookie = pos_cookie;
	*lat_ = lat;
	*lon_ = lon;
	*alt_ = alt;
	*fix_ = fix;
	return 0;
}


int gps_get_alt(int64_t * cookie, float * alt_)
{
	*cookie = alt_cookie;
	*alt_ = alt;
	return 0;
}


int gps_get_time(int64_t * cookie, uint64_t * time_s_, uint32_t * time_us_)
{
	*cookie = time_cookie;
	*time_s_ = time_s;
	*time_us_ = time_us;
	return 0;
}

int gps_get_gga(int64_t * cookie, struct minmea_sentence_gga * gga_)
{
	*cookie = time_cookie;
	*gga_ = gga;
	return 0;
}
