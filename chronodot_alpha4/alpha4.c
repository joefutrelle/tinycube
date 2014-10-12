/* Name: main.c
 * Author: Joe Futrelle
 * License: CC0 - Free as in freedom
 */
/* Adafruit 4-digit alphanumeric backpack, I2C
 * Via USI_TWI_master from application note AVR310
 * http://www.atmel.com/Images/Atmel-2561-Using-the-USI-Module-as-a-I2C-Master_AP-Note_AVR310.pdf
 */
/* Datasheet for HT16K33
 * http://www.adafruit.com/datasheets/ht16K33v110.pdf
 */
/* Adafruit's Arduino library:
 * https://github.com/adafruit/Adafruit-LED-Backpack-Library/blob/master/Adafruit_LEDBackpack.cpp
 */
#include <avr/io.h>
#include <stdlib.h>
#include "USI_TWI_Master.h"
#include "alpha4.h"
#include "i2c.h"

// font (takes up some RAM, could be moved to flash)
uint16_t a4_font[] = {
  0b0000000000111111, // 0
  0b0000000000000110, // 1
  0b0000000011011011, // 2
  0b0000000011001111, // 3
  0b0000000011100110, // 4
  0b0000000011101101, // 5
  0b0000000011111101, // 6
  0b0000000000000111, // 7
  0b0000000011111111, // 8
  0b0000000011101111, // 9
  0b0000000011110111, // A
  0b0001001010001111, // B
  0b0000000000111001, // C
  0b0001001000001111, // D
  0b0000000011111001, // E
  0b0000000011110001, // F
  0b0000000010111101, // G
  0b0000000011110110, // H
  0b0001001000000000, // I
  0b0000000000011110, // J
  0b0010010001110000, // K
  0b0000000000111000, // L
  0b0000010100110110, // M
  0b0010000100110110, // N
  0b0000000000111111, // O
  0b0000000011110011, // P
  0b0010000000111111, // Q
  0b0010000011110011, // R
  0b0000000011101101, // S
  0b0001001000000001, // T
  0b0000000000111110, // U
  0b0000110000110000, // V
  0b0010100000110110, // W
  0b0010110100000000, // X
  0b0001010100000000, // Y
  0b0000110000001001, // Z
};

// alpha stuff

// send a command to the alpha display
void a4_cmd(uint8_t cmd) {
  uint8_t buf[] = {
    i2c_write_mode(HT16K33_ADDR),
    cmd
  };
  TX(buf,2);
}

#define LSB(b) b & 0xFF
#define MSB(b) b >> 8
#define INT16(i) LSB(i), MSB(i)

// display a specific LED configuration specified
// using four 16-bit flags (lit high)
void a4_display(uint16_t *raw) {
  uint8_t buf[] = {
    i2c_write_mode(HT16K33_ADDR),
    0x00,
    INT16(raw[0]),
    INT16(raw[1]),
    INT16(raw[2]),
    INT16(raw[3])
  };
  TX(buf,10);
}

// make the display blank
void a4_clear() {
  uint16_t raw[] = { 0x0000, 0x0000, 0x0000, 0x0000 };
  a4_display(raw);
}

// start up the display and
// - set brightness to max
// - disable any blinking
// - clear the display
void a4_begin() {
  a4_cmd(HT16K33_CMD_OSC_ON);
  a4_cmd(HT16K33_CMD_BRIGHTNESS | 15); // set brightness to 15
  a4_cmd(HT16K33_CMD_BLINK | HT16K33_BLINK_DISPLAYON); // no blink
  a4_clear();
}

// display a value in hexidecimal
void a4_hex(uint16_t val) {
  uint16_t raw[4];
  int i;
  for(i = 0; i < 4; i++) {
    uint8_t b = (val >> (12 - (4 * i))) & 0xF;
    raw[i] = a4_font[b];
  }
  a4_display(raw);
}

// display alphanumeric text. upcases all letters.
// only letters and numbers and space. nothing else.
void a4_alpha(char *text) {
  uint16_t raw[4];
  int i;
  for(i = 0; i < 4; i++) {
    char c = *(text + i);
    if(c >= 'a' && c <= 'z') {
      raw[i] = a4_font[c - 87];
    } else if(c >= 'A' && c <= 'Z') {
      raw[i] = a4_font[c - 55];
    } else if(c >= '0' && c <= '9') {
      raw[i] = a4_font[c - 48];
    } else {
      raw[i] = 0x0000;
    }
  }
  a4_display(raw);
}

// faster version of alpha
// faster, use exactly four lowercase letters only
void a4_text(char *text) {
  uint16_t raw[4];
  int i;
  for(i = 0; i < 4; i++) {
    uint8_t j = (*(text + i)) - 87;
    raw[i] = a4_font[j];
  }
  a4_display(raw);
}
