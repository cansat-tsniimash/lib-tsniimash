/*
 * nrf24_lower_api.c
 *
 *  Created on: 29 янв. 2022 г.
 *      Author: 1
 */

#include <stm32f4xx_hal.h>
#ifdef HAL_SPI_MODULE_ENABLED
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../nrf24_lower_api.h"

extern SPI_HandleTypeDef hspi2;

#define CE_PORT GPIOA
#define CE_PIN GPIO_PIN_1

#define CS_PORT GPIOA
#define CS_PIN GPIO_PIN_0

void _rf24_CS(bool mode)
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

void rf24_read_register(uint8_t reg_addr, uint8_t * reg_data, size_t data_size)
{
	_rf24_CS(true);

	// Добавляем в 5 битов адреса еще 3 бита для чтения из этого регистра
	reg_addr = reg_addr & ~((1 << 5) | (1 << 6) | (1 << 7));

	// Передаем адресс регистра, который читаем
	HAL_SPI_Transmit(&hspi2, &reg_addr, 1, HAL_MAX_DELAY);

	// Читаем данные
	HAL_SPI_Receive(&hspi2, reg_data, data_size, HAL_MAX_DELAY);

	_rf24_CS(false);
}

void rf24_write_register(uint8_t reg_addr, const uint8_t * reg_data, size_t data_size)
{
	_rf24_CS(true);

	// Добавляем в 5 битов адреса еще 3 бита для чтения из этого регистра
	//printf("addr pre 0x%02X\n", (int)reg_addr);
	reg_addr = (reg_addr & ~((1 << 6) | (1 << 7))) | (1 << 5);
	//reg_addr &= ~((1 << 7) | (1 << 6));
	//reg_addr |= (1 << 5);
	//printf("addr after 0x%02X\n", (int)reg_addr);

	//printf("write_reg_tx: ");

	//char bits_buffer[10] = {0};
	//print_bits(reg_addr, bits_buffer);
	//printf("addr 0x%02X (0b%s); ", (int)reg_addr, bits_buffer);

	//for (size_t i = 0; i < data_size; i++)
	//{
		//memset(bits_buffer, 0x00, sizeof(bits_buffer));
		//print_bits(reg_data[i], bits_buffer);
		//printf("0x%02X (0b%s) ", (int)reg_data[i], bits_buffer);
	//}
	//printf("\n");

	// Передаем адресс регистра, который читаем, на шину spi и получаем в ответ значение STATUS регистра
	HAL_SPI_Transmit(&hspi2, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(&hspi2, (uint8_t*)reg_data, data_size, HAL_MAX_DELAY);

	_rf24_CS(false);
}

void rf24_read_rx_payload(uint8_t * payload_buffer, size_t payload_buffer_size)
{
	uint8_t command = RF24_R_RX_PAYLOAD;
	uint8_t payload_size = 0;
	rf24_get_rx_payload_size(&payload_size);
	if (payload_size > 0 || payload_size < 32)
	{
		if (payload_size > payload_buffer_size)
		{
			payload_size = payload_buffer_size;
		}
		_rf24_CS(true);
		HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
		HAL_SPI_Receive(&hspi2, payload_buffer, payload_size, HAL_MAX_DELAY);
		_rf24_CS(false);
	}
}

void rf24_write_tx_payload(const uint8_t * payload_buffer, size_t payload_size, bool use_ack)
{
	uint8_t command;
	_rf24_CS(true);

	if (use_ack)
	{
		command = RF24_W_TX_PAYLOAD;
	}
	else
	{
		command = RF24_W_TX_PAYLOAD_NO_ACK;
	}
	/*передаем команду */
	HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(&hspi2, (uint8_t*)payload_buffer, payload_size, HAL_MAX_DELAY);

	_rf24_CS(false);
}

void rf24_flush_rx(void)
{
	uint8_t command = RF24_FLUSH_RX;
	_rf24_CS(true);
	HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
	_rf24_CS(false);
}

void rf24_flush_tx(void)
{
	uint8_t command = RF24_FLUSH_TX;
	_rf24_CS(true);
	HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
	_rf24_CS(false);
}

void rf24_get_rx_payload_size(uint8_t * payload_size)
{
	uint8_t command = RF24_R_RX_PL_WID;
	_rf24_CS(true);
	HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi2, payload_size, 1, HAL_MAX_DELAY);
	_rf24_CS(false);
    if (*payload_size > 32)
    {
    	rf24_flush_rx();
    	payload_size = 0;
    }
}

void rf24_get_status(uint8_t * status)
{
	uint8_t command = RF24_NOP;
	_rf24_CS(true);
	HAL_SPI_TransmitReceive(&hspi2, &command, status, 1, HAL_MAX_DELAY);
	_rf24_CS(false);
}


// Управление CE пином модуля.
/* onoff: значение true - CE прижат к полу; false - CE поднят к 1 */
void rf24_ce_activate(bool onoff)
{
	if (onoff)
	{
		HAL_GPIO_WritePin(CE_PORT, CE_PIN,  GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(CE_PORT, CE_PIN,  GPIO_PIN_SET);
	}
}
#endif /* HAL_SPI_MODULE_ENABLED */
