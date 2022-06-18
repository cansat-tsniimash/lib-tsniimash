#include <math.h>
#include "../photorezistor.h"

#ifdef HAL_ADC_MODULE_ENABLED


float photorezistor_get_lux(photorezistor_t ph) {
	HAL_ADC_Start(ph.hadc);
	HAL_ADC_PollForConversion(ph.hadc, 100);
	float volts = HAL_ADC_GetValue(ph.hadc) * 3.3 / 4095;	//Volts
	float ohms = volts*(ph.resist)/(3.3-volts);		//Ohms
	float lux = exp((3.823-log(ohms/1000))/0.816)*10.764;	//Lux
	HAL_ADC_Stop(ph.hadc);
	return lux;
}


#endif
