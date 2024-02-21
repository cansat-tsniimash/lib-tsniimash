/*
 * ADS1115.c
 *
 *  Created on: Dec 1, 2023
 *      Author: Роберт
 */
#include <stm32f4xx_hal.h>

#ifdef HAL_I2C_MODULE_ENABLED

#include "ADS1115.h"
#include "ADS1115/i2c-crutch.h"

//ads пишет
int ads1115_write(ads1115_t *ads, uint8_t reg_addr, uint16_t reg_data, uint32_t Timeout)
{
	uint8_t data[3];
	data[2] = (uint8_t)reg_data;
	data[1] = (uint8_t)(reg_data >> 8);
	data[0] = reg_addr;
	int res = HAL_I2C_Master_Transmit(ads->hi2c, ads->DevAddress, &reg_addr, 1, Timeout);
	if (res != HAL_OK)
	{
		I2C_ClearBusyFlagErratum(ads->hi2c, 10);
		return res;
	}
	res = HAL_I2C_Master_Transmit(ads->hi2c, ads->DevAddress, data, 3, Timeout);
	if (res != HAL_OK)
	{
		I2C_ClearBusyFlagErratum(ads->hi2c, 10);
	}
	return res;
}
//ads читает
int ads1115_read(ads1115_t *ads, uint8_t reg_addr, uint16_t *reg_data, uint32_t Timeout)
{
	uint8_t data[2];
	int res = HAL_I2C_Master_Transmit(ads->hi2c, ads->DevAddress, &reg_addr, 1, Timeout);
	if (res != HAL_OK)
	{
		I2C_ClearBusyFlagErratum(ads->hi2c, 10);
		return res;
	}
	res = HAL_I2C_Master_Receive(ads->hi2c, ads->DevAddress, data, 2, Timeout);
	if (res != HAL_OK)
	{
		I2C_ClearBusyFlagErratum(ads->hi2c, 10);
		return res;
	}
	*reg_data = (data[0] << 8) + data[1];
	return res;
}

//облегчение поиска из конфигурационного регистра
int ads1115_conf_os(uint16_t conf_reg)//состояние устройства
{
	return (conf_reg >> 15) & 0x01;
}

int ads1115_conf_mode(uint16_t conf_reg)//Режим работы
{
	return (conf_reg >> 8) & 0x01;
}

int ads1115_conf_comp_mode(uint16_t conf_reg)//Тип компаратора
{
	return (conf_reg >> 4) & 0x01;
}

int ads1115_conf_comp_pol(uint16_t conf_reg)//Полярность компаратора
{
	return (conf_reg >> 3) & 0x01;
}

int ads1115_conf_comp_lat(uint16_t conf_reg)//Режим компаратора
{
	return (conf_reg >> 2) & 0x01;
}

int ads1115_conf_mux(uint16_t conf_reg)//Настройка мультиплексора
{
	return (conf_reg >> 12) & 7;
}

int ads1115_conf_pga(uint16_t conf_reg)//Коэффициент усиления усилителя
{
	return (conf_reg >> 9) & 7;
}

int ads1115_conf_dr(uint16_t conf_reg)//Частота дискретизации
{
	return (conf_reg >> 5) & 7;
}

int ads1115_conf_comp_que(uint16_t conf_reg)//Управление компаратором
{
	return (conf_reg >> 0) & 3;
}

int ads1115_write_mux(ads1115_mux_t mux, ads1115_t *ads)
{
	int res = ads1115_read(ads, CONF_REG, &ads->conf_reg, 100);
	if (res != HAL_OK)
		return res;
	ads->conf_reg = ads->conf_reg & ~(7 << 12); //  & это И  ~ НЕ
	ads->conf_reg = ads->conf_reg | (mux << 12);//  | это ИЛИ
	res = ads1115_write(ads, CONF_REG, ads->conf_reg, 100);
	return res;
}

