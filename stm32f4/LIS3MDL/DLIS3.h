#include <stdio.h>
#include "lis3mdl_reg.h"

#include <stm32f4xx_hal.h>

#ifdef HAL_SPI_MODULE_ENABLED
extern SPI_HandleTypeDef hspi1;


struct lis_spi_intf
{
		GPIO_TypeDef  *GPIO_Port;
		uint16_t GPIO_Pin;
		SPI_HandleTypeDef* spi;
};



void lisset(stmdev_ctx_t *ctx);
void lisread(stmdev_ctx_t *ctx, float *temperature_celsius_mag, float (*mag)[3]);
#endif
