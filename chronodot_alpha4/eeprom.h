#pragma once
#include <avr/io.h>

void eep_init();
uint8_t eep_err();
uint8_t eep_write_byte(uint16_t addr, uint8_t b);
uint8_t eep_read_byte(uint16_t addr);
