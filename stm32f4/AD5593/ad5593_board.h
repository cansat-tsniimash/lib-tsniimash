#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


//! Дескриптор для работы с шинами
/*! Каждая платфома определяет эту структуру по-своему */
struct ad5593_board_t;
typedef struct ad5593_board_t ad5593_board_t;


//! Создание объекта абстракции доступа к датчику
/*! user_arg - всякие нужные для платфомы штуки */
int ad5593_brd_ctor(ad5593_board_t ** brd, void * user_arg);

//! Удаление объекта абстракции доступа к датчику
void ad5593_brd_dtor(ad5593_board_t * brd);

//! Запись данных в "регистр"
/*! addr - i2с адрес ведомого выровненый по младшему биту */
int ad5593_brd_write(ad5593_board_t * brd, uint8_t addr, uint8_t pointer, uint16_t data);

//! Чтение данных из "регистра"
/*! addr - i2с адрес ведомого выровненый по младшему биту */
int ad5593_brd_read(ad5593_board_t * brd, uint8_t addr, uint8_t pointer, uint16_t * data);
