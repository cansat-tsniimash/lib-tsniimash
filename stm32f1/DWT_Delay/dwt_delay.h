#ifndef INC_DWT_DELAY_H_
#define INC_DWT_DELAY_H_


#include <stdint.h>


int dwt_delay_init(void);

void dwt_delay_us(uint32_t pause_us);

void dwt_delay_ms(uint32_t pause_ms);


#endif /* INC_DWT_DELAY_H_ */
