#include "ad5593_board.h"

#include <stm32f4xx_hal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


//! Дескриптор шины для линуксового i2cdev
struct ad5593_board_t
{
    //! Дескриптор шины
    I2C_HandleTypeDef * bus;
};


static ad5593_board_t ad5593;


int ad5593_brd_ctor(ad5593_board_t ** brd, void * user_arg)
{
    ad5593.bus = (I2C_HandleTypeDef*)user_arg;
    *brd = &ad5593;
    return 0;
}


void ad5593_brd_dtor(ad5593_board_t * brd)
{
    return;
}


int ad5593_brd_write(ad5593_board_t * brd, uint8_t addr, uint8_t pointer, uint16_t data)
{
    // Устройство работает в биг эндиане
    uint8_t sequence[] = {
        pointer,
        (data >> 8) & 0xFF, 
        (data >> 0) & 0xFF
    };

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(brd->bus, addr << 1, sequence, sizeof(sequence), HAL_MAX_DELAY);
    if (status != HAL_OK)
    	return 100;

    return 0;
}


int ad5593_brd_read(ad5593_board_t * brd, uint8_t addr, uint8_t pointer, uint16_t * data)
{
    uint16_t raw_data;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(brd->bus, addr << 1, pointer, 1, &raw_data, 2, HAL_MAX_DELAY);
    if (status != HAL_OK)
    	return -1;

    *data = ((raw_data & 0x00FF) << 8) | ((raw_data & 0xFF00) >> 8);
    return 0;
}
