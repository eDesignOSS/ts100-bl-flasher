/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/cm3/systick.h>

#include "i2c.h"
#include "systick.h"

extern uint32_t _binary_payload_rom_start;
extern uint32_t _binary_payload_rom_end;
extern uint32_t _binary_payload_rom_size;

int main(void)
{
	rcc_clock_setup_in_hsi_out_48mhz();

  systick_init(48000000);
  i2c_init();

  delay_ms(1);

  // Flash firmware
  uint8_t *ptr = (uint8_t*)&_binary_payload_rom_start;
  uint8_t *flashptr = (unsigned char*)0x08000000;

  for(uint32_t i = 0; i < (0x4000 / 1024); i++) {
    flash_unlock();
    flash_erase_page((uint32_t)(flashptr + (i*1024)));
    flash_lock();
  }

  for(uint32_t i = 0; i < _binary_payload_rom_size; i+=4) {
    int buf = 0;
    flash_unlock();
    memcpy(&buf, ptr + i, 4);
    flash_program_word((uint32_t)(flashptr + i), buf);
    flash_lock();
  }

  for(;;);
}
