/*
 * AD5593.c
 *
 *  Created on: 22 апр. 2023 г.
 *      Author: Install
 */
#include "AD5593/AD5593.h"
#define AD5593_NOP             0b00000000
#define AD5593_ADC_SEQ_REG     0b00000010
#define AD5593_GP_CTRL_REG     0b00000011
#define AD5593_ADC_PIN_CONF    0b00000100
#define AD5593_DAC_PIN_CONF    0b00000101
#define AD5593_PULL_DOWN_CONF  0b00000110
#define AD5593_LDAC_MODE       0b00000111
#define AD5593_GPIO_W_CONF     0b00001000
#define AD5593_GPIO_W_DATA     0b00001001
#define AD5593_GPIO_R_CONF     0b00001010
#define AD5593_POWER_DOWN_CTRL 0b00001011
#define AD5593_OPEN_DRAIN_CONF 0b00001100
#define AD5593_3-STATE_PINS    0b00001101
#define AD5593_SOFT_RESET      0b00001111

#define AD5593_ADC_BUF_PRECHARGE	(1 << 8)
#define AD5593_ADC_BUFFER_ENDABLE	(1 << 7)
#define AD5593_LOCK_CONF			(1 << 6)
#define AD5593_W_ALL_DAC			(1 << 5)
#define AD5593_ADC_RANGE_SELECT		(1 << 4)
#define AD5593_DAC_RANGE_SELECT		(1 << 3)

void AD5593_init(AD5593_t ad)
{

}

/*
void AD5593_read_reg(AD5593_t ad, uint8_t reg_addr, uint16_t *data)
{
	HAL_I2C_Master_Transmit(ad.bus, ad.addr, &reg_addr, 1, AD5593_TIMEOUT);
	HAL_I2C_Master_Receive(ad.bus, ad.addr, data, 2, AD5593_TIMEOUT);
}

void AD5593_write_reg(AD5593_t ad, uint8_t reg_addr, uint16_t data)
{
	uint8_t byte3[3];
	byte3[0] = reg_addr;
	byte3[1] = data >> 8;
	byte3[2] = data & 0xFF;
	HAL_I2C_Master_Receive(ad.bus, ad.addr, sizeof(bit3), 3, AD5593_TIMEOUT);
}
*/
