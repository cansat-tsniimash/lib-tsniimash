#include <Delay/delay.h>
#include <stm32f0xx.h>
#define F_CPU 48000000UL

int delay_init(void)
{
	//SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk; //ставим бит номер 0 в 0 (на всякий случай) //т.е. не используем прерывания
	//SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
   	return 0;
}


void delay_us(uint32_t us)
{
	//SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	//SysTick->LOAD = (((F_CPU/1000000)*us)-30)/6;    //загружаем число тактов ожидания (не более 2^24)// 30 - вычитание тактор накладных    расходов
	//SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //включаем таймер SysTick

	//while(0x00 == (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) {}    //0x10000 - бит номер 16 //SysTick_CTRL_COUNTFLAG_Msk //(uint32_t)0x10000U
	//SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; //выключение таймер SysTick
	//SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

	for (int i = 0; i < (((F_CPU/1000000)*us)-30); i++)
	{
		volatile a = 3;
	}
}


void delay_ms(uint32_t ms)
{
	while(ms)
	  {
	    ms--;
	  delay_us(1000);
	  }
}
