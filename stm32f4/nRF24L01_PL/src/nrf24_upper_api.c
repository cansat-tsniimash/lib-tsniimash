#include <stm32f4xx_hal.h>
#ifdef HAL_SPI_MODULE_ENABLED

#include <nRF24L01_PL/nrf24_upper_api.h>


int nrf24_setup_rf(const nrf24_rf_config_t * config)
{

	return 0;
}


int nrf24_setup_protocol(const nrf24_protocol_config_t * config)
{

	return -1;
}


int nrf24_pipe_rx_start(uint8_t pipe_no, const nrf24_pipe_config_t * config)
{

	return -1;
}


int nrf24_pipe_rx_stop(uint8_t pipe_no)
{

	return -1;
}


int nrf24_pipe_set_tx_addr(uint64_t tx_addr)
{

	return -1;
}


int nrf24_mode_power_down(void)
{

	return -1;
}


int nrf24_mode_standby(void)
{

	return -1;
}


int nrf24_mode_tx(void)
{

	return -1;
}


int nrf24_mode_rx(void)
{

	return -1;
}


int nrf24_fifo_peek(uint8_t * rx_packet_size, uint8_t * rx_pipe_no, bool * tx_full)
{
	return -1;
}


int nrf24_fifo_status(nrf24_fifo_status_t * rx_status, nrf24_fifo_status_t * tx_status)
{
	return -1;
}


int nrf24_fifo_read(uint8_t * packet_buffer, uint8_t packet_buffer_size)
{
	return -1;
}


int nrf24_fifo_write(const uint8_t * packet, uint8_t packet_size, bool use_ack)
{
	return -1;
}


int nrf24_fifo_write_ack_pld(uint8_t pipe_no, const uint8_t * packet, uint8_t packet_size)
{
	return -1;
}


int nrf24_fifo_flush_tx(void)
{
	return -1;
}


int nrf24_fifo_flush_rx(void)
{

	return -1;
}


int nrf24_irq_get(int * composition)
{

	return -1;
}


int nrf24_irq_clear(int composition)
{

	return -1;
}

#endif /* HAL_SPI_MODULE_ENABLED */
