#ifndef NRF24L01_PL_SRC_NRF24_LOWER_API_STM32_H_
#define NRF24L01_PL_SRC_NRF24_LOWER_API_STM32_H_

#include <stm32f4xx_hal.h>
#ifdef HAL_SPI_MODULE_ENABLED

// Структура, содержащая параметры SPI и пинов  Chip Enab и SPI Chip Select
typedef struct nrf24_lower_api_config_t
{
	// Дескриптор SPI
	SPI_HandleTypeDef *hspi;
	// Настройки Chip Enab
	GPIO_TypeDef *ce_port;
	uint16_t ce_pin;
	// Настройки SPI Chip Select
	GPIO_TypeDef *cs_port;
	uint16_t cs_pin;
} nrf24_lower_api_config_t;

#endif /* HAL_SPI_MODULE_ENABLED */
#endif /* NRF24L01_PL_SRC_NRF24_LOWER_API_STM32_H_ */
