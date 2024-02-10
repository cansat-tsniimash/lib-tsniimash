/*
 * ADS1115.h
 *
 *  Created on: Dec 1, 2023
 *      Author: Роберт
 */

#ifndef ADS1115_ADS1115_H_
#define ADS1115_ADS1115_H_

#include <stm32f4xx_hal.h>

#ifdef HAL_I2C_MODULE_ENABLED

#define CONV_REG 0b00000000 //Регистр хранения результата преобразования
#define CONF_REG 0b00000001 //Конфигурационный регистр
#define LO_REG 0b00000010 //Регистр уставки, минимальное значение
#define HI_REG 0b00000011 //Регистр уставки, максимальное значение

typedef enum
{
	ADS1115_MUX_P0_NG = 4, //AINp=AIN0 и AINn=GND
	ADS1115_MUX_P1_NG = 5,
	ADS1115_MUX_P2_NG = 6,
	ADS1115_MUX_P3_NG = 7
} ads1115_mux_t;

typedef enum
{
	ADS1115_PGA_FS_6p144 = 0, //диапозон измерений
	ADS1115_PGA_FS_4p096 = 1,
	ADS1115_PGA_FS_2p048 = 2,
	ADS1115_PGA_FS_1p024 = 3,
	ADS1115_PGA_FS_0p512 = 4,
	ADS1115_PGA_FS_0p256 = 5
} ads1115_pga_t;

typedef enum
{
	ADS1115_OS_RESET = 0,
	ADS1115_OS_SET = 1
} ads1115_os_t;

typedef enum
{
	ADS1115_MODE_CONTINOUS = 0,
	ADS1115_MODE_SINGLE = 1
} ads1115_mode_t;

typedef enum
{
	ADS1115_DR_8HZ = 0,
	ADS1115_DR_16HZ = 1,
	ADS1115_DR_32HZ = 2,
	ADS1115_DR_64HZ = 3,
	ADS1115_DR_128HZ = 4,
	ADS1115_DR_250HZ = 5,
	ADS1115_DR_475HZ = 6,
	ADS1115_DR_870HZ = 7
} ads1115_dr_t;


/*   Структура описывает внешний АЦП ADS1115
 *   hi2c - указатель на дискриптор(структура-описатель) i2c из библиотеки стм. Надо заполнить самим до использования библиотеки.
 *   DevAddress - Адрес устройстава на шине i2c(7 бит) со сдвигом << 1. Надо заполнить самим до использования библиотеки.
 *   conf_reg - Последнее полученое значение Config register(Регистра настроек). Заполнять самим не надо.
 */
typedef struct
{
	I2C_HandleTypeDef *hi2c;
	uint8_t DevAddress;
	uint16_t conf_reg;
} ads1115_t;


/*  Функция первичной настройки датчика
 *  ads - указатель на дискриптор(структура-описатель) внешннего АЦП ADS1115
 *  return: код ошибки из библиотеки стм
 */
int ads1115_init(ads1115_t *ads);

/*  функция запроса одного измерения
 *  ads - указатель на дискриптор(структура-описатель) внешннего АЦП ADS1115
 *  return: код ошибки из библиотеки стм
 */
int ads1115_req_single(ads1115_t *ads);


/*  функция считывания результата одного измерения
 *  ads - указатель на дискриптор(структура-описатель) внешннего АЦП ADS1115
 *  ptr - указатель на переменную, куда будет записан результат
 *  return: код ошибки из библиотеки стм + -1 - результат не готов, -2 - выбран неверный режим
 */
int ads1115_read_single(ads1115_t *ads, uint16_t *ptr);

/*  функция пересчёта сырых значений в вольты
 *  ads - указатель на дискриптор(структура-описатель) внешннего АЦП ADS1115
 *  volt - сырые значения
 *  return: вольты
 */
float ads1115_convert(ads1115_t *ads, uint16_t volt);

/* функция настройки мультиплексора
  */
int ads1115_write_mux(ads1115_mux_t mux, ads1115_t *ads);

#endif // HAL_I2C_MODULE_ENABLED

#endif /* ADS1115_ADS1115_H_ */
