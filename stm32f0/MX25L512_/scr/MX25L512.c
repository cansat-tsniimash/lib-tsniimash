#include <stdint.h>
#include <stdbool.h>
#include <stm32f0xx_hal.h>

#include "../MX25L512.h"

#ifdef HAL_SPI_MODULE_ENABLED

extern SPI_HandleTypeDef hspi1;



void mx25l512_wren(bus_t *bus)
{
	uint8_t cmd = MX25L512_WREN;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_wrdi(bus_t *bus)
{
	uint8_t cmd = MX25L512_WRDI;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_rdid(bus_t *bus, uint8_t *pData)
{
	uint8_t cmd = MX25L512_RDID;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Receive(bus->hspi, pData, 3, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_rdsr(bus_t *bus, uint8_t *pData)
{
	uint8_t cmd = MX25L512_RDSR;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Receive(bus->hspi, pData, 1, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_wrsr(bus_t *bus, uint16_t *Data)
{
	uint8_t cmd = MX25L512_WRSR;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 2, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)Data, 2, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_read(bus_t *bus, uint32_t *addr, uint8_t *pData, size_t size)
{
	uint8_t cmd = MX25L512_READ;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)addr, 3, MX25L512_TIMEOUT);
	HAL_SPI_Receive(bus->hspi, pData, size, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_fast_read(bus_t *bus, uint32_t *addr, uint8_t *pData, size_t size)
{
	uint8_t cmd = MX25L512_FAST_READ;
	uint16_t dummy = MX25L512_DUMMY;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)addr, 3, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)&dummy, 1, MX25L512_TIMEOUT);
	HAL_SPI_Receive(bus->hspi, pData, size, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_SE(bus_t *bus, uint32_t *addr)
{
	uint8_t cmd = MX25L512_SE;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)addr, 1, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_BE(bus_t *bus, uint32_t *addr)
{
	uint8_t cmd = MX25L512_BE;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)addr, 1, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_CE(bus_t *bus)
{
	uint8_t cmd = MX25L512_CE;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_PP(bus_t *bus, uint32_t *addr, uint8_t *data, size_t size)
{
	uint8_t cmd = MX25L512_PP;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)addr, 3, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)data, size, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_DP(bus_t *bus)
{
	uint8_t cmd = MX25L512_DP;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_RDP(bus_t *bus)
{
	uint8_t cmd = MX25L512_RDP;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_RES(bus_t *bus, uint8_t *pData)
{
	uint8_t cmd = MX25L512_RES;
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Receive(bus->hspi, pData, 1, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

void mx25l512_REMS(bus_t *bus, uint8_t *pData, bool wgf) //1 - manufacturer's ID first, 0 - device ID first
{
	uint8_t cmd = MX25L512_REMS;
	uint16_t dummy = MX25L512_DUMMY;
	uint8_t addr;
	if (wgf == 1)
		{
		addr = MX25L512_MANUFACTURER_ID_FIRST;
		}
		else
		{
		addr = MX25L512_MX25L512_DEVICE_ID_FIRST;
		}
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)&dummy, 2, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, &addr, 1, MX25L512_TIMEOUT);
	HAL_SPI_Receive(bus->hspi, pData, 2, MX25L512_TIMEOUT);
	HAL_GPIO_WritePin(bus->GPIOx, bus->GPIO_Pin, GPIO_PIN_SET);
}

#endif
