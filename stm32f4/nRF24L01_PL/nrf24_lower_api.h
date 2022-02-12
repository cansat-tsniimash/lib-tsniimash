/*
 * nrf24_lower_api.h
 *
 *  Created on: 29 янв. 2022 г.
 *      Author: 1
 */

#ifndef NRF24_LOWER_API_H_
#define NRF24_LOWER_API_H_

#include <stm32f4xx_hal.h>
#ifdef HAL_SPI_MODULE_ENABLED

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define RF24_REGADDR_CONFIG			0x00
#define RF24_REGADDR_EN_AA			0x01
#define RF24_REGADDR_EN_RXADDR		0x02
#define RF24_REGADDR_SETUP_AW		0x03
#define RF24_REGADDR_SETUP_RETR		0x04
#define RF24_REGADDR_RF_CH			0x05
#define RF24_REGADDR_RF_SETUP		0x06
#define RF24_REGADDR_STATUS			0x07
#define RF24_REGADDR_OBSERVE_TX		0x08
#define RF24_REGADDR_RPD			0x09
#define RF24_REGADDR_RX_ADDR_P0		0x0A
#define RF24_REGADDR_RX_ADDR_P1		0x0B
#define RF24_REGADDR_RX_ADDR_P2		0x0C
#define RF24_REGADDR_RX_ADDR_P3		0x0D
#define RF24_REGADDR_RX_ADDR_P4		0x0E
#define RF24_REGADDR_RX_ADDR_P5		0x0F
#define RF24_REGADDR_TX_ADDR		0x10
#define RF24_REGADDR_RX_PW_P0		0x11
#define RF24_REGADDR_RX_PW_P1		0x12
#define RF24_REGADDR_RX_PW_P2		0x13
#define RF24_REGADDR_RX_PW_P3		0x14
#define RF24_REGADDR_RX_PW_P4		0x15
#define RF24_REGADDR_RX_PW_P5		0x16
#define RF24_REGADDR_FIFO_STATUS	0x17
#define RF24_REGADDR_DYNPD			0x1C
#define RF24_REGADDR_FEATURE		0x1D

// Есть данные в RX буферах
#define RF24_STATUS_RX_DR	(1 << 6)
// Данные из TX буфера отправлены
#define RF24_STATUS_TX_DS	(1 << 5)
// Превышено количество попыток отправки данных
#define RF24_STATUS_MAX_RT	(1 << 4)
// Нет данных в TX буферах
#define RF24_STATUS_TX_FULL	(1 << 0)

// Это поле в статусе идет не одни битом а числом, поэтому для них можно сделать функции
// Я для разнообразия сделаю сложный макрос с параметром

// Номер пайпа, пакет с которого ожидает выгребания в RX FIFO
#define RF24_STATUS_RX_P_NO_GET(status_reg) ((status_reg >> 1) & (0x07)) // 0x07 это 111 в двоичной


// Прерывание по получению данных
#define RF24_CONFIG_MASK_RX_DR	(1 << 6)
// Прерывание по успешной отправке данных
#define RF24_CONFIG_MASK_TX_DR	(1 << 5)
// Прерывание по проавлу отправки данных
#define RF24_CONFIG_MASK_MAX_RT	(1 << 4)
// Включение CRC (всегда включено, если включены авто-ACK)
#define RF24_CONFIG_EN_CRC		(1 << 3)
// Длина CRC (0 - 1 байт, 1 - 2 байта)
#define RF24_CONFIG_CRCO		(1 << 2)
// Включение модуля (если 0, то модуль спит)
#define RF24_CONFIG_PWR_UP		(1 << 1)
// Упрвавление режимом - приём или передача
#define RF24_CONFIG_PRIM_RX		(1 << 0)


#define RF24_RFSETUP_CONT_WAVE	(1 << 7)
#define RF24_RFSETUP_RF_DR_LOW	(1 << 5)
#define RF24_RFSETUP_PLL_LOCK	(1 << 4)
#define RF24_RFSETUP_RF_DR_HIGH	(1 << 3)
#define RF24_RFSETUP_RF_PWR_OFFSET	1
#define RF24_RFSETUP_RF_PWR_MASK	0x03


#define RF24_FEATURE_EN_DPL		(1 << 2)
#define RF24_FEATURE_EN_ACK_PAY	(1 << 1)
#define RF24_FEATURE_EN_DYN_ACK	(1 << 0)

#define RF24_NOP 0xFF
#define RF24_FLUSH_TX	0b11100001
#define RF24_FLUSH_RX	0b11100010
#define RF24_W_TX_PAYLOAD 0b10100000
#define RF24_W_TX_PAYLOAD_NO_ACK 0b10110000
#define RF24_R_RX_PAYLOAD 0b01100001
#define RF24_R_RX_PL_WID 0b01100000

// Чтение данных из регистров RF24
/* Аргументы:
   reg_addr - адрес читаемого регистра
   reg_data - указатель на буфер в который кладутся прочитанные данные
   data_size - размер буфера для прочитанных данных */
void rf24_read_register(uint8_t reg_addr, uint8_t * reg_data, size_t data_size);

// Запись данных в регистры RF24
/* Аргументы:
   reg_addr - адрес записываемого регистра
   reg_data - указатель на буфер с записываемыми данными
   data_size - размер буфера с записываемыми данными */
void rf24_write_register(uint8_t reg_addr, const uint8_t * reg_data, size_t data_size);


// Чтение полученного пакета
/* Аргументы:
   payload_buffer - буфер в который кладутся байты прочитанного пакета
   payload_buffer_size - размер буфера payload_buffer.

   Если полученный пакет не влезает в payload_buffer, то будет записано только то, что влезло
   пакет удалется из RF24 при чтении. А  максимум он может быть 32 байта.
   Для получения длины полученного пакета можно использовать */
void rf24_read_rx_payload(uint8_t * payload_buffer, size_t payload_buffer_size);


// Запись пакета для отправки
/* Аргументы:
   payload_buffer - указатель на буфер с отправляемым пакетом
   payload_size - размер отправляемого пакета в буфере payload_buffer
   use_ack - нужно ли использовать ACK для этого пакета?

   Эта функция реализует две команды: W_TX_PAYLOAD_NOACK и W_TX_PAYLOAD */
void rf24_write_tx_payload(const uint8_t * payload_buffer, size_t payload_size, bool use_ack);


// Сброс всех данных из буферов RF на отправку
void rf24_flush_tx(void);


// Сброс всех полученных rf24 пакетов
void rf24_flush_rx(void);


// Повтор предыдущего отправляемого пакета
void rf24_ruse_tx_pl(void);


// Получает из радио размер полученного пакета
/* Документация говорит, что нужно сделать flush_rx (или он был сделан?)
   Если прочитался размер больше 32ух байт.
   Предполагаю что для пустого FIFO прочитается ноль? */
void rf24_get_rx_payload_size(uint8_t * payload_size);


// Запись пакета для отправки вместе с очередным ACK пакетом
void rf24_write_ack_payload(const uint8_t * payload, size_t payload_size);


// Получение статуса устройства
void rf24_get_status(uint8_t * status);

// Управление CE пином модуля.
/* onoff: значение true - CE прижат к полу; false - CE поднят к 1 */
void rf24_ce_activate(bool onoff);

#endif /* HAL_SPI_MODULE_ENABLED */
#endif /* NRF24_LOWER_API_H_ */
