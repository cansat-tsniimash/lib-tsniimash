#include "../shift_reg.h"
#ifdef HAL_SPI_MODULE_ENABLED

#include "Delay/delay.h"

void shift_reg_init(shift_reg_t *this) {
	HAL_GPIO_WritePin(this->latch_port, this->latch_pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(this->oe_port, this->oe_pin, GPIO_PIN_RESET);
	this->value = 0;
}

void shift_reg_push_8(shift_reg_t *this, uint8_t buffer) {
	HAL_SPI_Transmit(this->bus, &buffer, 1, HAL_MAX_DELAY);
	this->value = (this->value << 8) | buffer;
}

void shift_reg_push_16(shift_reg_t *this, uint16_t buffer) {
	uint16_t next_buffer = ((buffer & 0x00FF) << 8) | ((buffer & 0xFF00) >> 8);
	HAL_SPI_Transmit(this->bus, (uint8_t*)&next_buffer, 2, HAL_MAX_DELAY);
	this->value = buffer;
}

void shift_reg_latch(shift_reg_t *this) {
	delay_init();
	HAL_GPIO_WritePin(this->latch_port, this->latch_pin, GPIO_PIN_SET);
	delay_us(1);
	HAL_GPIO_WritePin(this->latch_port, this->latch_pin, GPIO_PIN_RESET);
}

void shift_reg_oe(shift_reg_t *this, bool onoff) { // true значит выключить, false - включить
	if (onoff) {
		HAL_GPIO_WritePin(this->oe_port, this->oe_pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(this->oe_port, this->oe_pin, GPIO_PIN_RESET);
	}
}

void shift_reg_write_8(shift_reg_t *this, uint8_t buffer) {
	HAL_SPI_Transmit(this->bus, &buffer, 1, HAL_MAX_DELAY);
	this->value = (this->value << 8) | buffer;
	shift_reg_latch(this);
}

void shift_reg_write_16(shift_reg_t *this, uint16_t buffer) {
	uint16_t next_buffer = ((buffer & 0x00FF) << 8) | ((buffer & 0xFF00) >> 8);
	HAL_SPI_Transmit(this->bus, (uint8_t*)&next_buffer, 2, HAL_MAX_DELAY);
	this->value = buffer;
	shift_reg_latch(this);
}

void shift_reg_write_bit_8(shift_reg_t *this, uint8_t pos, bool onoff) {
	uint8_t tmp_buffer = this->value & ~(1 << pos);
	tmp_buffer = tmp_buffer | ((onoff ? 1 : 0) << pos);
	shift_reg_write_8(this, tmp_buffer);
}

void shift_reg_write_bit_16(shift_reg_t *this, uint16_t pos, bool onoff) {
	uint16_t tmp_buffer = this->value & ~(1 << pos);
	tmp_buffer = tmp_buffer | ((onoff ? 1 : 0) << pos);
	shift_reg_write_16(this, tmp_buffer);
}

#endif /* HAL_SPI_MODULE_ENABLED */
