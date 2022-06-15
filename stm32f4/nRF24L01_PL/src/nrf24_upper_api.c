#include <stm32f4xx_hal.h>
#ifdef HAL_SPI_MODULE_ENABLED

#include "../nrf24_upper_api.h"
#include "../nrf24_lower_api.h"
#include "../nrf24_defs.h"


int nrf24_setup_rf(void * intf_ptr, const nrf24_rf_config_t * config)
{
	uint8_t rf_setup = 0;
	nrf24_read_register(intf_ptr, NRF24_REGADDR_RF_SETUP, &rf_setup, 1);
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
	case NRF24_DATARATE_2000_KBIT :
		rf_setup &= ~NRF24_RFSETUP_RF_DR_LOW;
		rf_setup |= NRF24_RFSETUP_RF_DR_HIGH;
		break;
	}

	rf_setup &= ~(NRF24_RFSETUP_RF_PWR_MASK << NRF24_RFSETUP_RF_PWR_OFFSET); // занулили
	rf_setup |= (config->tx_power & NRF24_RFSETUP_RF_PWR_MASK) << NRF24_RFSETUP_RF_PWR_OFFSET; // записали

	nrf24_write_register(intf_ptr, NRF24_REGADDR_RF_SETUP, &rf_setup, 1);

	uint8_t rf_channel = config->rf_channel;
    nrf24_write_register(intf_ptr, NRF24_REGADDR_RF_CH, &rf_channel, 1);
	return 0;
}


int nrf24_setup_protocol(void * intf_ptr, const nrf24_protocol_config_t * config)
{
    uint8_t conf_reg = 0;
    nrf24_read_register(intf_ptr, NRF24_REGADDR_CONFIG, &conf_reg, 1);
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
    nrf24_write_register(intf_ptr, NRF24_REGADDR_CONFIG, &conf_reg, 1);

    uint8_t setup_aw = config->address_width;
    nrf24_write_register(intf_ptr, NRF24_REGADDR_SETUP_AW, &setup_aw, 1);

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
    nrf24_write_register(intf_ptr, NRF24_REGADDR_FEATURE, &features, 1);

    uint8_t setup_retr = (config->auto_retransmit_delay << 4) | (config->auto_retransmit_count & 0xf);
    nrf24_write_register(intf_ptr, NRF24_REGADDR_SETUP_RETR, &setup_retr, 1);
	return 0;
}


int nrf24_pipe_rx_start(void * intf_ptr, uint8_t pipe_no, const nrf24_pipe_config_t * config)
{
	uint8_t en_aa = 0 ;
	nrf24_read_register(intf_ptr, NRF24_REGADDR_EN_AA, &en_aa, 1);
	if (config->enable_auto_ack)
	{
		en_aa |= (1 << pipe_no);
	}
	else
	{
		en_aa &= ~(1 << pipe_no);
	}
	nrf24_write_register(intf_ptr, NRF24_REGADDR_EN_AA, &en_aa, 1);

	uint8_t en_rxaddr = 0;
	nrf24_read_register(intf_ptr, NRF24_REGADDR_EN_RXADDR, &en_rxaddr, 1);
	en_rxaddr |= (1 << pipe_no);
	nrf24_write_register(intf_ptr, NRF24_REGADDR_EN_RXADDR, &en_rxaddr, 1);

	uint8_t rx_pw = 0;
	switch (pipe_no)
	{
	case 0:
		nrf24_write_register(intf_ptr, NRF24_REGADDR_RX_ADDR_P0, (uint8_t *)(&config->address), 5);
		rx_pw = NRF24_REGADDR_RX_PW_P0;
		break;
	case 1:
		nrf24_write_register(intf_ptr, NRF24_REGADDR_RX_ADDR_P1, (uint8_t *)(&config->address), 5);
		rx_pw = NRF24_REGADDR_RX_PW_P1;
		break;
	case 2:
		nrf24_write_register(intf_ptr, NRF24_REGADDR_RX_ADDR_P2, ((uint8_t *)(&config->address) + 4), 1);
		rx_pw = NRF24_REGADDR_RX_PW_P2;
		break;
	case 3:
		nrf24_write_register(intf_ptr, NRF24_REGADDR_RX_ADDR_P3, ((uint8_t *)(&config->address) + 4), 1);
		rx_pw = NRF24_REGADDR_RX_PW_P3;
		break;
	case 4:
		nrf24_write_register(intf_ptr, NRF24_REGADDR_RX_ADDR_P4, ((uint8_t *)(&config->address) + 4), 1);
		rx_pw = NRF24_REGADDR_RX_PW_P4;
		break;
	case 5:
		nrf24_write_register(intf_ptr, NRF24_REGADDR_RX_ADDR_P5, ((uint8_t *)(&config->address) + 4), 1);
		rx_pw = NRF24_REGADDR_RX_PW_P5;
		break;
	}

	uint8_t dynpd = 0;
	nrf24_read_register(intf_ptr, NRF24_REGADDR_DYNPD, &dynpd, 1);
	if (config->payload_size >= 0)
	{
		nrf24_write_register(intf_ptr, rx_pw, (uint8_t *)(&config->payload_size), 1);
		dynpd &= ~(1 << pipe_no);
	}
	else
	{
		uint8_t pld_size = 32; // Нужно записать не 0, чтобы пайп в целом работал
		nrf24_write_register(intf_ptr, rx_pw, &pld_size, 1);
		dynpd |= (1 << pipe_no);
	}
	nrf24_write_register(intf_ptr, NRF24_REGADDR_DYNPD, &dynpd, 1);

	return 0;
}


