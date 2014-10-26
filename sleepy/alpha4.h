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
#pragma once

// ht16k33 4-digit alpha display stuff

// i2c address
#define HT16K33_ADDR 0x70

// commands and parameters
#define HT16K33_CMD_OSC_ON 0x21
#define HT16K33_CMD_BLINK 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_BLINK_OFF 0
#define HT16K33_BLINK_2HZ  1
#define HT16K33_BLINK_1HZ  2
#define HT16K33_BLINK_HALFHZ  3

#define HT16K33_CMD_BRIGHTNESS 0xE0

// send a command to the alpha display
void a4_cmd(uint8_t cmd);
// display a specific LED configuration specified
// using four 16-bit flags (lit high)
void a4_display(uint16_t *raw);
// make the display blank
void a4_clear();
// start up the display and
// - set brightness to max
// - disable any blinking
// - clear the display
void a4_begin();
// display a value in hexidecimal
void a4_hex(uint16_t val);
// display alphanumeric text. upcases all letters.
// only letters and numbers and space. nothing else.
void a4_alpha(char *text);

// faster version of alpha
// faster, use exactly four lowercase letters only
void a4_text(char *text);

