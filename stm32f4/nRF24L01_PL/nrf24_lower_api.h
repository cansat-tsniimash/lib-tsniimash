#ifndef NRF24_LOWER_API_H_
#define NRF24_LOWER_API_H_

#include <stm32f4xx_hal.h>
#ifdef HAL_SPI_MODULE_ENABLED

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "nrf24_lower_api_stm32.h"

#include "nrf24_defs.h"

struct nrf24_lower_api_config_t;
typedef struct nrf24_lower_api_config_t nrf24_lower_api_config_t;

// Чтение данных из регистров NRF24
/* Аргументы:
   intf_ptr - указатель на структуру, содержащую настройки lower_api
   reg_addr - адрес читаемого регистра
   reg_data - указатель на буфер в который кладутся прочитанные данные
   data_size - размер буфера для прочитанных данных */
void nrf24_read_register(void * intf_ptr, uint8_t reg_addr, uint8_t * reg_data, size_t data_size);

// Запись данных в регистры NRF24
/* Аргументы:
   intf_ptr - указатель на структуру, содержащую настройки lower_api
   reg_addr - адрес записываемого регистра
   reg_data - указатель на буфер с записываемыми данными
   data_size - размер буфера с записываемыми данными */
void nrf24_write_register(void * intf_ptr, uint8_t reg_addr, const uint8_t * reg_data, size_t data_size);

// Чтение полученного пакета
/* Аргументы:
 * intf_ptr - указатель на структуру, содержащую настройки lower_api
   payload_buffer - буфер в который кладутся байты прочитанного пакета
   payload_buffer_size - размер буфера payload_buffer.

   Если полученный пакет не влезает в payload_buffer, то будет записано только то, что влезло
   пакет удалется из NRF24 при чтении. А  максимум он может быть 32 байта.
   Для получения длины полученного пакета можно использовать nrf24_get_rx_payload_size*/
void nrf24_read_rx_payload(void * intf_ptr, uint8_t * payload_buffer, size_t payload_buffer_size);

// Запись пакета для отправки
/* Аргументы:
   intf_ptr - указатель на структуру, содержащую настройки lower_api
   payload_buffer - указатель на буфер с отправляемым пакетом
   payload_size - размер отправляемого пакета в буфере payload_buffer
   use_ack - нужно ли использовать ACK для этого пакета?

   Эта функция реализует две команды: W_TX_PAYLOAD_NOACK и W_TX_PAYLOAD */
void nrf24_write_tx_payload(void * intf_ptr, const uint8_t * payload_buffer, size_t payload_size, bool use_ack);

// Сброс всех данных из буферов RF на отправку
/* Аргументы:
   intf_ptr - указатель на структуру, содержащую настройки lower_api*/
void nrf24_flush_tx(void * intf_ptr);

// Сброс всех полученных nrf24 пакетов
/* Аргументы:
   intf_ptr - указатель на структуру, содержащую настройки lower_api*/
void nrf24_flush_rx(void * intf_ptr);

// Повтор предыдущего отправляемого пакета
/* Аргументы:
   intf_ptr - указатель на структуру, содержащую настройки lower_api*/
void nrf24_ruse_tx_pl(void * intf_ptr);

// Получает из радио размер полученного пакета
/* Аргументы:
   intf_ptr - указатель на структуру, содержащую настройки lower_api
   payload_size - указатель на переменную, в которую функция запишет размер полученного пакета*/
/* Документация говорит, что нужно сделать flush_rx (или он был сделан?)
   Если прочитался размер больше 32ух байт.
   Предполагаю что для пустого FIFO прочитается ноль */
void nrf24_get_rx_payload_size(void * intf_ptr, uint8_t * payload_size);

// Запись пакета для отправки вместе с очередным ACK пакетом
/* Аргументы:
   intf_ptr - указатель на структуру, содержащую настройки lower_api
   payload - указатель на буфер с отправляемым пакетом
   payload_size - размер отправляемого пакета в буфере payload_buffer
   pipe - номер пайпа*/
void nrf24_write_ack_payload(void * intf_ptr, const uint8_t * payload, size_t payload_size, uint8_t pipe);

// Получение статуса устройства
/* Аргументы:
   intf_ptr - указатель на структуру, содержащую настройки lower_api*/
void nrf24_get_status(void * intf_ptr, uint8_t * status);

// Управление CE пином модуля.
/* Аргументы:
   intf_ptr - указатель на структуру, содержащую настройки lower_api
   onoff - значение true - CE поднят к 1; false - CE прижат к полу */
void nrf24_ce_activate(void * intf_ptr, bool onoff);

#endif /* HAL_SPI_MODULE_ENABLED */
#endif /* NRF24_LOWER_API_H_ */
