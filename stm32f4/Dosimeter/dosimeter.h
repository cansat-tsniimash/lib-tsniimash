#ifndef DOSIMETER_DOSIMETER_H_
#define DOSIMETER_DOSIMETER_H_

#include <stdint.h>

void Dosimeter_Callback(uint16_t pin, uint16_t dosimeter_pin);
double Dosimeter_Get_TPM();
uint32_t Dosimeter_Get_TPS(uint8_t num);

#endif /* DOSIMETER_DOSIMETER_H_ */
