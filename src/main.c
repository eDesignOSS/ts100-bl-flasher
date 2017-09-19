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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/scb.h>

#include "systick.h"
#include "crc32.h"
#include "oled.h"

extern uint32_t _binary_payload_rom_start;
extern uint32_t _binary_payload_rom_end;
extern uint32_t _binary_payload_rom_size;

void i2c_init(void);
void i2c_init(void) {
	rcc_periph_clock_enable(RCC_GPIOB | RCC_I2C1);
  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
                GPIO_I2C1_SCL | GPIO_I2C1_SDA);
  i2c_peripheral_disable(I2C1);
  i2c_set_clock_frequency(I2C1, I2C_CR2_FREQ_24MHZ);
  i2c_set_fast_mode(I2C1);
  i2c_set_ccr(I2C1, 0x14);
  i2c_set_trise(I2C1, 0x08);
  i2c_peripheral_enable(I2C1);
}

void gpio_init(void);
void gpio_init(void) {
	rcc_periph_clock_enable(RCC_GPIOC);
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                GPIO_CNF_INPUT_PULL_UPDOWN,
                GPIO9);
  gpio_set(GPIOA, GPIO9);
}

void keydnup(void);
void keydnup(void) {
  // Key down
  while(gpio_get(GPIOA, GPIO9)) { delay_ms(1); }
  // Key up
  while(!gpio_get(GPIOA, GPIO9)) { delay_ms(1); }
}

int main(void)
{
  SCB_VTOR=0x4000;
  rcc_clock_setup_in_hsi_out_48mhz();

  systick_init(48000000);
  i2c_init();
  gpio_init();
  Init_Oled(0);
  delay_ms(1);

  Clear_Screen();
  OLED_Sync();
  OLED_DrawString("BL Flash", 8);
  OLED_Sync();
  delay_ms(1000);
  Clear_Screen();
  OLED_DrawString("CONT?", 5);
  OLED_Sync();

  keydnup();

  uint32_t bl_crc32 = 0;
  char bl_crc32_hex[9] = {0, };
  uint8_t *bl_ptr = (uint8_t*)&_binary_payload_rom_start;
  size_t bl_size = (size_t)&_binary_payload_rom_size;

  bl_crc32 = crc32(bl_crc32, bl_ptr, bl_size);

  for(int i = 0; i < 8; i++) {
    bl_crc32_hex[7 - i] = (bl_crc32 & 0xF) + '0';
    if (bl_crc32_hex[7 - i] > '9') {
      bl_crc32_hex[7 - i] += 7;
    }
    bl_crc32 = bl_crc32 >> 4;
  }

  Clear_Screen();
  OLED_DrawString("CHK CRC", 7);
  OLED_Sync();
  delay_ms(1000);
  Clear_Screen();
  OLED_DrawString(bl_crc32_hex, 8);
  OLED_Sync();

  keydnup();

  // Flash firmware
  uint8_t *flashptr = (unsigned char*)0x08000000;

  for(uint32_t i = 0; i < (0x4000 / 1024); i++) {
    flash_unlock();
    flash_erase_page((uint32_t)(flashptr + (i*1024)));
    flash_lock();
  }

  for(size_t i = 0; i < bl_size; i+=4) {
    int buf = 0;
    flash_unlock();
    memcpy(&buf, bl_ptr + i, 4);
    flash_program_word((uint32_t)(flashptr + i), buf);
    flash_lock();
  }

  Clear_Screen();
  OLED_DrawString("Done!", 5);
  OLED_Sync();

  for(;;);
}
