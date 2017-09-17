#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>

#include "systick.h"

volatile uint32_t count = 0;

void sys_tick_handler(void) {
  count ++;
}

void systick_init(uint64_t freq) {
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
  systick_set_reload(freq / 8 / 1000 - 1);
  systick_interrupt_enable();
  systick_counter_enable();
}

void delay_ms(uint32_t ms) {
  uint32_t endtime = ms + count;

  while(count < endtime);
}
