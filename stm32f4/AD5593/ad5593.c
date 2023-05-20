#include "ad5593.h"

#include "ad5593_board.h"

// Сборка значения поинтера из страших и младших битов
#define AD5593_PTR_COMBO(high, low) ((uint8_t)( \
    ((high & 0x0F) << 4) | ((low & 0x0F) << 0) \
))

// Старшие биты комбинаций поинтера
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Запись в регистры
#define AD5593_PTRH_CFG      (0b0000) // CONFIG_MODE_POINTER
#define AD5593_PTRH_DAC_WR   (0b0001) // DAC_WR_POINTER
#define AD5593_PTRH_ADC_RD   (0b0100) // ADC_RD_POINTER
#define AD5593_PTRH_DAC_RD   (0b0101) // DAC_RD_POINTER
#define AD5593_PTRH_GPIO_RD  (0b0110) // GPIO_RD_POINTER
#define AD5593_PTRH_REG_RD   (0b0111) // REG_RD_POINTER

// Адреса регистров
#define AD5593_REG_NOP              (0x00)
#define AD5593_REG_ADC_SEQ          (0x02)
#define AD5593_REG_GEN_CTRL_SEQ     (0x03)
#define AD5593_REG_ADC_CONFIG       (0x04)
#define AD5593_REG_DAC_CONFIG       (0x05)
#define AD5593_REG_PULLDOWN_CONFIG  (0x06)
#define AD5593_REG_LDAC_MODE        (0x07)
#define AD5593_REG_GPIO_CONFIG      (0x08)
#define AD5593_REG_GPIO_OUTPUT      (0x09)
#define AD5593_REG_GPIO_INPUT       (0x0A)
#define AD5593_REG_PD_REF_CTRL      (0x0B)
#define AD5593_REG_GPIO_OPENDRAIN_CONFIG    (0x0C)
#define AD5593_REG_IO_TS_CONFIG     (0x0D)
#define AD5593_REG_SW_RESET         (0x0F)
// В доке этот регистр есть, но на самом деле это комбо от (0b0001, 0b0000)
// И это комбо во втором аргументе может иметь не 0 например
//#define AD5593_REG_DAC_WR           (0x10)

// Код для программного сброса
#define AD5593_RESET_CODE (0x0DAC)


inline static int _write(ad5593_t * device, uint8_t pointer, uint16_t data)
{
    return ad5593_brd_write(device->_brd, device->_addr, pointer, data);
}


inline static int _read(ad5593_t * device, uint8_t pointer, uint16_t * data)
{
    return ad5593_brd_read(device->_brd, device->_addr, pointer, data);
}


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


int ad5593_ctor(ad5593_t * device, ad5593_addr_t addr, void * board_user_arg)
{
    device->_addr = (addr == AD5593_ADDR_A0_LOW) ? 0x10 : 0x11;
    int rc = ad5593_brd_ctor(&device->_brd, board_user_arg);
    return rc;
}


void ad5593_dtor(ad5593_t * device)
{
    ad5593_brd_dtor(device->_brd);
    device->_brd = NULL;
}


int ad5593_sw_reset(ad5593_t * device)
{
    const uint8_t pointer = AD5593_PTR_COMBO(AD5593_PTRH_CFG, AD5593_REG_SW_RESET);
    const uint16_t code = AD5593_RESET_CODE;
    int rc = _write(device, pointer, code);
    return rc;
}


int ad5593_power_config(ad5593_t * device, ad5593_power_t power, uint8_t dac_mask)
{
    uint16_t reg_data = 0;

    switch (power)
    {
        // Включены оба
        case AD5593_POWER_DAC_REF:
            reg_data &= ~(uint16_t)(1 << 10);
            reg_data |=  (uint16_t)(1 <<  9);
            break;

        // Включен только ЦАП
        case AD5593_POWER_DAC:
            reg_data &= ~(uint16_t)(1 << 10);
            reg_data &= ~(uint16_t)(1 <<  9);
            break;

        // Все выключено
        case AD5593_POWER_NONE:
            reg_data |=  (uint16_t)(1 << 10);
            reg_data &= ~(uint16_t)(1 << 9);
            break;

        default:
            return -1;
    }

    // Выключаем все линии ЦАП (они выключаются единицей)
    reg_data |= (uint16_t)0xFF;
    // Включаем то что сказали
    reg_data &= ~(uint16_t)dac_mask;

    // Пишем в устройство
    const uint8_t pointer = AD5593_PTR_COMBO(AD5593_PTRH_CFG, AD5593_REG_PD_REF_CTRL);
    int rc = _write(device, pointer, reg_data);
    return rc;
}


