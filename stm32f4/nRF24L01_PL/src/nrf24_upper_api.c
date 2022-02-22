#include <stm32f4xx_hal.h>
#ifdef HAL_SPI_MODULE_ENABLED

#include "../nrf24_upper_api.h"
#include "../nrf24_lower_api.h"
#include "../nrf24_defs.h"


int nrf24_setup_rf(const nrf24_rf_config_t * config)
{
	uint8_t rf_setup = 0;
	nrf24_read_register(NRF24_REGADDR_RF_SETUP, &rf_setup, 1);
	switch (config->data_rate)
	{
	case NRF24_DATARATE_250_KBIT:
		rf_setup |= NRF24_RFSETUP_RF_DR_LOW; // ставим 1
		rf_setup &= ~NRF24_RFSETUP_RF_DR_HIGH; // ставим 0
		break;
	case NRF24_DATARATE_1000_KBIT :
		rf_setup &= ~NRF24_RFSETUP_RF_DR_LOW;
		rf_setup &= ~NRF24_RFSETUP_RF_DR_HIGH;
		break;
	case NRF24_DATARATE_2000_MBIT :
		rf_setup &= ~NRF24_RFSETUP_RF_DR_LOW;
		rf_setup |= NRF24_RFSETUP_RF_DR_HIGH;
		break;
	}

	rf_setup &= ~(NRF24_RFSETUP_RF_PWR_MASK << NRF24_RFSETUP_RF_PWR_OFFSET); // занулили
	rf_setup |= (config->tx_power & NRF24_RFSETUP_RF_PWR_MASK) << NRF24_RFSETUP_RF_PWR_OFFSET; // записали

	nrf24_write_register(NRF24_REGADDR_RF_SETUP, &rf_setup, 1);

	uint8_t rf_channel = config->rf_channel;
    nrf24_write_register(NRF24_REGADDR_RF_CH, &rf_channel, 1);
	return 0;
}


int nrf24_setup_protocol(const nrf24_protocol_config_t * config)
{
    uint8_t conf_reg = 0;
    nrf24_read_register(NRF24_REGADDR_CONFIG, &conf_reg, 1);
    switch (config->crc_size)
    {
    case NRF24_CRCSIZE_DISABLE:
    	conf_reg &= ~NRF24_CONFIG_EN_CRC;
    	break;
    case NRF24_CRCSIZE_1BYTE :
    	conf_reg |= NRF24_CONFIG_EN_CRC;
    	conf_reg &= ~NRF24_CONFIG_CRCO;
    	break;
    case NRF24_CRCSIZE_2BYTE :
    	conf_reg |= NRF24_CONFIG_EN_CRC;
    	conf_reg |= NRF24_CONFIG_CRCO;
    	break;
    }
    nrf24_write_register(NRF24_REGADDR_CONFIG, &conf_reg, 1);

    uint8_t setup_aw = config->address_width;
    nrf24_write_register(NRF24_REGADDR_SETUP_AW, &setup_aw, 1);

    uint8_t features = 0;
    if (config->en_ack_payload)
    {
    	features |= NRF24_FEATURE_EN_ACK_PAY;
    }
    if (config->en_dyn_ack)
    {
    	features |= NRF24_FEATURE_EN_DYN_ACK;
    }
    if (config->en_dyn_payload_size)
    {
    	features |= NRF24_FEATURE_EN_DPL;
    }
    nrf24_write_register(NRF24_REGADDR_FEATURE, &features, 1);

    uint8_t setup_retr = (config->auto_retransmit_delay << 4) | (config->auto_retransmit_count & 0x15);
    nrf24_write_register(NRF24_REGADDR_SETUP_RETR, &setup_retr, 1);
	return 0;
}


int nrf24_pipe_rx_start(uint8_t pipe_no, const nrf24_pipe_config_t * config)
{
	uint8_t en_aa = 0 ;
	nrf24_read_register(NRF24_REGADDR_EN_AA, &en_aa, 1);
	if (config->enable_auto_ack)
	{
		en_aa |= (1 << pipe_no);
	}
	else
	{
		en_aa &= ~(1 << pipe_no);
	}
	nrf24_write_register(NRF24_REGADDR_EN_AA, &en_aa, 1);

	uint8_t en_rxaddr = 0;
	nrf24_read_register(NRF24_REGADDR_EN_RXADDR, &en_rxaddr, 1);
	en_rxaddr |= (1 << pipe_no);
	nrf24_write_register(NRF24_REGADDR_EN_RXADDR, &en_rxaddr, 1);

	switch (pipe_no)
	{
	case 0:
		nrf24_write_register(NRF24_REGADDR_RX_ADDR_P0, (uint8_t *)(&config->address), 5);
		nrf24_write_register(NRF24_REGADDR_RX_PW_P0, config->payload_size, 1);
		break;
	case 1:
		nrf24_write_register(NRF24_REGADDR_RX_ADDR_P1, (uint8_t *)(&config->address), 5);
		nrf24_write_register(NRF24_REGADDR_RX_PW_P1, config->payload_size, 1);
		break;
	case 2:
		nrf24_write_register(NRF24_REGADDR_RX_ADDR_P2, ((uint8_t *)(&config->address) + 4), 1);
		nrf24_write_register(NRF24_REGADDR_RX_PW_P2, config->payload_size, 1);
		break;
	case 3:
		nrf24_write_register(NRF24_REGADDR_RX_ADDR_P3, ((uint8_t *)(&config->address) + 4), 1);
		nrf24_write_register(NRF24_REGADDR_RX_PW_P3, config->payload_size, 1);
		break;
	case 4:
		nrf24_write_register(NRF24_REGADDR_RX_ADDR_P4, ((uint8_t *)(&config->address) + 4), 1);
		nrf24_write_register(NRF24_REGADDR_RX_PW_P4, config->payload_size, 1);
		break;
	case 5:
		nrf24_write_register(NRF24_REGADDR_RX_ADDR_P5, ((uint8_t *)(&config->address) + 4), 1);
		nrf24_write_register(NRF24_REGADDR_RX_PW_P5, config->payload_size, 1);
		break;
	}
	return 0;
}


