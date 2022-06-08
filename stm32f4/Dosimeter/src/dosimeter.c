#include "../dosimeter.h"

#include <stdint.h>
#include <string.h>
#include <stm32f4xx_hal.h>

#define buffer_size 60

static uint32_t dosimeter_buffer[buffer_size] = {0};
static uint32_t ticks_sum = 0;
static uint32_t buffer_head = 0;
static uint32_t last_second = 0;
static uint32_t tick_count = 0;

void Dosimeter_Callback(uint16_t pin, uint16_t dosimeter_pin) {
	if (pin == dosimeter_pin) {
		ticks_sum += 1;
		uint16_t time_delta = HAL_GetTick() - last_second * 1000;
		if (time_delta < 1000) {
			tick_count += 1;
		} else if (time_delta >= 1000) {
			dosimeter_buffer[buffer_head] = tick_count;
			uint16_t time_delta_sec = time_delta / 1000;
			if (buffer_head + time_delta_sec >= buffer_size) {
				if ((buffer_head + time_delta_sec) / buffer_size > 1) {
					memset(&dosimeter_buffer, 0, buffer_size * 4);
					buffer_head += time_delta_sec;
					buffer_head %= buffer_size;
					tick_count = 1;
				} else {
					for (uint32_t i = buffer_head; i < buffer_size; i += 1) dosimeter_buffer[i] = 0;
					buffer_head += time_delta_sec;
					buffer_head %= buffer_size;
					for (uint8_t i = 0; i < buffer_head; i += 1) dosimeter_buffer[i] = 0;
				}
			} else {
				buffer_head += time_delta_sec;
			}
			last_second += time_delta_sec;
		}
	} else {
		return;
	}
}

uint32_t Dosimeter_Get_TPS() {
	if (HAL_GetTick() - last_second * 1000 < 1000) {
		return dosimeter_buffer[buffer_head - 1];
	} else {
		return 0;
	}
}

uint32_t Dosimeter_Get_TPM() {
	uint32_t sum = 0;
	for (uint8_t i = 0; i < buffer_size; i += 1) sum += dosimeter_buffer[i];
	return sum;
}

uint32_t Dosimeter_Get_Sum() {
	return ticks_sum;
}
