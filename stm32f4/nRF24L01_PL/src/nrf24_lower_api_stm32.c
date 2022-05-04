#include <stm32f4xx_hal.h>
#ifdef HAL_SPI_MODULE_ENABLED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../nrf24_lower_api.h"
#include "../nrf24_lower_api_stm32.h"

#include "../nrf24_defs.h"

extern SPI_HandleTypeDef hspi2;



static void _nrf24_CS(void * intf_ptr, bool mode)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	nrf24_spi_pins_t *api_config_low = (nrf24_spi_pins_t *)api_config->intf_ptr;
	if (mode)
	{
		// Опускаем chip select для того, что бы начать общение с конкретным устройством.
		HAL_GPIO_WritePin(api_config_low->cs_port, api_config_low->cs_pin,  GPIO_PIN_RESET);
		//HAL_Delay(11);
	}
	else
	{
		// Поднимаем chip select для того, что бы закончить общение с конкретным устройством.
		HAL_GPIO_WritePin(api_config_low->cs_port, api_config_low->cs_pin,  GPIO_PIN_SET);
		//HAL_Delay(100);
	}
}

static void _nrf24_CS_sr(void * intf_ptr, bool mode)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	nrf24_spi_pins_sr_t *api_config_low = (nrf24_spi_pins_sr_t *)api_config->intf_ptr;
	if (!mode)
	{
		shift_reg_oe(api_config_low->this, true);
		shift_reg_write_bit_8(api_config_low->this, api_config_low->pos_CS, !mode);
		shift_reg_oe(api_config_low->this, false);
	}
	else
	{
		shift_reg_oe(api_config_low->this, true);
		shift_reg_write_bit_8(api_config_low->this, api_config_low->pos_CS, !mode);
		shift_reg_oe(api_config_low->this, false);
	}
}

void nrf24_read_register(void * intf_ptr, uint8_t reg_addr, uint8_t * reg_data, size_t data_size)
{

	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;

	api_config->nrf24_CS(intf_ptr, true);

	// Добавляем в 5 битов адреса еще 3 бита для чтения из этого регистра
	reg_addr = reg_addr & ~((1 << 5) | (1 << 6) | (1 << 7));

	// Передаем адресс регистра, который читаем и читаем данные
	HAL_SPI_Transmit(api_config->hspi, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(api_config->hspi, reg_data, data_size, HAL_MAX_DELAY);

	api_config->nrf24_CS(intf_ptr, false);
}

void nrf24_write_register(void * intf_ptr, uint8_t reg_addr, const uint8_t * reg_data, size_t data_size)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	api_config->nrf24_CS(intf_ptr, true);

	// Добавляем в 5 битов адреса еще 3 бита для записи в регистр
	reg_addr = (reg_addr & ~((1 << 6) | (1 << 7))) | (1 << 5);

	// Передаем адресс регистра, в который пишем и пишем в регистр
	HAL_SPI_Transmit(api_config->hspi, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(api_config->hspi, (uint8_t*)reg_data, data_size, HAL_MAX_DELAY);

	api_config->nrf24_CS(intf_ptr, false);
}

void nrf24_read_rx_payload(void * intf_ptr, uint8_t * payload_buffer, size_t payload_buffer_size)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	uint8_t command = NRF24_R_RX_PAYLOAD;
	uint8_t payload_size = 0;
    // Считываем размер данных
	nrf24_get_rx_payload_size(intf_ptr, &payload_size);
	// Если размер данных от 1 до 32, то читаем данные
	if (payload_size > 0 || payload_size < 32)
	{
		// Если места на запись меньше, чем у нас есть данных, читем сколько, сколько есть места
		if (payload_size > payload_buffer_size)
		{
			payload_size = payload_buffer_size;
		}
		api_config->nrf24_CS(intf_ptr, true);
		// Читаем данные
		HAL_SPI_Transmit(api_config->hspi, &command, 1, HAL_MAX_DELAY);
		HAL_SPI_Receive(api_config->hspi, payload_buffer, payload_size, HAL_MAX_DELAY);
		api_config->nrf24_CS(intf_ptr, false);
	}
}

void nrf24_write_tx_payload(void * intf_ptr, const uint8_t * payload_buffer, size_t payload_size, bool use_ack)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	uint8_t command;
	api_config->nrf24_CS(intf_ptr, true);
	// Учитываем использование ack
	if (use_ack)
	{
		command = NRF24_W_TX_PAYLOAD;
	}
	else
	{
		command = NRF24_W_TX_PAYLOAD_NO_ACK;
	}
	// Передаем данные
	HAL_SPI_Transmit(api_config->hspi, &command, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(api_config->hspi, (uint8_t*)payload_buffer, payload_size, HAL_MAX_DELAY);

	api_config->nrf24_CS(intf_ptr, false);
}

