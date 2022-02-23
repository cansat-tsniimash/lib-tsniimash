#ifndef NRF24_DEFS_H_
#define NRF24_DEFS_H_

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Регистры
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#define NRF24_REGADDR_CONFIG		0x00
#define NRF24_REGADDR_EN_AA			0x01
#define NRF24_REGADDR_EN_RXADDR		0x02
#define NRF24_REGADDR_SETUP_AW		0x03
#define NRF24_REGADDR_SETUP_RETR	0x04
#define NRF24_REGADDR_RF_CH			0x05
#define NRF24_REGADDR_RF_SETUP		0x06
#define NRF24_REGADDR_STATUS		0x07
#define NRF24_REGADDR_OBSERVE_TX	0x08
#define NRF24_REGADDR_RPD			0x09
#define NRF24_REGADDR_RX_ADDR_P0	0x0A
#define NRF24_REGADDR_RX_ADDR_P1	0x0B
#define NRF24_REGADDR_RX_ADDR_P2	0x0C
#define NRF24_REGADDR_RX_ADDR_P3	0x0D
#define NRF24_REGADDR_RX_ADDR_P4	0x0E
#define NRF24_REGADDR_RX_ADDR_P5	0x0F
#define NRF24_REGADDR_TX_ADDR		0x10
#define NRF24_REGADDR_RX_PW_P0		0x11
#define NRF24_REGADDR_RX_PW_P1		0x12
#define NRF24_REGADDR_RX_PW_P2		0x13
#define NRF24_REGADDR_RX_PW_P3		0x14
#define NRF24_REGADDR_RX_PW_P4		0x15
#define NRF24_REGADDR_RX_PW_P5		0x16
#define NRF24_REGADDR_FIFO_STATUS	0x17
#define NRF24_REGADDR_DYNPD			0x1C
#define NRF24_REGADDR_FEATURE		0x1D

// Есть данные в RX буферах
#define NRF24_STATUS_RX_DR	 (1 << 6)
// Данные из TX буфера отправлены
#define NRF24_STATUS_TX_DS	 (1 << 5)
// Превышено количество попыток отправки данных
#define NRF24_STATUS_MAX_RT	 (1 << 4)
// Флаги прерываний
#define NRF24_STATUS_FLAG_OFFSET  4
#define NRF24_STATUS_FLAG_MASK    0x07
// Нет данных в TX буферах
#define NRF24_STATUS_TX_FULL (1 << 0)
// Номер пайпа, пакет с которого ожидает выгребания в RX FIFO
#define NRF24_STATUS_RX_P_NO_OFFSET 1
#define NRF24_STATUS_RX_P_NO_MASK   0x07 // 111
// Наличие свободного места в TX буферах
#define NRF24_STATUS_TX_FULL_OFFSET  0
#define NRF24_STATUS_TX_FULL_MASK    0x01

// Прерывание по получению данных
#define NRF24_CONFIG_MASK_RX_DR	 (1 << 6)
// Прерывание по успешной отправке данных
#define NRF24_CONFIG_MASK_TX_DR	 (1 << 5)
// Прерывание по проавлу отправки данных
#define NRF24_CONFIG_MASK_MAX_RT (1 << 4)
// Включение CRC (всегда включено, если включены авто-ACK)
#define NRF24_CONFIG_EN_CRC		 (1 << 3)
// Длина CRC (0 - 1 байт, 1 - 2 байта)
#define NRF24_CONFIG_CRCO		 (1 << 2)
// Включение модуля (если 0, то модуль спит)
#define NRF24_CONFIG_PWR_UP		 (1 << 1)
// Упрвавление режимом - приём или передача
#define NRF24_CONFIG_PRIM_RX	 (1 << 0)

#define NRF24_RFSETUP_CONT_WAVE	 (1 << 7)
#define NRF24_RFSETUP_RF_DR_LOW	 (1 << 5)
#define NRF24_RFSETUP_PLL_LOCK	 (1 << 4)
#define NRF24_RFSETUP_RF_DR_HIGH (1 << 3)
#define NRF24_RFSETUP_RF_PWR_OFFSET  1
#define NRF24_RFSETUP_RF_PWR_MASK    0x03

#define NRF24_FIFO_STATUS_TX_FULL_OFFSET  5
#define NRF24_FIFO_STATUS_TX_FULL_MASK    0x01
#define NRF24_FIFO_STATUS_TX_EMPTY_OFFSET 4
#define NRF24_FIFO_STATUS_TX_EMPTY_MASK   0x01
#define NRF24_FIFO_STATUS_RX_FULL_OFFSET  1
#define NRF24_FIFO_STATUS_RX_FULL_MASK    0x01
#define NRF24_FIFO_STATUS_RX_EMPTY_OFFSET 0
#define NRF24_FIFO_STATUS_RX_EMPTY_MASK   0x01

#define NRF24_FEATURE_EN_DPL		(1 << 2)
#define NRF24_FEATURE_EN_ACK_PAY	(1 << 1)
#define NRF24_FEATURE_EN_DYN_ACK	(1 << 0)


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Команды
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#define NRF24_R_REGISTER          0b000 // В конец пишется адрес регистра (5 бит)
#define NRF24_W_REGISTER          0b001 // В конец пишется адрес регистра (5 бит)
#define NRF24_R_RX_PAYLOAD        0b01100001
#define NRF24_W_TX_PAYLOAD        0b10100000
#define NRF24_FLUSH_TX	          0b11100001
#define NRF24_FLUSH_RX	          0b11100010
#define NRF24_REUSE_TX_PL	      0b11100011
#define NRF24_R_RX_PL_WID         0b01100000
#define NRF24_W_ACK_PAYLOAD       0b10101 // В конец пишется номер пайпа (3 бита)
#define NRF24_W_TX_PAYLOAD_NO_ACK 0b10110000
#define NRF24_NOP                 0b11111111

#endif /* NRF24_DEFS_H_ */
