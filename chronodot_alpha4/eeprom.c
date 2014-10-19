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
  if(!TX(EEP_ADDR,msg,2) &&
     RX(EEP_ADDR,msg,1)) {
    return 0;
  }
  return msg[0];
}