int ad5593_an_config(ad5593_t * device, const ad5593_an_config_t * config)
{
    uint16_t reg_value = 0;
    
    if (config->adc_buffer_enable)
        reg_value |= (1 << 8);
    
    if (config->adc_buffer_precharge)
        reg_value |= (1 << 9);

    switch (config->adc_range)
    {
        case AD5593_RANGE_1REF:
            reg_value &= ~(1 << 5);
            break;

        case AD5593_RANGE_2REF:
            reg_value |= (1 << 5);
            break;

        default:
            return -1;
    }

    // TODO: остальные настройкn
    const uint8_t pointer = AD5593_PTR_COMBO(AD5593_PTRH_CFG, AD5593_REG_GEN_CTRL_SEQ);
    int rc = _write(device, pointer, reg_value);
    return rc;
}


int ad5593_pin_config(ad5593_t * device, uint8_t mask, ad5593_pinmode_t mode)
{
    if (0 == mask)
        return 0;

    // Определяем пары адрес, значение для регистров, которые будем шевелить
    struct ad5593_reg_pair_t
    {
        uint8_t addr;
        uint16_t value;
    };

    struct ad5593_reg_pair_t regs[] = {
        { AD5593_REG_ADC_CONFIG, 0},   // 0 - пины для АЦП
        { AD5593_REG_DAC_CONFIG, 0},   // 1 - пины для ЦАП
        { AD5593_REG_PULLDOWN_CONFIG, 0}, // 2 - управление пулдауном
        { AD5593_REG_GPIO_CONFIG, 0},  // 3 - пины для push pull output
        { AD5593_REG_GPIO_INPUT, 0},   // 4 - пины для input
        { AD5593_REG_PD_REF_CTRL, 0},  // 5 - пины для power down
        { AD5593_REG_GPIO_OPENDRAIN_CONFIG, 0},   // 6 - управление open drain
        { AD5593_REG_IO_TS_CONFIG, 0}, // 7- пины для three state output
    };

    // Загружаем значения всех управляющих пинами регистров
    for (size_t i = 0; i < sizeof(regs)/sizeof(*regs); i++)
    {
        const uint8_t pointer = AD5593_PTR_COMBO(AD5593_PTRH_REG_RD, regs[i].addr);
        int rc = _read(device, pointer, &regs[i].value);
        if (0 != rc)
            return rc;

        // Сразу глушим маской биты, которые мы будем править
        regs[i].value &= ~(uint16_t)mask;
    }

    // В зависимости от режима выставляем биты, которые хотим
    switch (mode)
    {
        case AD5593_PINMODE_ADC:
            regs[0].value |= mask;
            break;

        // TODO: остальные режимы

        default:
            return -1;
    };

    // Записываем все обратно
    for (size_t i = 0; i < sizeof(regs)/sizeof(*regs); i++)
    {
        const uint8_t pointer = AD5593_PTR_COMBO(AD5593_PTRH_CFG, regs[i].addr);
        int rc = _write(device, pointer, regs[i].value);
        if (0 != rc)
            return rc;  // FIXME: подумать как тут быть то, возможно стоит попытаться записать 
                        // остальное
    }

    return 0;
}


int ad5593_adc_auto_setup(ad5593_t * device, int adc_channels_mask, bool repeat)
{
    uint16_t reg_value = 0;

    reg_value |= adc_channels_mask & 0x01FF;
    if (repeat)
        reg_value |= (1 << 9);

    const uint8_t pointer = AD5593_PTR_COMBO(AD5593_PTRH_CFG, AD5593_REG_ADC_SEQ);
    int rc = _write(device, pointer, reg_value);
    return rc;
}


int ad5593_adc_auto_read(ad5593_t * device, ad5593_channel_id_t * channel, uint16_t * value)
{
    const uint8_t pointer = AD5593_PTR_COMBO(AD5593_PTRH_ADC_RD, 0x00);
    uint16_t raw;
    int rc = _read(device, pointer, &raw);
    if (0 != rc)
        return rc;

    const uint8_t channel_bits = (raw >> 12) & 0x0F;
    *channel = channel_bits; // TODO: проверка на адекватность
    *value = raw & 0x0FFF;
    return 0;
}


int ad5593_adc_read(ad5593_t * device, ad5593_channel_id_t channel, uint16_t * value)
{
    const int channel_mask = (1 << channel);
    int rc = ad5593_adc_auto_setup(device, channel_mask, true);
    if (0 != rc)
        return rc;

    uint16_t buffer;
    ad5593_channel_id_t in_channel;
    rc = ad5593_adc_auto_read(device, &in_channel, &buffer);
    if (0 != rc)
        return rc;

    if (in_channel != channel)
        return -2;

    *value = buffer;
    return 0;
}
