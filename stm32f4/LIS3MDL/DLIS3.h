#include <stdio.h>
#include "lis3mdl_reg.h"

#include <stm32f4xx_hal.h>
extern SPI_HandleTypeDef hspi1;

#ifdef HAL_SPI_MODULE_ENABLED

struct lis_spi_intf
{
		GPIO_TypeDef  *GPIO_Port;
		uint16_t GPIO_Pin;
		SPI_HandleTypeDef* spi;
};



void lisset();
void lisread();
#endif
