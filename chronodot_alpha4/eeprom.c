#include "eeprom.h"

// FIXME mock
#define MOCK_SIZE 64
uint8_t mock_eeprom[MOCK_SIZE];

void eep_init() {
  // FIXME stub
}

void eep_write_byte(uint16_t addr, uint8_t b) {
  addr %= MOCK_SIZE;
  mock_eeprom[addr] = b;
}

uint8_t eep_read_byte(uint16_t addr) {
  addr %= MOCK_SIZE;
  return mock_eeprom[addr];
}
