#include <stm32f4xx_hal.h>


#ifdef HAL_SPI_MODULE_ENABLED


#include <stdio.h>
#include "../bme280.h"
#include "../DriverForBME280.h"



// Наши личные непубличные функции
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


// Функция для управления чипселектом напрямую
static void spi_chip_select_direct(bme_intf_spi * device_, bool active)
{
	// Кастуем дескриптор в правильный тип
	bme_intf_spi_cs_direct * device = (bme_intf_spi_cs_direct *)device_;

	HAL_GPIO_WritePin(
			device->cs_port,
			device->cs_pin,
			active ? GPIO_PIN_RESET : GPIO_PIN_SET
	);
}


// Функция для управления чипселектом через сдвиговый регистр
static void spi_chip_select_shiftreg(bme_intf_spi * device_, bool active)
{
	// Кастуем дескриптор в правильный тип
	bme_intf_spi_cs_shiftreg * device = (bme_intf_spi_cs_shiftreg *)device_;

	// Выключаем OE (делаем единицей)
	HAL_GPIO_WritePin(device->oe_port, device->oe_pin, GPIO_PIN_SET);

	// Формируем значение которое будем загонять
	uint16_t reg_value;
	if (active)
		reg_value = ~device->cs_mask;
	else
		reg_value = 0xFFFF; // Все в единицу

	// Загоняем значение
	HAL_SPI_Transmit(device->shift_reg_bus, (uint8_t*)&reg_value, sizeof(reg_value), HAL_MAX_DELAY);

	// Дергаем latch для фиксации значений на сдвиговом регистре
	HAL_GPIO_WritePin(device->latch_port, device->latch_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(device->latch_port, device->latch_pin, GPIO_PIN_RESET);

	// Включаем OE (делаем нулём)
	HAL_GPIO_WritePin(device->oe_port, device->oe_pin, GPIO_PIN_RESET);
}


// Функция для чтения регистра через SPI
static BME280_INTF_RET_TYPE bme_spi_read(
		uint8_t reg_addr, uint8_t * data, uint32_t data_len, void *intf_ptr
)
{
	// Колдуем intf_ptr в наш дескриптор
	bme_intf_spi * device = (bme_intf_spi*)intf_ptr;

	//Активируем CS
	device->chip_select(device, true);

	// Загоняем адрес регистра, который мы хотим почитать
	reg_addr |= (1 << 7); // Старший бит в 1 показывает что мы хотим читать
	HAL_SPI_Transmit(device->bus, &reg_addr, 1, HAL_MAX_DELAY);

	// Читаем данные
	HAL_SPI_Receive(device->bus, data, data_len, HAL_MAX_DELAY);

	// Выключаем чипселект
	device->chip_select(device, false);

	return 0;
}


static BME280_INTF_RET_TYPE bme_spi_write(
		uint8_t reg_addr, const uint8_t * data, uint32_t data_len, void *intf_ptr
)
{
	// Колдуем intf_ptr в наш дескриптор
	bme_intf_spi * device = (bme_intf_spi*)intf_ptr;

	//Активируем CS
	device->chip_select(device, true);

	// Загоняем адрес записываемого регистра
	reg_addr &= ~(1 << 7); // 0 в старшем бите показывает что мы хотим писать
	HAL_SPI_Transmit(device->bus, &reg_addr, 1, HAL_MAX_DELAY);

	// Гоним записываемые данные
	HAL_SPI_Transmit(device->bus, (uint8_t*)data, data_len, HAL_MAX_DELAY);

	// Выключаем CS
	device->chip_select(device, false);

	return 0;
}


// Функция для паузы в микросекунды
static void bme_delay_us(uint32_t period, void *intf_ptr)
{
	if (period < 1000)
		period = 1;
	else
		period = period / 1000;

	HAL_Delay(period);
}



// Публичные функции
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

int bme_ctor_spi_cs_direct(
	bme_intf_spi_cs_direct * device,	/* Дескриптор устройства */
	SPI_HandleTypeDef * bus, 	/* SPI Шина на которой сидит датчик */
	GPIO_TypeDef * cs_port,		/* порт на котором сидит CS пин */
	uint16_t cs_pin				/* собственно CS пин */
)
{
	// Заполняем дексриптор
	// Начнем с официального драйвера
	device->base.base.bme.intf = BME280_SPI_INTF; // Мы работаем через SPI
	device->base.base.bme.delay_us = bme_delay_us;
	device->base.base.bme.read = bme_spi_read;
	device->base.base.bme.write = bme_spi_write;
	// Запоминаем саму структуру как intf указатель для передчи в read/write
	device->base.base.bme.intf_ptr = device;

	// Теперь заполняем то, что касается уровня абстракции SPI
	device->base.bus = bus;
	device->base.chip_select = spi_chip_select_direct;

	// А теперь заполняем то что касается нашего способа управления чип селектом
	device->cs_port = cs_port;
	device->cs_pin = cs_pin;

	// Готово!
	return 0;
}


int bme_ctor_spi_cs_shiftreg(
	bme_intf_spi_cs_shiftreg * device,	/* Дескриптор устройства */
	SPI_HandleTypeDef * bus,			/* SPI Шина на которой сидит датчик */
	SPI_HandleTypeDef * shiftreg_bus,	/* SPI Шина на которой сидит сдвиговый регистр */
	GPIO_TypeDef * latch_port,			/* Порт на котором сидит latch пин сдвигового регистра */
	uint16_t latch_pin,					/* Пин latch сдвигового регистра */
	GPIO_TypeDef * oe_port,				/* Порт на котором сидит oe пин сдвигого регистра */
	uint16_t oe_pin,					/* oe пин сдвигового регистра */
	uint16_t cs_mask					/* Маска чипселекта */
)
{
	// Заполняем дексриптор
	// Начнем с официального драйвера
	device->base.base.bme.intf = BME280_SPI_INTF; // Мы работаем через SPI
	device->base.base.bme.delay_us = bme_delay_us;
	device->base.base.bme.read = bme_spi_read;
	device->base.base.bme.write = bme_spi_write;
	// Запоминаем саму структуру как intf указатель для передчи в read/write
	device->base.base.bme.intf_ptr = device;

	// Теперь заполняем то, что касается уровня абстракции SPI
	device->base.bus = bus;
	device->base.chip_select = spi_chip_select_shiftreg;

	// А теперь заполняем то что касается нашего способа управления чип селектом
	device->shift_reg_bus = shiftreg_bus;
	device->latch_port = latch_port;
	device->latch_pin = latch_pin;
	device->oe_port = oe_port;
	device->oe_pin = oe_pin;
	device->cs_mask = cs_mask;

	// Готово!
	return 0;
}



int bme_setup_default(bme_intf_base * device)
{
	// Настройка bme280 =-=-=-=-=-=-=-=-=-=-=-=-
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	struct bme280_dev * bme = &device->bme;

	bme280_soft_reset(bme);
	bme280_init(bme);
	bme->settings.osr_h = BME280_OVERSAMPLING_1X;
	bme->settings.osr_p = BME280_OVERSAMPLING_16X;
	bme->settings.osr_t = BME280_OVERSAMPLING_2X;
	bme->settings.filter = BME280_FILTER_COEFF_16;
	bme->settings.standby_time = BME280_STANDBY_TIME_0_5_MS;

	uint8_t settings_sel;
	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_FILTER_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	bme280_set_sensor_settings(settings_sel, bme);
	bme280_set_sensor_mode(BME280_NORMAL_MODE, bme);

	return 0;
}


struct bme280_data bme_read_data(bme_intf_base * device)
{
	struct bme280_dev * bme = &device->bme;
	struct bme280_data comp_data;
	bme280_get_sensor_data(BME280_ALL, &comp_data, bme);
	return comp_data;
}


#endif
