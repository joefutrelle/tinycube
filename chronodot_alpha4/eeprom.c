#include "eeprom.h"
#include "i2c.h"

#define EEP_ADDR 0x50

#define LSB(b) b & 0xFF
#define MSB(b) b >> 8

void eep_init() {
  // FIXME stub
}

void eep_write_byte(uint16_t addr, uint8_t b) {
  uint8_t buf[] = {
    i2c_write_mode(EEP_ADDR),
    MSB(addr),
    LSB(addr),
    b
  };
  TX(buf,4);
}

uint8_t eep_read_byte(uint16_t addr) {
  uint8_t addr_msg[] = {
    i2c_write_mode(EEP_ADDR),
    MSB(addr),
    LSB(addr)
  };
  TX(addr_msg,3);
  uint8_t read_msg[] = {
    i2c_read_mode(EEP_ADDR),
    0x00
  };
  TX(read_msg,2);
  return read_msg[1];
}
