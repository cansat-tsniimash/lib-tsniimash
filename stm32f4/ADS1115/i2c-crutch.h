/*
 * i2c-crutch.h
 *
 *  Created on: 12 нояб. 2022 г.
 *      Author: Agnus
 */

#ifndef INC_I2C_CRUTCH_H_
#define INC_I2C_CRUTCH_H_

#include <stm32f4xx_hal.h>

#ifdef HAL_I2C_MODULE_ENABLED

void I2C_ClearBusyFlagErratum(I2C_HandleTypeDef *hi2c, uint32_t timeout);
void reset_i2c_1();

#endif // HAL_I2C_MODULE_ENABLED

#endif /* INC_I2C_CRUTCH_H_ */
