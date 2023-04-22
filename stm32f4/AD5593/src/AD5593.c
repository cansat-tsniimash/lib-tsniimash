/*
 * AD5593.c
 *
 *  Created on: 22 апр. 2023 г.
 *      Author: Install
 */


void AD5593_read_reg(AD5593_t ad, uint8_t reg_addr)
{

	// tx 1 byte reg_addr
	// rx 2 byte reg_data
	HAL_I2C_Master_Transmit(hi2c, DevAddress, pData, Size, Timeout);
	HAL_I2C_Master_Resive(hi2c, DevAddress, pData, Size, Timeout);
}

void AD5593_write_reg(AD5593_t ad);
