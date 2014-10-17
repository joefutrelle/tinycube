#pragma once
#include <avr/io.h>

void eep_init();
void eep_write_byte(uint16_t addr, uint8_t b);
uint8_t eep_read_byte(uint16_t addr);
