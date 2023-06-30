/*
 * MX25L512_up.c
 *
 *  Created on: 31 мая 2023 г.
 *      Author: Install
 */


#include "../MX25L512_up.h"



int mx25l512_WRDI_up (bus_t *bus, uint16_t *Data, uint32_t timeout)
{
	uint8_t stat_reg = 0;
	mx25l512_wren(bus);
	uint32_t start_time = HAL_GetTick();
	while((stat_reg & 0x03) != 2)
	{
		mx25l512_rdsr(bus, &stat_reg);
		if (HAL_GetTick() - start_time > timeout)
			return ARTEM_TIMEOUT;
	}
	mx25l512_wrsr(bus, Data);
	return 0;
}

int mx25l512_PP_up (bus_t *bus, uint32_t *addr, uint8_t *data, size_t size, uint32_t timeout)
{
	uint8_t stat_reg = 0;
	mx25l512_wren(bus);
	uint32_t start_time = HAL_GetTick();
	while((stat_reg & 0x03) != 2)
	{
		mx25l512_rdsr(bus, &stat_reg);
		if (HAL_GetTick() - start_time > timeout)
			return ARTEM_TIMEOUT;
	}
	while((stat_reg & 0x01) != 0)
		{
			mx25l512_rdsr(bus, &stat_reg);
			if (HAL_GetTick() - start_time > timeout)
				return ARTEM_TIMEOUT;
		}
	mx25l512_PP(bus, addr, data, size);
	timeout = HAL_GetTick();
	do
	{
		mx25l512_rdsr(bus, &stat_reg);
		if (HAL_GetTick() - start_time > timeout)
			return ARTEM_TIMEOUT;
	}
	while((stat_reg & 0x01) != 0);
	return 0;
}

int mx25l512_SE_up (bus_t *bus, uint32_t *addr, uint32_t timeout)
{
	uint8_t stat_reg = 0;
	mx25l512_wren(bus);
	uint32_t start_time = HAL_GetTick();
	while((stat_reg & 0x03) != 2)
	{
		mx25l512_rdsr(bus, &stat_reg);
		if (HAL_GetTick() - start_time > timeout)
			return ARTEM_TIMEOUT;
	}
	mx25l512_SE(bus, addr);
	return 0;
}

int mx25l512_BE_up (bus_t *bus, uint32_t *addr, uint32_t timeout)
{
	uint8_t stat_reg = 0;
	mx25l512_wren(bus);
	uint32_t start_time = HAL_GetTick();
	while((stat_reg & 0x03) != 2)
	{
		mx25l512_rdsr(bus, &stat_reg);
		if (HAL_GetTick() - start_time > timeout)
			return ARTEM_TIMEOUT;
	}
	mx25l512_BE(bus, addr);
	return 0;
}

int mx25l512_CE_up (bus_t *bus, uint32_t timeout)
{
	uint8_t stat_reg = 0;
	mx25l512_wren(bus);
	uint32_t start_time = HAL_GetTick();
	while((stat_reg & 0x03) != 2)
	{
		mx25l512_rdsr(bus, &stat_reg);
		if (HAL_GetTick() - start_time > timeout)
			return ARTEM_TIMEOUT;
	}
	mx25l512_CE(bus);
	timeout = HAL_GetTick();
	do
	{
		mx25l512_rdsr(bus, &stat_reg);
		if (HAL_GetTick() - start_time > timeout)
			return ARTEM_TIMEOUT;
	}
	while((stat_reg & 0x01) != 0);
	return 0;
}
