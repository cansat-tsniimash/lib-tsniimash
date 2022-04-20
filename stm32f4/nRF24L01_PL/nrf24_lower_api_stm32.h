#ifndef NRF24L01_PL_SRC_NRF24_LOWER_API_STM32_H_
#define NRF24L01_PL_SRC_NRF24_LOWER_API_STM32_H_

#include "../Shift_Register/shift_reg.h"
#include <stm32f4xx_hal.h>
#ifdef HAL_SPI_MODULE_ENABLED

// Структура, содержащая параметры SPI и пинов  Chip Enab и SPI Chip Select
typedef struct nrf24_lower_api_config_t
{
	// Дескриптор SPI
	SPI_HandleTypeDef *hspi;
	void (*nrf24_CS)(void * intf_ptr, bool mode);
	void (*nrf24_CE)(void * intf_ptr, bool mode);
	void *intf_ptr;
} nrf24_lower_api_config_t;

// Структура, содержащая параметры SPI пинов  Chip Enab и SPI Chip Select
typedef struct nrf24_spi_pins_t
{
	// Настройки Chip Enab
	GPIO_TypeDef *ce_port;
	uint16_t ce_pin;
	// Настройки SPI Chip Select
	GPIO_TypeDef *cs_port;
	uint16_t cs_pin;
} nrf24_spi_pins_t;

// Структура, содержащая параметры SPI пинов  Chip Enab и SPI Chip Select для сдвигового регистра
typedef struct nrf24_spi_pins_sr_t
{
	shift_reg_t *this;
	// Настройки Chip Enab
	uint8_t pos_CE;
	// Настройки SPI Chip Select
	uint8_t pos_CS;
} nrf24_spi_pins_sr_t;

void nrf24_spi_init(nrf24_lower_api_config_t* nrf24, SPI_HandleTypeDef *hspi, nrf24_spi_pins_t* pins);
void nrf24_spi_init_sr(nrf24_lower_api_config_t* nrf24, SPI_HandleTypeDef *hspi, nrf24_spi_pins_sr_t* pins);

#endif /* HAL_SPI_MODULE_ENABLED */
#endif /* NRF24L01_PL_SRC_NRF24_LOWER_API_STM32_H_ */
