#include <string.h>
#include <util/delay.h>
#include "eeprom.h"
#include "i2c.h"

#define EEP_ADDR 0x50

#define LSB(b) b & 0xFF
#define MSB(b) b >> 8

#define MAX_EEP_BLOCK 16

void eep_init() {
}

uint8_t eep_err() {
  return USI_TWI_Get_State_Info();
}

uint8_t eep_write_byte(uint16_t addr, uint8_t b) {
  uint8_t buf[] = {
    MSB(addr),
    LSB(addr),
    b
  };
  uint8_t ret = TX(EEP_ADDR,buf,3);
  _delay_ms(5);
  return ret;
}

uint8_t eep_read_byte(uint16_t addr) {
  uint8_t msg[] = {
    MSB(addr),
    LSB(addr)
  };
  if(!(TX(EEP_ADDR,msg,2) &&
       RX(EEP_ADDR,msg,1))) {
    return 0;
  }
  return msg[0];
}

uint8_t eep_write_block(uint16_t addr, void *b, size_t len) {
  uint8_t msg[MAX_EEP_BLOCK+2];
  len = len > MAX_EEP_BLOCK ? MAX_EEP_BLOCK : len;
  msg[0] = MSB(addr);
  msg[1] = LSB(addr);
  memcpy(&(msg[2]),b,len);
  return TX(EEP_ADDR,msg,len+2);
}

uint8_t eep_read_block(uint16_t addr, void *b, size_t len) {
  return 1; // FIXME implement
}
