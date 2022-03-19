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
#include <math.h>
#include <stdio.h>

#include "main.h"

typedef struct {
	// сопротивление резистора в делителе напряжения (R)
	float resist;
	ADC_HandleTypeDef* hadc;
} photorezistor_t;
/*
 * переводим из показателей фоторезистора в люксы
 */
float photorezistor_get_lux(photorezistor_t);