int ads1115_write_pga(ads1115_pga_t pga, ads1115_t *ads)
{
	int res = ads1115_read(ads, CONF_REG, &ads->conf_reg, 100);
	if (res != HAL_OK)
		return res;
	ads->conf_reg = ads->conf_reg & ~(7 << 9); //  & это И  ~ НЕ
	ads->conf_reg = ads->conf_reg | (pga << 9);//  | это ИЛИ
	res = ads1115_write(ads, CONF_REG, ads->conf_reg, 100);
	return res;
}


int ads1115_write_os(ads1115_os_t os, ads1115_t *ads)
{
	int res = ads1115_read(ads, CONF_REG, &ads->conf_reg, 100);
	if (res != HAL_OK)
		return res;
	ads->conf_reg = ads->conf_reg & ~(1 << 15);
	ads->conf_reg = ads->conf_reg | (os << 15);
	res = ads1115_write(ads, CONF_REG, ads->conf_reg, 100);
	return res;

}

int ads1115_write_mode( ads1115_mode_t mode, ads1115_t *ads)
{
	int res = ads1115_read(ads, CONF_REG, &ads->conf_reg, 100);
		if (res != HAL_OK)
			return res;
		ads->conf_reg = ads->conf_reg & ~(1 << 8);
		ads->conf_reg = ads->conf_reg | (mode << 8);
		res = ads1115_write(ads, CONF_REG, ads->conf_reg, 100);
		return res;
}

int ads1115_write_dr( ads1115_dr_t dr, ads1115_t *ads)
{
	int res = ads1115_read(ads, CONF_REG, &ads->conf_reg, 100);
		if (res != HAL_OK)
			return res;
		ads->conf_reg = ads->conf_reg & ~(7 << 5);
		ads->conf_reg = ads->conf_reg | (dr << 5);
		res = ads1115_write(ads, CONF_REG, ads->conf_reg, 100);
		return res;
}

int ads1115_init(ads1115_t *ads)
{
	int res;
	res = ads1115_read(ads, CONF_REG, &ads->conf_reg, 100);
	if (res != HAL_OK)
		return res;
	res = ads1115_write_pga(ADS1115_PGA_FS_4p096, ads);
	if (res != HAL_OK)
		return res;
	res = ads1115_write_mux(ADS1115_MUX_P0_NG, ads);
	if (res != HAL_OK)
		return res;
	res = ads1115_write_mode(ADS1115_MODE_SINGLE, ads);
	if (res != HAL_OK)
		return res;
	res = ads1115_write_dr(ADS1115_DR_128HZ, ads);
	return res;
}

int ads1115_req_single(ads1115_t *ads)
{
	int res = ads1115_write_os(ADS1115_OS_SET, ads);
	return res;
}

int ads1115_read_single(ads1115_t *ads, uint16_t *ptr)
{
	int res;
	res = ads1115_read(ads, CONF_REG, &ads->conf_reg, 100);
	if (res != HAL_OK)
		return res;
	if (ads1115_conf_mode(ads->conf_reg) != ADS1115_MODE_SINGLE)
		return -2;
	if (ads1115_conf_os(ads->conf_reg) == 0)
		return -1;
	res = ads1115_read(ads, CONV_REG, ptr, 100);
	return res;
}

float ads1115_convert(ads1115_t *ads, uint16_t volt)
{
	switch(ads1115_conf_pga(ads->conf_reg))
	{
	case ADS1115_PGA_FS_6p144:
		return volt * 0.1875f;
	case ADS1115_PGA_FS_4p096:
		return volt * 0.125f;
	case ADS1115_PGA_FS_2p048:
		return volt * 0.0625f;
	case ADS1115_PGA_FS_1p024:
		return volt * 0.03125f;
	case ADS1115_PGA_FS_0p512:
		return volt * 0.015625f;
	case ADS1115_PGA_FS_0p256:
		return volt * 0.007813f;
	}
	return 0;
}

#endif //HAL_I2C_MODULE_ENABLED


