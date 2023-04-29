/*
 * MX25L512.h
 *
 *  Created on: 21 янв. 2023 г.
 *      Author: Install
 */

#include "Shift_Register/shift_reg.h"

#ifndef MX25L512__MX25L512_H_
#define MX25L512__MX25L512_H_

#define MX25L512_WRSR 0x01
#define MX25L512_PP 0x02
#define MX25L512_READ 0x03
#define MX25L512_WRDI 0x04
#define MX25L512_RDSR 0x05
#define MX25L512_WREN 0x06
#define MX25L512_RDID 0x9F
#define MX25L512_FAST_READ 0x0B
#define MX25L512_SE 0x20
#define MX25L512_BE 0x52
#define MX25L512_CE 0x60
#define MX25L512_DP 0xB9
#define MX25L512_RDP 0xAB
#define MX25L512_RES 0xAB
#define MX25L512_REMS 0x90

#define MX25L512_DUMMY 0xAAAA
#define MX25L512_MANUFACTURER_ID_FIRST 0x00
#define MX25L512_MX25L512_DEVICE_ID_FIRST 0x01

#define MX25L512_TIMEOUT HAL_MAX_DELAY

typedef struct bus_s
{
	void (*mx25l512_CS)(void * intf_ptr, bool mode);
	SPI_HandleTypeDef *hspi;
	void *intf_ptr;
}bus_t;

// Структура, содержащая параметры SPI пиноа SPI Chip Select
typedef struct mx25l512_spi_pins_t
{
	// Настройки SPI Chip Select
	GPIO_TypeDef *cs_port;
	uint16_t cs_pin;
} mx25l512_spi_pins_t;

// Структура, содержащая параметры SPI пина SPI Chip Select для сдвигового регистра
typedef struct mx25l512_spi_pins_sr_t
{
	shift_reg_t *this;
	// Настройки SPI Chip Select
	uint8_t pos_CS;
} mx25l512_spi_pins_sr_t;


void mx25l512_spi_init(bus_t *bus, SPI_HandleTypeDef *hspi, mx25l512_spi_pins_t *pins);
void mx25l512_spi_init_sr(bus_t *bus, SPI_HandleTypeDef *hspi, mx25l512_spi_pins_sr_t *pins);

//Разрешение записи (нужно для работы PP, SE, BE, CE, WRSR)
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 */
void mx25l512_wren(bus_t *bus);


//Запрет записи
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 */
void mx25l512_wrdi(bus_t *bus);


//Чтение производителя, типа памяти, индивидуального номера чипа
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 * pData - указатель на переменную, куда записать данные
 */
void mx25l512_rdid(bus_t *bus, uint8_t *pData);


//Чтение битов статуса
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 * pData - указатель на переменную, куда записать данные
 */
void mx25l512_rdsr(bus_t *bus, uint8_t *pData);


//Изменение битов статуса
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 * Data - указатель на переменную, какие данные записать
 */
void mx25l512_wrsr(bus_t *bus, uint16_t *data);


//Чтение данных
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 * addr - указатель на адрес, откуда нужно читать данные. Первый байт ВСЕГДА 0x00
 * pData - указатель на переменную, куда записать данные
 * size - желаемый размер получаемых данных
 */
void mx25l512_read(bus_t *bus, uint32_t *addr, uint8_t *pData, size_t size);


//Быстрое чтение
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 * addr - указатель на адрес, откуда нужно читать данные. Первый байт ВСЕГДА 0x00
 * pData - указатель на переменную, куда записать данные
 * size - желаемый размер получаемых данных
 */
void mx25l512_fast_read(bus_t *bus, uint32_t *addr, uint8_t *pData, size_t size);


//Затирание сектора
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 * addr - указатель на адрес сектора, который нужно стереть. Первый байт ВСЕГДА 0x00
 */
void mx25l512_SE(bus_t *bus, uint32_t *addr);


//Затирание блока
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 * addr - указатель на адрес блока, который нужно стереть. Первый байт ВСЕГДА 0x00
 */
void mx25l512_BE(bus_t *bus, uint32_t *addr);


//Затирание чипа целиком
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 */
void mx25l512_CE(bus_t *bus);


//Запись данных
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 * addr - указатель на адрес, куда нужно записать данные. Первый байт ВСЕГДА 0x00
 * Data - указатель на структуру, какие данные записать
 * size - размер записываемых данных
 */
void mx25l512_PP(bus_t *bus, uint32_t *addr, uint8_t *Data, size_t size);


//Перевод чипа в deep power-down
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 */
void mx25l512_DP(bus_t *bus);


//Вывод чипа из deep power-down
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 */
void mx25l512_RDP(bus_t *bus);


//Чтение битов статуса в deep-power-down НЕ РЕКОМЕНДУЕТСЯ ИСПОЛЬЗОВАТЬ
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 * pData - указатель на переменную, куда записать данные
 */
void mx25l512_RES(bus_t *bus, uint8_t *pData);


//Чтение ID производителя и устройства
/*
 * bus_t - указатель на структуру, содержащую настройки CS
 * pData - указатель на переменную, куда записать данные
 */
void mx25l512_REMS(bus_t *bus, uint8_t *pData, bool wgf); //1 - manufacturer's ID first, 0 - device ID first

#endif /* MX25L512__MX25L512_H_ */
