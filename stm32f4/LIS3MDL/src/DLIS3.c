#include <stdio.h>
#include "..\lis3mdl_reg.h"
#include "..\DLIS3.h"
#include <stm32f4xx_hal.h>
extern SPI_HandleTypeDef hspi1;


static int32_t lis3mdl_write(void * intf_ptr, uint8_t reg_addr, const uint8_t * data, uint16_t data_size)
{

	struct lis_spi_intf* spi_intf = intf_ptr;

	reg_addr=reg_addr&~(1<<7);
	reg_addr=reg_addr|(1<<6);

	HAL_GPIO_WritePin (spi_intf->GPIO_Port,spi_intf->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(spi_intf->spi, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(spi_intf->spi, (uint8_t*)data, data_size, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(spi_intf->GPIO_Port, spi_intf->GPIO_Pin, GPIO_PIN_SET);
	return 0;
}


static int32_t lis3mdl_read(void * intf_ptr, uint8_t reg_addr, uint8_t * data, uint16_t data_size)
{
	struct lis_spi_intf* spi_intf = intf_ptr;

	reg_addr=reg_addr|(1<<7);
	reg_addr=reg_addr|(1<<6);

	HAL_GPIO_WritePin (spi_intf->GPIO_Port, spi_intf->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(spi_intf->spi, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(spi_intf->spi, data, data_size, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(spi_intf->GPIO_Port, spi_intf->GPIO_Pin, GPIO_PIN_SET);
	return 0;
}



void lisset(stmdev_ctx_t *ctx)
{
// Настройка lismdl =-=-=-=-=-=-=-=-=-=-=-=-
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	ctx->handle = NULL;
	ctx->read_reg = lis3mdl_read;
	ctx->write_reg = lis3mdl_write;

	// Это придется делать прямо сразу еще до всего
	// так как иначе он с ним общаться не сможет (судя по доке по-крайней мере)
	lis3mdl_spi_mode_set(ctx, LIS3MDL_SPI_3_WIRE);

	uint8_t whoami_mag = 0x00;
	lis3mdl_device_id_get(ctx, &whoami_mag);

	// Убедились что датчик тот который нам нужен
	// Сбросим его
	lis3mdl_reset_set(ctx, PROPERTY_ENABLE);
	HAL_Delay(100);

	// Настраиваем
	// Обновление данных только целыми порциями
	lis3mdl_block_data_update_set(ctx, PROPERTY_ENABLE);
	// Без экономии энергии
	lis3mdl_fast_low_power_set(ctx, PROPERTY_DISABLE);
	// Диапазон измерения (внимание LSM303 умеет только 16G)
	lis3mdl_full_scale_set(ctx, LIS3MDL_16_GAUSS);
	// Частота опроса
	// внимание для LSM303 запрещены значения
	// LIS3MDL_LP_1kHz, LIS3MDL_MP_560Hz, LIS3MDL_HP_300Hz, LIS3MDL_UHP_155Hz
	lis3mdl_data_rate_set(ctx, LIS3MDL_UHP_80Hz);
	// Включаем температурный сенсор
	lis3mdl_temperature_meas_set(ctx, PROPERTY_ENABLE);
	// режим работы
	lis3mdl_operating_mode_set(ctx, LIS3MDL_CONTINUOUS_MODE);
}

void lisread(stmdev_ctx_t *ctx, float *temperature_celsius_mag, float (*mag)[3])
{
    int16_t temperataure_raw_mag;
	int16_t mag_raw[3];
	lis3mdl_magnetic_raw_get(ctx, mag_raw);
	lis3mdl_temperature_raw_get(ctx, &temperataure_raw_mag);
	*temperature_celsius_mag = lis3mdl_from_lsb_to_celsius(temperataure_raw_mag);
	for (int i = 0; i < 3; i++)
		(*mag)[i] = lis3mdl_from_fs16_to_gauss(mag_raw[i]);
}
