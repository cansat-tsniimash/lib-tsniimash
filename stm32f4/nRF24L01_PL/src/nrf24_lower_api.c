#include <stm32f4xx_hal.h>
#ifdef HAL_SPI_MODULE_ENABLED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../nrf24_lower_api.h"
#include "../nrf24_defs.h"

extern SPI_HandleTypeDef hspi2;

#define CE_PORT GPIOA
#define CE_PIN GPIO_PIN_1

#define CS_PORT GPIOA
#define CS_PIN GPIO_PIN_0

static void _nrf24_CS(bool mode)
{
	if (mode)
	{
		// Опускаем chip select для того, что бы начать общение с конкретным устройством.
		HAL_GPIO_WritePin(CS_PORT, CS_PIN,  GPIO_PIN_RESET);
		HAL_Delay(11);
	}
	else
	{
		// Поднимаем chip select для того, что бы закончить общение с конкретным устройством.
		HAL_GPIO_WritePin(CS_PORT, CS_PIN,  GPIO_PIN_SET);
		HAL_Delay(100);
	}
}

void nrf24_read_register(uint8_t reg_addr, uint8_t * reg_data, size_t data_size)
{
	_nrf24_CS(true);

	// Добавляем в 5 битов адреса еще 3 бита для чтения из этого регистра
	reg_addr = reg_addr & ~((1 << 5) | (1 << 6) | (1 << 7));

	// Передаем адресс регистра, который читаем и читаем данные
	HAL_SPI_Transmit(&hspi2, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi2, reg_data, data_size, HAL_MAX_DELAY);

	_nrf24_CS(false);
}

void nrf24_write_register(uint8_t reg_addr, const uint8_t * reg_data, size_t data_size)
{
	_nrf24_CS(true);

	// Добавляем в 5 битов адреса еще 3 бита для записи в регистр
	reg_addr = (reg_addr & ~((1 << 6) | (1 << 7))) | (1 << 5);

	// Передаем адресс регистра, в который пишем и пишем в регистр
	HAL_SPI_Transmit(&hspi2, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(&hspi2, (uint8_t*)reg_data, data_size, HAL_MAX_DELAY);

	_nrf24_CS(false);
}

void nrf24_read_rx_payload(uint8_t * payload_buffer, size_t payload_buffer_size)
{
	uint8_t command = NRF24_R_RX_PAYLOAD;
	uint8_t payload_size = 0;
    // Считываем размер данных
	nrf24_get_rx_payload_size(&payload_size);
	// Если размер данных от 1 до 32, то читаем данные
	if (payload_size > 0 || payload_size < 32)
	{
		// Если места на запись меньше, чем у нас есть данных, читем сколько, сколько есть места
		if (payload_size > payload_buffer_size)
		{
			payload_size = payload_buffer_size;
		}
		_nrf24_CS(true);
		// Читаем данные
		HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
		HAL_SPI_Receive(&hspi2, payload_buffer, payload_size, HAL_MAX_DELAY);
		_nrf24_CS(false);
	}
}

void nrf24_write_tx_payload(const uint8_t * payload_buffer, size_t payload_size, bool use_ack)
{
	uint8_t command;
	_nrf24_CS(true);
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
	HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(&hspi2, (uint8_t*)payload_buffer, payload_size, HAL_MAX_DELAY);

	_nrf24_CS(false);
}

void nrf24_flush_rx(void)
{
	uint8_t command = NRF24_FLUSH_RX;
	_nrf24_CS(true);
	HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
	_nrf24_CS(false);
}

void nrf24_flush_tx(void)
{
	uint8_t command = NRF24_FLUSH_TX;
	_nrf24_CS(true);
	HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
	_nrf24_CS(false);
}

// Повтор предыдущего отправляемого пакета
void nrf24_ruse_tx_pl(void)
{
	uint8_t command = NRF24_REUSE_TX_PL;
	_nrf24_CS(true);
	HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
	_nrf24_CS(false);
}

void nrf24_get_rx_payload_size(uint8_t * payload_size)
{
	uint8_t command = NRF24_R_RX_PL_WID;
	_nrf24_CS(true);
	HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi2, payload_size, 1, HAL_MAX_DELAY);
	_nrf24_CS(false);
	// Если размер данных больше 32, то отчищаем rx буфер и зануляем его
    if (*payload_size > 32)
    {
    	nrf24_flush_rx();
    	payload_size = 0;
    }
}

// Запись пакета для отправки вместе с очередным ACK пакетом
void nrf24_write_ack_payload(const uint8_t * payload, size_t payload_size, uint8_t pipe)
{
	uint8_t command = (NRF24_W_ACK_PAYLOAD << 3) | (pipe & 0x07);
	_nrf24_CS(true);

	// Передаем данные
	HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(&hspi2, (uint8_t*)payload, payload_size, HAL_MAX_DELAY);

	_nrf24_CS(false);
}

void nrf24_get_status(uint8_t * status)
{
	uint8_t command = NRF24_NOP;
	_nrf24_CS(true);
	HAL_SPI_TransmitReceive(&hspi2, &command, status, 1, HAL_MAX_DELAY);
	_nrf24_CS(false);
}

void nrf24_ce_activate(bool onoff)
{
	if (onoff)
	{
		HAL_GPIO_WritePin(CE_PORT, CE_PIN,  GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(CE_PORT, CE_PIN,  GPIO_PIN_RESET);
	}
}

#endif /* HAL_SPI_MODULE_ENABLED */
