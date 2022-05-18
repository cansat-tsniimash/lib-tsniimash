#ifndef DLIS3_H_
#define DLIS3_H_

#include <stdio.h>
#include "lis3mdl_reg.h"

#include <stm32f4xx_hal.h>
#include "Shift_Register\shift_reg.h"



#ifdef HAL_SPI_MODULE_ENABLED
extern SPI_HandleTypeDef hspi1;


struct lis_spi_intf
{
		GPIO_TypeDef  *GPIO_Port;
		uint16_t GPIO_Pin;
		SPI_HandleTypeDef* spi;
};
struct lis_spi_intf_sr
{
		int sr_pin;
		SPI_HandleTypeDef* spi;
		//Shift reg device
		shift_reg_t *sr;
};

typedef struct lis_spi_intf_sr lis_spi_intf_sr;
//настройка и инициализация
void lisset(stmdev_ctx_t *ctx, struct lis_spi_intf *spi_interface);
//чтение данных
void lisread(stmdev_ctx_t *ctx, float *temperature_celsius_mag, float (*mag)[3]);

void lisset_sr(stmdev_ctx_t *ctx, struct lis_spi_intf_sr *spi_interface);

#endif // HAL_SPI_MODULE_ENABLED

#endif // DLIS3_H_
