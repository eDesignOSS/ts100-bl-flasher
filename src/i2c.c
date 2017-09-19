#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>

#include "i2c.h"

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

void i2c_write(uint8_t *data, uint32_t size, uint8_t addr) {
  i2c_send_start(I2C1);

  // Wait for I2C start pkt
  while (!((I2C_SR1(I2C1) & I2C_SR1_SB)
           & (I2C_SR2(I2C1) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

  i2c_send_7bit_address(I2C1, addr, I2C_WRITE);
  while (!(I2C_SR1(I2C1) & I2C_SR1_ADDR));

  for(uint32_t i = 0; i < size; i++) {
    i2c_send_data(I2C1, data[i]);
    while (!(I2C_SR1(I2C1) & I2C_SR1_BTF));
  }

  while (!(I2C_SR1(I2C1) & I2C_SR1_TxE));
  i2c_send_stop(I2C1);
}
