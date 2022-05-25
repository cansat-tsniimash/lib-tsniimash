#include "../one_wire.h"
#ifdef HAL_GPIO_MODULE_ENABLED

#include <stm32f4xx_hal.h>
#include "DWT_Delay/dwt_delay.h"

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Команды этапа адресации
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Команда для пропуска этапа адресации
#define ONEWIRE_CMD_SKIP_ROM	0xCC
// Команда для запроса адреса у единственного подключенного ведомого
#define ONEWIRE_CMD_READ_ROM	0x33
// Команда для выбора ведомого по адресу
#define ONEWIRE_CMD_MATCH_ROM	0x55

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Функциональные команды DS18B20
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Начать замер температуры
#define DS18B20_CMD_CONVERT_T			0x44
// Принять scratchpad от ведущего
#define DS18B20_CMD_WRITE_SCRATCHPAD	0x4E
// Передать scratchpad ведущему
#define DS18B20_CMD_READ_SCRATCHPAD		0xBE


// Макрос для включения такирования указанного порта
#define ONEWIRE_GPIO_CLOCK_ENABLE	__HAL_RCC_GPIOA_CLK_ENABLE

// Глобальная структура с настройками порта
static GPIO_InitTypeDef port_init = {0};

// Функция реализует задержку в микросекундах
static void delay_us(uint32_t delay)
{
	dwt_delay_us(delay);
}


// Функция прижимает 1w шину к земле
void bus_force_down(ds18b20_t* this)
{
	port_init.Mode = GPIO_MODE_OUTPUT_OD;
	HAL_GPIO_Init(this->onewire_port, &port_init);
}


// Функция отпускает 1w шину (и дальше ей управляют ведомые или никто)
void bus_release(ds18b20_t* this)
{
	port_init.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(this->onewire_port, &port_init);
}


// Возвращает текущий логический уровень на 1w шине
bool bus_read(ds18b20_t* this)
{
	return HAL_GPIO_ReadPin(this->onewire_port, this->onewire_pin) == GPIO_PIN_SET;
}


// Статическая таблица необходимая для расчета crc8 контрольной суммы
static const uint8_t onewire_crc_table[] = {
  0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
  0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
  0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
  0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
  0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
  0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
  0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
  0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
  0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
  0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
  0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
  0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
  0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
  0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
  0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
  0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35
};


// Обновляет значение контольной суммы crc применением всех бит байта b.
// Возвращает обновлённое значение контрольной суммы
uint8_t onewire_crc_update(uint8_t crc, uint8_t b) {
  return onewire_crc_table[crc ^ b];
}

// Считает контрольную сумму переданного блока памяти
uint8_t onewire_crc_calc(const uint8_t * data, size_t data_size)
{
	uint8_t crc = 0;
	for (size_t i = 0; i < data_size; i++)
		onewire_crc_update(crc, data[i]);

	return crc;
}


int onewire_init(ds18b20_t* this)
{
	dwt_delay_init();

	ONEWIRE_GPIO_CLOCK_ENABLE();

	port_init.Mode = GPIO_MODE_INPUT; // Начинаем с инпута
	port_init.Pin = this->onewire_pin;
	port_init.Pull = GPIO_NOPULL;
	port_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(this->onewire_port, &port_init);

	// Переводим пин в 0, чтобы при переключении его в output
	// он давал 0 на линии
	HAL_GPIO_WritePin(this->onewire_port, this->onewire_pin, GPIO_PIN_RESET);

	// Начинаем с положения шины "вверху"
	bus_release(this);
	return 0;
}


int onewire_reset(ds18b20_t* this)
{
	// Роняем шину на пол
	bus_force_down(this);
	// ждем 480-960 мкс
	delay_us(500);
	// Отпускам шину
	bus_release(this);

	// чуток ждем 15-60 мкс
	delay_us(60); // с небольшим запасом

	// Сейчас мы должны увидеть как ведомые
	// аж целых 240 мкс удерживают шину на полу
	// (если, конечно, они тут есть)
	// Проверим
	// Возьмем небольшой запасик
	delay_us(20);
	bool bit = bus_read(this); // TODO: добавить оверсамплинг
	// Теперь ждем до конца интервала, чтобы шина вернулась в покой
	delay_us(480-20-60);

	if (!bit)
	{
		// Если шина была на полу, значит на шине кто-то есть
		return 0;
	}
	else
	{
		// Если шина была наверху
		// Значит ведомых нет. Будем считать это ошибкой
		return 1;
	}
}


void onewire_write_bit(ds18b20_t* this, bool bit)
{
	if (bit)
	{
		bus_force_down(this);  // кладем шину на пол
		delay_us(10);      // удерживаем от 1 до 15 мкс
		bus_release(this);     // отпускаем шину
		delay_us(60 - 10); // дожидаемся конца 60 мкс фрейма
	}
	else
	{
		bus_force_down(this);  // кладем шину на пол
		delay_us(60);      // удерживаем 60 мкс
		bus_release(this);     // отпускаем
		delay_us(2);       // чтобы шина успела подняться
	}
}


