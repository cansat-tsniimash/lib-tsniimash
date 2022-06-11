/*
 * Achtung!!!!!!
 * В файле .с также есть остальные необходимые для осознавания принципа работы драйвера комменты!
 * Вам самим необходимо написать апп_майн. Это можно сделать по проверенному примеру:

#include <stdio.h>

#include "LIS3MDL/DLIS3.h"
#include "LSM6DS3/DLSM.h"
#include "stm32f4xx_hal.h"

extern SPI_HandleTypeDef hspi1;


int app_main()
{
	struct lsm_spi_intf spi_interface = {0};
	stmdev_ctx_t ctx = {0};

	spi_interface.GPIO_Port = GPIOB;
	spi_interface.GPIO_Pin = GPIO_PIN_0;
	spi_interface.spi = &hspi1;
	lsmset(&ctx, &spi_interface);
	float acc_g[3];
	float gyro_dps[3];
	float temperature_celsius_mag;
	while(1)
	{
		lsmread(&ctx, &temperature_celsius_mag, &acc_g, &gyro_dps);
		printf("ax: %10lf ay: %10lf az: %10lf gx: %10lf gy: %10lf gz: %10lf temp: %10lf\n", acc_g[0], acc_g[1], acc_g[2], gyro_dps[0], gyro_dps[1], gyro_dps[2], temperature_celsius_mag);
	}

	return 0;
}
 *
 */

#ifndef DLSM_H_
#define DLSM_H_

#include <stdio.h>
#include "lsm6ds3_reg.h"
#include "Shift_Register\shift_reg.h"

#include <stm32f1xx_hal.h>


#ifdef HAL_SPI_MODULE_ENABLED

// GPIO Port, GPIO PIN   CS подключения
struct lsm_spi_intf
{
		GPIO_TypeDef  *GPIO_Port;
		uint16_t GPIO_Pin;
		SPI_HandleTypeDef* spi;
};



struct lsm_spi_intf_sr
{
		int sr_pin;
		SPI_HandleTypeDef* spi;
		//Shift reg device
		shift_reg_t *sr;
};
typedef struct lsm_spi_intf_sr lsm_spi_intf_sr;
//настройка и инициализация
void lsmset(stmdev_ctx_t *ctx, 	struct lsm_spi_intf *spi_interface);
//чтение данных
void lsmread(stmdev_ctx_t *ctx, float *temperature_celsius_gyro, float (*acc_g)[3], float (*gyro_dps)[3]);


void lsmset_sr(stmdev_ctx_t *ctx, struct lsm_spi_intf_sr *spi_interface);

#endif // HAL_SPI_MODULE_ENABLED

#endif // DLSM_H_
