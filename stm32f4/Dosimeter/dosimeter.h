#ifndef DOSIMETER_DOSIMETER_H_
#define DOSIMETER_DOSIMETER_H_

#include <stdint.h>

//Custom Dosimeter Callback, use this in HAL_GPIO_EXTI_Callback
void Dosimeter_Callback(uint16_t pin, uint16_t dosimeter_pin);

//Ticks Per (last) Minute
double Dosimeter_Get_TPM();

//Ticks Per (last) Second
uint32_t Dosimeter_Get_TPS();

//All ticks, that 've been counted by dosimeter (ignoring cyclobuffer)
uint32_t Dosimeter_Get_Sum();

#endif /* DOSIMETER_DOSIMETER_H_ */
