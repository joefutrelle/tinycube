#include <util/delay.h>
#include "eeprom.h"
#include "i2c.h"

#define EEP_ADDR 0x50

#define LSB(b) b & 0xFF
#define MSB(b) b >> 8

void eep_init() {
}

uint8_t eep_err() {
  return USI_TWI_Get_State_Info();
}

uint8_t eep_write_byte(uint16_t addr, uint8_t b) {
  uint8_t buf[] = {
    i2c_write_mode(EEP_ADDR),
    MSB(addr),
    LSB(addr),
    b
  };
  uint8_t ret = TX(buf,4);
  _delay_ms(5);
  return ret;
}

uint8_t eep_read_byte(uint16_t addr) {
  uint8_t msg[] = {
    i2c_write_mode(EEP_ADDR),
    MSB(addr),
    LSB(addr)
  };
  if(!TX(msg,3)) {
    return 0x00;
  }
  msg[0] = i2c_read_mode(EEP_ADDR);
  if(!TX(msg,2)) {
    return 0x00;
  }
  return msg[1];
}
