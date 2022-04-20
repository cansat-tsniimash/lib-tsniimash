/*
 * Данные два рукотворные файлы программиста П (DBME280.c, DBME280.h) являются всего лишь прокладками, соединяющими, упро-
 * щающими и сокращающими остальные огромные строки кода из файлов папки BME280 и её подпапки src, которые написаны
 * программистами компании Bosh.
 */





#include <stm32f4xx_hal.h>


#ifdef HAL_SPI_MODULE_ENABLED


#include <stdio.h>
#include "bme280.h"



//Структура, содержащая в себе информацию о порте и пине подключения SPI CS
struct bme_spi_intf
{
	GPIO_TypeDef  *GPIO_Port;
	uint16_t GPIO_Pin;
	SPI_HandleTypeDef* spi;
};



struct bme_spi_intf_sr
{
	int sp_pin;
	SPI_HandleTypeDef* spi;
	//Shift reg device
};



/*
 * Создаёт дефолтные настройки, которые написал я и немцы, которые придумали датчик
 * spi_intf - настройка интерфейса SPI
 */
void bme_init_default( struct bme280_dev *bme, struct bme_spi_intf* spi_intf);

// Чтение данных с БМЕ280, может возвращать давление, влажность и температуру
struct bme280_data bme_read_data(struct bme280_dev *bme);



void bme_init_default_sr( struct bme280_dev *bme, struct bme_spi_intf_sr* spi_intf);
#endif
