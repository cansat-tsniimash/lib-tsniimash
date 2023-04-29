#include <stdint.h>
#include <stdbool.h>
#include <stm32f0xx_hal.h>

#include "../MX25L512.h"

#ifdef HAL_SPI_MODULE_ENABLED

extern SPI_HandleTypeDef hspi1;


static void _mx25l512_CS(void * intf_ptr, bool mode)
{
	mx25l512_spi_pins_t *api_config = (mx25l512_spi_pins_t *)intf_ptr;
	if (mode)
	{
		// Опускаем chip select для того, что бы начать общение с конкретным устройством.
		HAL_GPIO_WritePin(api_config->cs_port, api_config->cs_pin,  GPIO_PIN_RESET);
	}
	else
	{
		// Поднимаем chip select для того, что бы закончить общение с конкретным устройством.
		HAL_GPIO_WritePin(api_config->cs_port, api_config->cs_pin,  GPIO_PIN_SET);
	}
}

static void _mx25l512_CS_sr(void * intf_ptr, bool mode)
{
	mx25l512_spi_pins_sr_t *api_config = (mx25l512_spi_pins_sr_t *)intf_ptr;
	if (!mode)
	{
		shift_reg_oe(api_config->this, true);
		shift_reg_write_bit_8(api_config->this, api_config->pos_CS, !mode);
		shift_reg_oe(api_config->this, false);
	}
	else
	{
		shift_reg_oe(api_config->this, true);
		shift_reg_write_bit_8(api_config->this, api_config->pos_CS, !mode);
		shift_reg_oe(api_config->this, false);
	}
}

void mx25l512_spi_init(bus_t *bus, SPI_HandleTypeDef *hspi, mx25l512_spi_pins_t *pins)
{
	bus->hspi = hspi;
	bus->mx25l512_CS = _mx25l512_CS;
	bus->intf_ptr = pins;
}

void mx25l512_spi_init_sr(bus_t *bus, SPI_HandleTypeDef *hspi, mx25l512_spi_pins_sr_t *pins)
{
	bus->hspi = hspi;
	bus->mx25l512_CS = _mx25l512_CS_sr;
	bus->intf_ptr = pins;
}

void mx25l512_wren(bus_t *bus)
{
	uint8_t cmd = MX25L512_WREN;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

void mx25l512_wrdi(bus_t *bus)
{
	uint8_t cmd = MX25L512_WRDI;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

void mx25l512_rdid(bus_t *bus, uint8_t *pData)
{
	uint8_t cmd = MX25L512_RDID;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Receive(bus->hspi, pData, 3, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

void mx25l512_rdsr(bus_t *bus, uint8_t *pData)
{
	uint8_t cmd = MX25L512_RDSR;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Receive(bus->hspi, pData, 1, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

void mx25l512_wrsr(bus_t *bus, uint16_t *Data)
{
	uint8_t cmd = MX25L512_WRSR;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 2, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)Data, 2, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

void mx25l512_read(bus_t *bus, uint32_t *addr, uint8_t *pData, size_t size)
{
	uint8_t cmd = MX25L512_READ;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)addr, 3, MX25L512_TIMEOUT);
	HAL_SPI_Receive(bus->hspi, pData, size, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

void mx25l512_fast_read(bus_t *bus, uint32_t *addr, uint8_t *pData, size_t size)
{
	uint8_t cmd = MX25L512_FAST_READ;
	uint16_t dummy = MX25L512_DUMMY;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)addr, 3, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)&dummy, 1, MX25L512_TIMEOUT);
	HAL_SPI_Receive(bus->hspi, pData, size, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

void mx25l512_SE(bus_t *bus, uint32_t *addr)
{
	uint8_t cmd = MX25L512_SE;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)addr, 1, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

void mx25l512_BE(bus_t *bus, uint32_t *addr)
{
	uint8_t cmd = MX25L512_BE;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)addr, 1, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

void mx25l512_CE(bus_t *bus)
{
	uint8_t cmd = MX25L512_CE;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

void mx25l512_PP(bus_t *bus, uint32_t *addr, uint8_t *data, size_t size)
{
	uint8_t cmd = MX25L512_PP;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)addr, 3, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)data, size, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

void mx25l512_DP(bus_t *bus)
{
	uint8_t cmd = MX25L512_DP;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

void mx25l512_RDP(bus_t *bus)
{
	uint8_t cmd = MX25L512_RDP;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

void mx25l512_RES(bus_t *bus, uint8_t *pData)
{
	uint8_t cmd = MX25L512_RES;
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Receive(bus->hspi, pData, 1, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
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
	bus->mx25l512_CS(bus->intf_ptr, true);
	HAL_SPI_Transmit(bus->hspi, &cmd, 1, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, (uint8_t *)&dummy, 2, MX25L512_TIMEOUT);
	HAL_SPI_Transmit(bus->hspi, &addr, 1, MX25L512_TIMEOUT);
	HAL_SPI_Receive(bus->hspi, pData, 2, MX25L512_TIMEOUT);
	bus->mx25l512_CS(bus->intf_ptr, false);
}

#endif