bool onewire_read_bit(ds18b20_t* this)
{
	// Начинаем фрейм провалом шины
	bus_force_down(this);
	// Подержим её в таком состоянии 10 мкс
	delay_us(5);
	// Отпустим
	bus_release(this);

	// Подождем чтобы она успела подняться
	delay_us(5);
	// Читаем состояние шины
	bool bus_bit = bus_read(this); // TODO: Добавить оверсамплинг

	// Спим до конца фрейма
	delay_us(60-5-5);

	// Возвращаем значение
	return bus_bit;
}


void onewire_write_byte(ds18b20_t* this, uint8_t byte)
{
	for (int i = 0; i < 8; i++)
	{
		bool bit = (byte & 0x01) ? true : false;
		byte = byte >> 1;
		onewire_write_bit(this, bit);
	}
}


uint8_t onewire_read_byte(ds18b20_t* this)
{
	uint8_t accumulator;
	for (int i = 0; i < 8; i++)
	{
		bool bit = onewire_read_bit(this);
		accumulator = accumulator >> 1;
		accumulator = accumulator | ((bit ? 1 : 0) << 7);
	}

	return accumulator;
}


void onewire_write(ds18b20_t* this, const uint8_t * buffer, size_t buffer_size)
{
	for (size_t i = 0; i < buffer_size; i++)
		onewire_write_byte(this, buffer[i]);
}


void onewire_read(ds18b20_t* this, uint8_t * buffer, size_t buffer_size)
{
	for (size_t i = 0; i < buffer_size; i++)
		buffer[i] = onewire_read_byte(this);
}


void onewire_skip_rom(ds18b20_t* this)
{
	onewire_write_byte(this, ONEWIRE_CMD_SKIP_ROM);
}


void onewire_read_rom(ds18b20_t* this, uint8_t * rom_buffer)
{
	onewire_write_byte(this, ONEWIRE_CMD_READ_ROM);
	onewire_read(this, rom_buffer, 8 * sizeof(uint8_t));
}


int ds18b20_set_config(ds18b20_t* this, int8_t alarm_th, int8_t alarm_tl, ds18b20_resulution_t resolution)
{
	// Считаем значение конфигурационного регистра соответствующее разрешению
	uint8_t config_reg;
	switch (resolution)
	{
	case DS18B20_RESOLUTION_9_BIT:
		config_reg = 0x1F; // 0001 1111
		break;

	case DS18B20_RESOLUTION_10_BIT:
		config_reg = 0x3F; // 0011 1111
		break;

	case DS18B20_RESOLUTION_11_BIT:
		config_reg = 0x5F; // 0101 1111
		break;

	case DS18B20_RESOLUTION_12_BIT:
		config_reg = 0x7F; // 0111 1111
		break;

	default:
		// Если что-то пошло не так и к нам пришла дичь, то считаем это ошибкой
		return 100;
	}

	// даем reset на шину
	int reset_status = onewire_reset(this);
	// Если никто не наго не ответил или что-то пошло не так, то продолжать нет смысла
	if (0 != reset_status)
		return reset_status;

	// Пропускаем адресацию
	onewire_skip_rom(this);
	// Даем команду на запись скратчпада
	onewire_write_byte(this, DS18B20_CMD_WRITE_SCRATCHPAD);

	// формируем скратчпад и пишем
	const uint8_t to_write[3] = { alarm_th, alarm_tl, config_reg };
	onewire_write(this, to_write, sizeof(to_write));

	// у нас все ок
	return 0;
}


int ds18b20_start_conversion(ds18b20_t* this)
{
	// даем reset на шину
	int reset_status = onewire_reset(this);
	// Если никто не наго не ответил или что-то пошло не так, то продолжать нет смысла
	if (0 != reset_status)
		return reset_status;

	// Пропускаем адресацию
	onewire_skip_rom(this);
	// Даем команду на начало измерения
	onewire_write_byte(this, DS18B20_CMD_CONVERT_T);

	// у нас все ок
	return 0;
}


int ds18b20_read_raw_temperature(ds18b20_t* this, uint16_t * raw_temperature, bool * crc_ok)
{
	// даем reset на шину
	int reset_status = onewire_reset(this);
	// Если никто на него не ответил или что-то пошло не так, то продолжать нет смысла
	if (0 != reset_status)
		return reset_status;

	// Пропускаем адресацию
	onewire_skip_rom(this);
	// Даем команду на чтение скратчпада
	onewire_write_byte(this, DS18B20_CMD_READ_SCRATCHPAD);

	// Читаем его целиком
	uint8_t buffer[8];
	onewire_read(this, buffer, sizeof(buffer));

	// Считаем контрольную сумму
	if (0 != crc_ok)
	{
		uint8_t crc = onewire_crc_calc(buffer, sizeof(buffer));
		*crc_ok = (0x00 == crc);
	}
	uint16_t temperature_lsb = buffer[0];
	uint16_t temperature_msb = buffer[1];
	*raw_temperature = temperature_lsb | (temperature_msb << 8);

	return 0;
}

#endif /* HAL_GPIO_MODULE_ENABLED */