int nrf24_pipe_rx_stop(uint8_t pipe_no)
{
	uint8_t en_rxaddr = 0;
	nrf24_read_register(NRF24_REGADDR_EN_RXADDR, &en_rxaddr, 1);
	en_rxaddr &= ~(1 << pipe_no);
	nrf24_write_register(NRF24_REGADDR_EN_RXADDR, &en_rxaddr, 1);
	return 0;
}


int nrf24_pipe_set_tx_addr(uint64_t tx_addr)
{
	nrf24_write_register(NRF24_REGADDR_TX_ADDR, (uint8_t *)(&tx_addr), 5);
	return 0;
}


int nrf24_mode_power_down(void)
{
	uint8_t config_reg = 0;
	nrf24_read_register(NRF24_REGADDR_CONFIG, &config_reg, 1);
	config_reg &= ~NRF24_CONFIG_PWR_UP;
	nrf24_write_register(NRF24_REGADDR_CONFIG, &config_reg, 1);
	return 0;
}


int nrf24_mode_standby(void)
{
	uint8_t config_reg = 0;
	nrf24_read_register(NRF24_REGADDR_CONFIG, &config_reg, 1);
	config_reg |= NRF24_CONFIG_PWR_UP;
	nrf24_write_register(NRF24_REGADDR_CONFIG, &config_reg, 1);
	nrf24_ce_activate(false);
	return 0;
}


int nrf24_mode_tx(void)
{
	uint8_t mode_tx = 0;
	nrf24_read_register(NRF24_REGADDR_CONFIG, &mode_tx, 1);
	mode_tx &= ~NRF24_CONFIG_PRIM_RX;
	nrf24_write_register(NRF24_REGADDR_CONFIG, &mode_tx, 1);
	nrf24_ce_activate(true);
	return 0;
}


int nrf24_mode_rx(void)
{
	uint8_t mode_tx = 0;
	nrf24_read_register(NRF24_REGADDR_CONFIG, &mode_tx, 1);
	mode_tx |= NRF24_CONFIG_PRIM_RX;
	nrf24_write_register(NRF24_REGADDR_CONFIG, &mode_tx, 1);
	nrf24_ce_activate(true);
	return 0;
}


int nrf24_fifo_peek(uint8_t * rx_packet_size, uint8_t * rx_pipe_no, bool * tx_full)
{
	uint8_t status_reg = 0;
	nrf24_get_status(&status_reg);
	*tx_full = (status_reg >> NRF24_STATUS_TX_FULL_OFFSET) & NRF24_STATUS_TX_FULL_MASK;
	*rx_pipe_no = (status_reg >> NRF24_STATUS_RX_P_NO_OFFSET) & NRF24_STATUS_RX_P_NO_MASK;
	if (*rx_pipe_no <= 0x05)
	{
		nrf24_get_rx_payload_size(rx_packet_size);
	}
	else
	{
		if (*rx_pipe_no != 0x07)
		{
			return -1;
		}
		*rx_packet_size = 0 ;
	}
	return 0;
}


int nrf24_fifo_status(nrf24_fifo_status_t * rx_status, nrf24_fifo_status_t * tx_status)
{
	uint8_t fifo_satus = 0;
	nrf24_read_register(NRF24_REGADDR_FIFO_STATUS, &fifo_satus, 1);
	(fifo_satus >> NRF24_FIFO_STATUS_RX_EMPTY_OFFSET) & NRF24_FIFO_STATUS_RX_EMPTY_MASK

	*rx_status = NRF24_FIFO_NOT_EMPTY;
	if ((fifo_satus >> NRF24_FIFO_STATUS_RX_EMPTY_OFFSET) & NRF24_FIFO_STATUS_RX_EMPTY_MASK == 1)
	{
		*rx_status = NRF24_FIFO_EMPTY;
	}
	if ((fifo_satus >> NRF24_FIFO_STATUS_RX_FULL_OFFSET) & NRF24_FIFO_STATUS_RX_FULL_MASK == 1)
	{
		*rx_status = NRF24_FIFO_FULL;
	}

	*tx_status = NRF24_FIFO_NOT_EMPTY;
	if ((fifo_satus >> NRF24_FIFO_STATUS_TX_EMPTY_OFFSET) & NRF24_FIFO_STATUS_TX_EMPTY_MASK == 1)
	{
		*tx_status = NRF24_FIFO_EMPTY;
	}
	if ((fifo_satus >> NRF24_FIFO_STATUS_TX_FULL_OFFSET) & NRF24_FIFO_STATUS_TX_FULL_MASK == 1)
	{
		*tx_status = NRF24_FIFO_FULL;
	}
	return 0;
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