void nrf24_flush_rx(void * intf_ptr)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	uint8_t command = NRF24_FLUSH_RX;
	api_config->nrf24_CS(intf_ptr, true);
	HAL_SPI_Transmit(api_config->hspi, &command, 1, HAL_MAX_DELAY);
	api_config->nrf24_CS(intf_ptr, false);
}

void nrf24_flush_tx(void * intf_ptr)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	uint8_t command = NRF24_FLUSH_TX;
	api_config->nrf24_CS(intf_ptr, true);
	HAL_SPI_Transmit(api_config->hspi, &command, 1, HAL_MAX_DELAY);
	api_config->nrf24_CS(intf_ptr, false);
}

// Повтор предыдущего отправляемого пакета
void nrf24_ruse_tx_pl(void * intf_ptr)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	uint8_t command = NRF24_REUSE_TX_PL;
	api_config->nrf24_CS(intf_ptr, true);
	HAL_SPI_Transmit(api_config->hspi, &command, 1, HAL_MAX_DELAY);
	api_config->nrf24_CS(intf_ptr, false);
}

void nrf24_get_rx_payload_size(void * intf_ptr, uint8_t * payload_size)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	uint8_t command = NRF24_R_RX_PL_WID;
	api_config->nrf24_CS(intf_ptr, true);
	HAL_SPI_Transmit(api_config->hspi, &command, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(api_config->hspi, payload_size, 1, HAL_MAX_DELAY);
	api_config->nrf24_CS(intf_ptr, false);
	// Если размер данных больше 32, то отчищаем rx буфер и зануляем его
    if (*payload_size > 32)
    {
    	nrf24_flush_rx(intf_ptr);
    	payload_size = 0;
    }
}

// Запись пакета для отправки вместе с очередным ACK пакетом
void nrf24_write_ack_payload(void * intf_ptr, const uint8_t * payload, size_t payload_size, uint8_t pipe)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	uint8_t command = (NRF24_W_ACK_PAYLOAD << 3) | (pipe & 0x07);
	api_config->nrf24_CS(intf_ptr, true);

	// Передаем данные
	HAL_SPI_Transmit(api_config->hspi, &command, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(api_config->hspi, (uint8_t*)payload, payload_size, HAL_MAX_DELAY);

	api_config->nrf24_CS(intf_ptr, false);
}

void nrf24_get_status(void * intf_ptr, uint8_t * status)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	uint8_t command = NRF24_NOP;
	api_config->nrf24_CS(intf_ptr, true);
	HAL_SPI_TransmitReceive(api_config->hspi, &command, status, 1, HAL_MAX_DELAY);
	api_config->nrf24_CS(intf_ptr, false);
}

void nrf24_ce_activate(void * intf_ptr, bool onoff)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	api_config->nrf24_CE(intf_ptr, onoff);
}

void _nrf24_CE(void * intf_ptr, bool onoff)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	nrf24_spi_pins_t *api_config_low = (nrf24_spi_pins_t *)api_config->intf_ptr;
	if (onoff)
	{
		HAL_GPIO_WritePin(api_config_low->ce_port, api_config_low->ce_pin,  GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(api_config_low->ce_port, api_config_low->ce_pin, GPIO_PIN_RESET);
	}
}

void _nrf24_CE_sr(void * intf_ptr, bool onoff)
{
	nrf24_lower_api_config_t *api_config = (nrf24_lower_api_config_t *)intf_ptr;
	nrf24_spi_pins_sr_t *api_config_low = (nrf24_spi_pins_sr_t *)api_config->intf_ptr;

	shift_reg_oe(api_config_low->this, true);
	shift_reg_write_bit_8(api_config_low->this, api_config_low->pos_CE, onoff);
	shift_reg_oe(api_config_low->this, false);
}

void nrf24_spi_init(nrf24_lower_api_config_t* nrf24, SPI_HandleTypeDef *hspi, nrf24_spi_pins_t* pins)
{
	nrf24->hspi = hspi;
	nrf24->nrf24_CS = _nrf24_CS;
	nrf24->nrf24_CE = _nrf24_CE;
	nrf24->intf_ptr = pins;
}

void nrf24_spi_init_sr(nrf24_lower_api_config_t* nrf24, SPI_HandleTypeDef *hspi, nrf24_spi_pins_sr_t* pins)
{
	nrf24->hspi = hspi;
	nrf24->nrf24_CS = _nrf24_CS_sr;
	nrf24->nrf24_CE = _nrf24_CE_sr;
	nrf24->intf_ptr = pins;
}

#endif /* HAL_SPI_MODULE_ENABLED */
