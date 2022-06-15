#include "../dwt_delay.h"


#include <stm32f1xx.h>


int dwt_delay_init(void)
{
    static uint8_t reinit_defence = 0;
    if (reinit_defence) {
    	return 1; /*clock cycle counter not started*/
    }
	/* Disable TRC */
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
    /* Enable TRC */
    CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

    /* Disable clock cycle counter */
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
    /* Enable  clock cycle counter */
    DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

    /* Reset the clock cycle counter value */
    DWT->CYCCNT = 0;

    /* 3 NO OPERATION instructions */
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");

    /* Check if clock cycle counter has started */
    if(DWT->CYCCNT) {
    	reinit_defence = 1;
    	return 0; /*clock cycle counter started*/
    } else {
    	return 1; /*clock cycle counter not started*/
    }
}


void dwt_delay_us(uint32_t pause_us)
{
	const uint32_t initial_ticks = DWT->CYCCNT;
	const uint32_t ticks_per_us = (HAL_RCC_GetHCLKFreq() / 1000 / 1000);
	const uint32_t ticks_to_wait = ticks_per_us * pause_us;

	while ((DWT->CYCCNT - initial_ticks) < ticks_to_wait)
	{}
}


void dwt_delay_ms(uint32_t pause_ms)
{
	const uint32_t initial_ticks = DWT->CYCCNT;
	const uint32_t ticks_per_ms = (HAL_RCC_GetHCLKFreq() / 1000);
	const uint32_t ticks_to_wait = ticks_per_ms * pause_ms;

	while ((DWT->CYCCNT - initial_ticks) < ticks_to_wait)
	{}
}