int nrf24_pipe_rx_stop(void * intf_ptr, uint8_t pipe_no)
{
	uint8_t en_rxaddr = 0;
	nrf24_read_register(intf_ptr, NRF24_REGADDR_EN_RXADDR, &en_rxaddr, 1);
	en_rxaddr &= ~(1 << pipe_no);
	nrf24_write_register(intf_ptr, NRF24_REGADDR_EN_RXADDR, &en_rxaddr, 1);
	return 0;
}


int nrf24_pipe_set_tx_addr(void * intf_ptr, uint64_t tx_addr)
{
	nrf24_write_register(intf_ptr, NRF24_REGADDR_TX_ADDR, (uint8_t *)(&tx_addr), 5);
	return 0;
}


int nrf24_mode_power_down(void * intf_ptr)
{
	uint8_t config_reg = 0;
	nrf24_read_register(intf_ptr, NRF24_REGADDR_CONFIG, &config_reg, 1);
	config_reg &= ~NRF24_CONFIG_PWR_UP;
	nrf24_write_register(intf_ptr, NRF24_REGADDR_CONFIG, &config_reg, 1);
	return 0;
}


int nrf24_mode_standby(void * intf_ptr)
{
	uint8_t config_reg = 0;
	nrf24_read_register(intf_ptr, NRF24_REGADDR_CONFIG, &config_reg, 1);
	config_reg |= NRF24_CONFIG_PWR_UP;
	nrf24_write_register(intf_ptr, NRF24_REGADDR_CONFIG, &config_reg, 1);
	nrf24_ce_activate(intf_ptr, false);
	return 0;
}


int nrf24_mode_tx(void * intf_ptr)
{
	uint8_t mode_tx = 0;
	nrf24_read_register(intf_ptr, NRF24_REGADDR_CONFIG, &mode_tx, 1);
	mode_tx &= ~NRF24_CONFIG_PRIM_RX;
	nrf24_write_register(intf_ptr, NRF24_REGADDR_CONFIG, &mode_tx, 1);
	nrf24_ce_activate(intf_ptr, true);
	return 0;
}


int nrf24_mode_rx(void * intf_ptr)
{
	uint8_t mode_tx = 0;
	nrf24_read_register(intf_ptr, NRF24_REGADDR_CONFIG, &mode_tx, 1);
	mode_tx |= NRF24_CONFIG_PRIM_RX;
	nrf24_write_register(intf_ptr, NRF24_REGADDR_CONFIG, &mode_tx, 1);
	nrf24_ce_activate(intf_ptr, true);
	return 0;
}


