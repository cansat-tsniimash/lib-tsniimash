/*
 * AD5593.h
 *
 *  Created on: 22 апр. 2023 г.
 *      Author: Install
 */

#ifndef AD5593_AD5593_H_
#define AD5593_AD5593_H_

typedef struct AD5593_s
{
//	I2C_HandleTypeDef *bus;
//	uint8_t addr;
} AD5593_t;

enum AD5593_ADDR
{
	AD5593_ADDR_0 = 0x20,
	AD5593_ADDR_1 = 0x22
};

#define AD5593_TIMEOUT 10




#endif /* AD5593_AD5593_H_ */
