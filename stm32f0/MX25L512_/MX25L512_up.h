/*
 * MX25L512_up.h
 *
 *  Created on: 31 мая 2023 г.
 *      Author: Install
 */

#ifndef MX25L512__MX25L512_UP_H_
#define MX25L512__MX25L512_UP_H_

#include "MX25L512.h"

typedef enum
{
  ARTEM_TIMEOUT      = 0x04U
} ARTEM_ERROR_t;

int mx25l512_WRDI_up (bus_t *bus, uint16_t *Data, uint32_t timeout);

int mx25l512_PP_up (bus_t *bus, uint32_t *addr, uint8_t *data, size_t size, uint32_t timeout);

int mx25l512_SE_up (bus_t *bus, uint32_t *addr, uint32_t timeout);

int mx25l512_BE_up (bus_t *bus, uint32_t *addr, uint32_t timeout);

int mx25l512_CE_up (bus_t *bus, uint32_t timeout);

#endif /* MX25L512__MX25L512_UP_H_ */
