/*                         3.3V
 *                         _|_
 *                         |R |           _________
 *                         |__|          |         |
 *                           |___________|    MK   |
 *                           |           |         |
 *                        \ _|_          |_________|
 *                        \ |  |
 *                        \ |__| -фоторезистор
 *                           |
 *                          _|_
 * зарисовка работы фоторезистора
 */

#ifndef PHOTOREZISTOR_H_
#define PHOTOREZISTOR_H_

#include "main.h"

#ifdef HAL_ADC_MODULE_ENABLED

typedef struct {
	float resist; // сопротивление резистора в делителе напряжения (R)
	ADC_HandleTypeDef* hadc;
} photorezistor_t;


//переводим из показателей фоторезистора в люксы
float photorezistor_get_lux(photorezistor_t ph);

#endif // HAL_ADC_MODULE_ENABLED

#endif // PHOTOREZISTOR_H_
