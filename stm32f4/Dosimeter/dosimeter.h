#ifndef DOSIMETER_DOSIMETER_H_
#define DOSIMETER_DOSIMETER_H_

#include <stdint.h>
#include <stdbool.h>

bool dosimeter_1sec_past(void);

//Custom Dosimeter Callback, use this in HAL_GPIO_EXTI_Callback
void Dosimeter_Callback(uint16_t pin, uint16_t dosimeter_pin);

//Ticks Per (last) Second
uint32_t Dosimeter_Get_TPS(void);

//Ticks Per (last) Minute
uint32_t Dosimeter_Get_TPM(void);

//All ticks, that 've been counted by dosimeter (ignoring cyclobuffer)
uint32_t Dosimeter_Get_Sum(void);



#endif /* DOSIMETER_DOSIMETER_H_ */
