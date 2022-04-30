#include <stm32f4xx_hal.h>


#ifdef HAL_SPI_MODULE_ENABLED


#include <stdio.h>
#include "..\bme280.h"
#include "..\DriverForBME280.h"
#include "..\..\Shift_Register\shift_reg.h"










static BME280_INTF_RET_TYPE bme_spi_read(
		uint8_t reg_addr, uint8_t * data, uint32_t data_len, void *intf_ptr
)
{


	struct bme_spi_intf* spi_intf = intf_ptr;

	HAL_GPIO_WritePin(spi_intf->GPIO_Port, spi_intf->GPIO_Pin, GPIO_PIN_RESET);
	reg_addr |= (1 << 7);
	HAL_SPI_Transmit(spi_intf->spi, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(spi_intf->spi, data, data_len, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(spi_intf->GPIO_Port, spi_intf->GPIO_Pin, GPIO_PIN_SET);

	return 0;
}


static BME280_INTF_RET_TYPE bme_spi_write(
		uint8_t reg_addr, const uint8_t * data, uint32_t data_len, void *intf_ptr
)
{

	struct bme_spi_intf* spi_intf = intf_ptr;


	HAL_GPIO_WritePin(spi_intf->GPIO_Port, spi_intf->GPIO_Pin, GPIO_PIN_RESET);
	reg_addr &= ~(1 << 7);
	HAL_SPI_Transmit(spi_intf->spi, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(spi_intf->spi, (uint8_t*)data, data_len, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(spi_intf->GPIO_Port, spi_intf->GPIO_Pin, GPIO_PIN_SET);

	return 0;
}


static void bme_delay_us(uint32_t period, void *intf_ptr)
{
	if (period < 1000)
		period = 1;
	else
		period = period / 1000;

	HAL_Delay(period);
}



static BME280_INTF_RET_TYPE bme_spi_read_sr(
		uint8_t reg_addr, uint8_t * data, uint32_t data_len, void *intf_ptr
)
{


	struct bme_spi_intf_sr* spi_intf = intf_ptr;

	shift_reg_write_bit_16  (spi_intf->sr, spi_intf->sr_pin, 0);
	reg_addr |= (1 << 7);
	HAL_SPI_Transmit(spi_intf->spi, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(spi_intf->spi, data, data_len, HAL_MAX_DELAY);
	shift_reg_oe(spi_intf->sr, 1);
	shift_reg_write_bit_16(spi_intf->sr, spi_intf->sr_pin, 1);
	shift_reg_oe(spi_intf->sr, 0);

	return 0;
}


static BME280_INTF_RET_TYPE bme_spi_write_sr(
		uint8_t reg_addr, const uint8_t * data, uint32_t data_len, void *intf_ptr
)
{

	struct bme_spi_intf_sr* spi_intf = intf_ptr;


	shift_reg_write_bit_16(spi_intf->sr, spi_intf->sr_pin, 0);
	reg_addr &= ~(1 << 7);
	HAL_SPI_Transmit(spi_intf->spi, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(spi_intf->spi, (uint8_t*)data, data_len, HAL_MAX_DELAY);
	shift_reg_oe(spi_intf->sr, 1);
	shift_reg_write_bit_16(spi_intf->sr, spi_intf->sr_pin, 1);
	shift_reg_oe(spi_intf->sr, 0);


	return 0;
}


static void bme_delay_us_sr(uint32_t period, void *intf_ptr)
{
	if (period < 1000)
		period = 1;
	else
		period = period / 1000;

	HAL_Delay(period);
}






void bme_init_default( struct bme280_dev *bme, struct bme_spi_intf* spi_intf)
{
	// Настройка bme280 =-=-=-=-=-=-=-=-=-=-=-=-
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	(*bme).intf_ptr = spi_intf;
	(*bme).intf = BME280_SPI_INTF;
	(*bme).read = bme_spi_read;
	(*bme).write = bme_spi_write;
	(*bme).delay_us = bme_delay_us;

    bme280_soft_reset(bme);


	bme280_init(bme);


	(*bme).settings.osr_h = BME280_OVERSAMPLING_1X;
	(*bme).settings.osr_p = BME280_OVERSAMPLING_16X;
	(*bme).settings.osr_t = BME280_OVERSAMPLING_2X;
	(*bme).settings.filter = BME280_FILTER_COEFF_16;
	(*bme).settings.standby_time = BME280_STANDBY_TIME_0_5_MS;

	uint8_t settings_sel;
	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_FILTER_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	 bme280_set_sensor_settings(settings_sel, bme);
	 bme280_set_sensor_mode(BME280_NORMAL_MODE, bme);
}



void bme_init_default_sr( struct bme280_dev *bme, struct bme_spi_intf_sr* spi_intf)
{
	// Настройка bme280 =-=-=-=-=-=-=-=-=-=-=-=-
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	(*bme).intf_ptr = spi_intf;
	(*bme).intf = BME280_SPI_INTF;
	(*bme).read = bme_spi_read_sr;
	(*bme).write = bme_spi_write_sr;
	(*bme).delay_us = bme_delay_us_sr;

    bme280_soft_reset(bme);


	bme280_init(bme);


	(*bme).settings.osr_h = BME280_OVERSAMPLING_1X;
	(*bme).settings.osr_p = BME280_OVERSAMPLING_16X;
	(*bme).settings.osr_t = BME280_OVERSAMPLING_2X;
	(*bme).settings.filter = BME280_FILTER_COEFF_16;
	(*bme).settings.standby_time = BME280_STANDBY_TIME_0_5_MS;

	uint8_t settings_sel;
	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_FILTER_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	 bme280_set_sensor_settings(settings_sel, bme);
	 bme280_set_sensor_mode(BME280_NORMAL_MODE, bme);
}
struct bme280_data bme_read_data(struct bme280_dev *bme)
{
      	// Чтение данные из bme280
	struct bme280_data comp_data;
		 bme280_get_sensor_data(BME280_ALL, &comp_data, bme);
		 return comp_data;
}
#endif