int nrf24_fifo_peek(void * intf_ptr, uint8_t * rx_packet_size, uint8_t * rx_pipe_no, bool * tx_full)
{
	uint8_t status_reg = 0;
	nrf24_get_status(intf_ptr, &status_reg);
	*tx_full = (status_reg >> NRF24_STATUS_TX_FULL_OFFSET) & NRF24_STATUS_TX_FULL_MASK;
	*rx_pipe_no = (status_reg >> NRF24_STATUS_RX_P_NO_OFFSET) & NRF24_STATUS_RX_P_NO_MASK;
	if (*rx_pipe_no <= 0x05)
	{
		nrf24_get_rx_payload_size(intf_ptr, rx_packet_size);
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


int nrf24_fifo_status(void * intf_ptr, nrf24_fifo_status_t * rx_status, nrf24_fifo_status_t * tx_status)
{
	uint8_t fifo_satus = 0;
	nrf24_read_register(intf_ptr, NRF24_REGADDR_FIFO_STATUS, &fifo_satus, 1);

	*rx_status = NRF24_FIFO_NOT_EMPTY;
	if (((fifo_satus >> NRF24_FIFO_STATUS_RX_EMPTY_OFFSET) & NRF24_FIFO_STATUS_RX_EMPTY_MASK) == 1)
	{
		*rx_status = NRF24_FIFO_EMPTY;
	}
	if (((fifo_satus >> NRF24_FIFO_STATUS_RX_FULL_OFFSET) & NRF24_FIFO_STATUS_RX_FULL_MASK) == 1)
	{
		*rx_status = NRF24_FIFO_FULL;
	}

	*tx_status = NRF24_FIFO_NOT_EMPTY;
	if (((fifo_satus >> NRF24_FIFO_STATUS_TX_EMPTY_OFFSET) & NRF24_FIFO_STATUS_TX_EMPTY_MASK) == 1)
	{
		*tx_status = NRF24_FIFO_EMPTY;
	}
	if (((fifo_satus >> NRF24_FIFO_STATUS_TX_FULL_OFFSET) & NRF24_FIFO_STATUS_TX_FULL_MASK) == 1)
	{
		*tx_status = NRF24_FIFO_FULL;
	}
	return 0;
}


int nrf24_fifo_read(void * intf_ptr, uint8_t * packet_buffer, uint8_t packet_buffer_size)
{
	uint8_t payload_size = 0;
	nrf24_get_rx_payload_size(intf_ptr, &payload_size);
	if (payload_size > 0)
	{
		nrf24_read_rx_payload(intf_ptr, packet_buffer, packet_buffer_size);
	}
	return payload_size;
}


int nrf24_fifo_write(void * intf_ptr, const uint8_t * packet, uint8_t packet_size, bool use_ack)
{
	if (packet_size == 0)
	{
		return 0;
	}
	if (packet_size <= 32)
	{
		nrf24_write_tx_payload(intf_ptr, packet,  packet_size,  use_ack);
		return packet_size;
	}
	else
	{
		nrf24_write_tx_payload(intf_ptr, packet,  32,  use_ack);
		return 32;
	}
}


int nrf24_fifo_write_ack_pld(void * intf_ptr, uint8_t pipe_no, const uint8_t * packet, uint8_t packet_size)
{
	if (packet_size == 0)
	{
		return 0;
	}
	if (packet_size <= 32)
	{
		nrf24_write_ack_payload(intf_ptr, packet, packet_size, pipe_no);
		return packet_size;
	}
	else
	{
		nrf24_write_ack_payload(intf_ptr, packet, 32, pipe_no);
		return 32;
	}
}


int nrf24_fifo_flush_tx(void * intf_ptr)
{
	nrf24_flush_tx(intf_ptr);
	return 0;
}


int nrf24_fifo_flush_rx(void * intf_ptr)
{
	nrf24_flush_rx(intf_ptr);
	return 0;
}


int nrf24_irq_mask_set(void * intf_ptr, int composition)
{
	// Читаем конфигурационный регистр
	uint8_t config_reg;
	nrf24_read_register(intf_ptr, NRF24_REGADDR_CONFIG, &config_reg, sizeof(config_reg));

	// Ставим во все биты маскирования единицы (это выключает прерывания)
	config_reg |= NRF24_CONFIG_MASK_MAX_RT | NRF24_CONFIG_MASK_TX_DR | NRF24_CONFIG_MASK_RX_DR;

	// Тушим соответствующие единицы
	if (NRF24_IRQ_MAX_RT & composition)
		config_reg &= ~NRF24_CONFIG_MASK_MAX_RT;

	if (NRF24_IRQ_TX_DR & composition)
		config_reg &= ~NRF24_CONFIG_MASK_TX_DR;

	if (NRF24_IRQ_RX_DR & composition)
		config_reg &= ~NRF24_CONFIG_MASK_RX_DR;

	// Пишем конфигурационный регистр обратно
	nrf24_write_register(intf_ptr, NRF24_REGADDR_CONFIG, &config_reg, sizeof(config_reg));

	return 0;
}


int nrf24_irq_mask_get(void * intf_ptr, int * composition)
{
	uint8_t config_reg;
	nrf24_read_register(intf_ptr, NRF24_REGADDR_CONFIG, &config_reg, sizeof(config_reg));

	// Пока что считаем что включены все
	*composition = NRF24_IRQ_MAX_RT | NRF24_IRQ_TX_DR | NRF24_IRQ_RX_DR;

	// Смотрим какие выключены (единица это имено выключенно а не включено)
	if (config_reg & NRF24_CONFIG_MASK_MAX_RT)
		*composition &= ~NRF24_IRQ_MAX_RT;

	if (config_reg & NRF24_CONFIG_MASK_TX_DR)
		*composition &= ~NRF24_IRQ_TX_DR;

	if (config_reg & NRF24_CONFIG_MASK_RX_DR)
		*composition &= ~NRF24_IRQ_RX_DR;

	return 0;
}


int nrf24_irq_get(void * intf_ptr, int * composition)
{
	uint8_t status_reg = 0;
	nrf24_get_status(intf_ptr, &status_reg);
	*composition = (status_reg >> NRF24_STATUS_FLAG_OFFSET) & NRF24_STATUS_FLAG_MASK;
	return 0;
}


int nrf24_irq_clear(void * intf_ptr, int composition)
{
	uint8_t status_reg = (composition & NRF24_STATUS_FLAG_MASK) << NRF24_STATUS_FLAG_OFFSET;
	nrf24_write_register(intf_ptr, NRF24_REGADDR_STATUS, &status_reg, 1);
	return 0;
}

#endif /* HAL_SPI_MODULE_ENABLED */
