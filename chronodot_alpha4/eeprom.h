#pragma once
#include <avr/io.h>

/* Microchip 24LC256 256kbit i2c EEPROM
 * Datasheet
 * http://ww1.microchip.com/downloads/en/DeviceDoc/21203M.pdf
 */
void eep_init();
uint8_t eep_err();
uint8_t eep_write_byte(uint16_t addr, uint8_t b);
uint8_t eep_read_byte(uint16_t addr);
uint8_t eep_write_block(uint16_t addr, void *buf, size_t len);
uint8_t eep_read_block(uint16_t addr, void *buf, size_t len);
