#ifndef SHIFT_REG_H_
#define SHIFT_REG_H_

#include <stdint.h>
#include <stdbool.h>
#include <stm32f4xx_hal.h>

#ifdef HAL_SPI_MODULE_ENABLED

//Так выглядит дескриптор сдвигового регистра
typedef struct {
	SPI_HandleTypeDef *bus; // Хэндлер шины SPI
	GPIO_TypeDef *latch_port; // Порт Latch-а, например, GPIOA, GPIOB, etc
	uint16_t latch_pin; // Маска Latch-а, например, GPIO_Pin_1, GPIO_Pin_2, etc
	GPIO_TypeDef *oe_port; // Порт OE, например, GPIOA, GPIOB, etc
	uint16_t oe_pin; // Маска OE, например, GPIO_Pin_1, GPIO_Pin_2, etc
	uint16_t value; // Текущее состояние сдвигового регистра
} shift_reg_t;

// Инициализация шифт регистра, передаём дескриптор сдвигового регистра
void shift_reg_init(shift_reg_t *this);
// Функция для передачи в сдвиговый регистр одного байта
void shift_reg_push_8(shift_reg_t *this, uint8_t buffer);
// Функция для передачи в сдвиговый регистр двух байт
void shift_reg_push_16(shift_reg_t *this, uint16_t buffer);
// Функция для применения изменений
void shift_reg_latch(shift_reg_t *this);
// Функция для поднятия/опускания OE
void shift_reg_oe(shift_reg_t *this, bool onoff); // true значит выключить, false - включить
// Функция для передачи в сдвиговый регистр одного байта с последующим применением изменений
void shift_reg_write_8(shift_reg_t *this, uint8_t buffer);
// Функция для передачи в сдвиговый регистр двух байт с последующим применением изменений
void shift_reg_write_16(shift_reg_t *this, uint16_t buffer);
// Функция для вставки нужного бита в младший байт на позицию pos, при этом данные из первого байта сдвинутся во второй. Может привести к неожиданным последствиям
void shift_reg_write_bit_8(shift_reg_t *this, uint8_t pos, bool onoff);
// Функция для вставки нужного бита в два байта на позицию pos
void shift_reg_write_bit_16(shift_reg_t *this, uint16_t pos, bool onoff);

#endif /* HAL_SPI_MODULE_ENABLED */
#endif /* SHIFT_REG_H_ */