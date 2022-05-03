#include <stdio.h>
#include "..\lsm6ds3_reg.h"
#include "..\DLSM.h"
#include <stm32f1xx_hal.h>


#ifdef HAL_SPI_MODULE_ENABLED
extern SPI_HandleTypeDef hspi1;

static int32_t lsmd6s3_write(void * intf_ptr, uint8_t reg_addr, const uint8_t * data, uint16_t data_size)
{
	struct lsm_spi_intf* spi_intf = intf_ptr;


	reg_addr=reg_addr&~(1<<7);

	HAL_GPIO_WritePin (spi_intf->GPIO_Port, spi_intf->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(spi_intf->spi, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(spi_intf->spi, (uint8_t*)data, data_size, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(spi_intf->GPIO_Port, spi_intf->GPIO_Pin, GPIO_PIN_SET);
	return 0;
}


static int32_t lsm6ds3_read(void * intf_ptr, uint8_t reg_addr, uint8_t * data, uint16_t data_size)
{
	struct lsm_spi_intf* spi_intf = intf_ptr;


	reg_addr=reg_addr|(1<<7);

	HAL_GPIO_WritePin (spi_intf->GPIO_Port, spi_intf->GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(spi_intf->spi, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(spi_intf->spi, data, data_size, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(spi_intf->GPIO_Port, spi_intf->GPIO_Pin, GPIO_PIN_SET);
	return 0;
}



static int32_t lsmd6s3_write_sr(void * intf_ptr, uint8_t reg_addr, const uint8_t * data, uint16_t data_size)
{
	struct lsm_spi_intf_sr* spi_intf = intf_ptr;


	reg_addr=reg_addr&~(1<<7);

	shift_reg_write_bit_16(spi_intf->sr, spi_intf->sr_pin, 0);
	HAL_SPI_Transmit(spi_intf->spi, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(spi_intf->spi, (uint8_t*)data, data_size, HAL_MAX_DELAY);
	shift_reg_oe(spi_intf->sr, 1);
	shift_reg_write_bit_16  (spi_intf->sr, spi_intf->sr_pin, 1);
	shift_reg_oe(spi_intf->sr, 0);

	return 0;
}


static int32_t lsm6ds3_read_sr(void * intf_ptr, uint8_t reg_addr, uint8_t * data, uint16_t data_size)
{
	struct lsm_spi_intf_sr* spi_intf = intf_ptr;


	reg_addr=reg_addr|(1<<7);

	shift_reg_write_bit_16  (spi_intf->sr, spi_intf->sr_pin, 0);
	HAL_SPI_Transmit(spi_intf->spi, &reg_addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(spi_intf->spi, (uint8_t*)data, data_size, HAL_MAX_DELAY);
	shift_reg_oe(spi_intf->sr, 1);
	shift_reg_write_bit_16  (spi_intf->sr, spi_intf->sr_pin, 1);
	shift_reg_oe(spi_intf->sr, 0);


	return 0;
}


void lsmset(stmdev_ctx_t *ctx, struct lsm_spi_intf *spi_interface)
{
	// Настройка lsm6ds3 =-=-=-=-=-=-=-=-=-=-=-=-
		// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

		ctx->handle = spi_interface;
		ctx->read_reg = lsm6ds3_read;
		ctx->write_reg = lsmd6s3_write;

		uint8_t whoami = 0x00;
		lsm6ds3_device_id_get(ctx, &whoami);

		lsm6ds3_reset_set(ctx, PROPERTY_ENABLE);
		HAL_Delay(100);

		lsm6ds3_xl_full_scale_set(ctx, LSM6DS3_16g);
		lsm6ds3_xl_data_rate_set(ctx, LSM6DS3_XL_ODR_104Hz);

		lsm6ds3_gy_full_scale_set(ctx, LSM6DS3_2000dps);
		lsm6ds3_gy_data_rate_set(ctx, LSM6DS3_GY_ODR_104Hz);
}

void lsmset_sr(stmdev_ctx_t *ctx, struct lsm_spi_intf_sr *spi_interface)
{
	// Настройка lsm6ds3 =-=-=-=-=-=-=-=-=-=-=-=-
		// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

		ctx->handle = spi_interface;
		ctx->read_reg = lsm6ds3_read_sr;
		ctx->write_reg = lsmd6s3_write_sr;

		uint8_t whoami = 0x00;
		lsm6ds3_device_id_get(ctx, &whoami);

		lsm6ds3_reset_set(ctx, PROPERTY_ENABLE);
		HAL_Delay(100);

		lsm6ds3_xl_full_scale_set(ctx, LSM6DS3_16g);
		lsm6ds3_xl_data_rate_set(ctx, LSM6DS3_XL_ODR_104Hz);

		lsm6ds3_gy_full_scale_set(ctx, LSM6DS3_2000dps);
		lsm6ds3_gy_data_rate_set(ctx, LSM6DS3_GY_ODR_104Hz);
}
void lsmread(stmdev_ctx_t *ctx, float *temperature_celsius_gyro, float (*acc_g)[3], float (*gyro_dps)[3])
{
	// Чтение данных из lsm6ds3
		// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
		int16_t temperature_raw_gyro = {0};
		int16_t acc_raw[3] = {0};
		int16_t gyro_raw[3] = {0};
		lsm6ds3_temperature_raw_get(ctx, &temperature_raw_gyro);
		lsm6ds3_acceleration_raw_get(ctx, acc_raw);
		lsm6ds3_angular_rate_raw_get(ctx, gyro_raw);

		// Пересчет из попугаев в человеческие величины
		*temperature_celsius_gyro = lsm6ds3_from_lsb_to_celsius(temperature_raw_gyro);
		for (int i = 0; i < 3; i++)
		{
			(*acc_g)[i] = lsm6ds3_from_fs16g_to_mg(acc_raw[i]) / 1000;
			(*gyro_dps)[i] = lsm6ds3_from_fs2000dps_to_mdps(gyro_raw[i]) / 1000;
		}
}
#endif
