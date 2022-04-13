#include <stdio.h>
#include "lsm6ds3_reg.h"

#include <stm32f4xx_hal.h>


#ifdef HAL_SPI_MODULE_ENABLED


struct lsm_spi_intf
{
		GPIO_TypeDef  *GPIO_Port;
		uint16_t GPIO_Pin;
		SPI_HandleTypeDef* spi;
};

void lsmset(stmdev_ctx_t *ctx);
void lsmread(stmdev_ctx_t *ctx, float temperature_celsius_gyro, float acc_g[3], float gyro_dps[3]);
#endif
