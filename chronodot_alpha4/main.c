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
#include <string.h>
#include <util/delay.h>
#include "USI_TWI_Master.h"
#include "i2c.h"
#include "power.h"
#include "alpha4.h"
#include "chronodot.h"
#include "eeprom.h"
#include "wdt_sleep.h"

/* I2C bus is powered from PB1
 * to enable powering peripherals up and down
 * during sleep
 */
#define I2C_POWER_PIN PB1

#define DEBUG

#ifdef DEBUG

void pause() {
  _delay_ms(200);
  _delay_ms(200);
  _delay_ms(200);
  _delay_ms(200);
}

void a4_int(uint16_t i) {
  char text[4];
  uint16_t rem = i;
  text[0] = '0' + (rem / 1000);
  rem %= 1000;
  text[1] = '0' + (rem / 100);
  rem %= 100;
  text[2] = '0' + (rem / 10);
  rem %= 10;
  text[3] = '0' + rem;
  a4_alpha(text);
}

void _show(char *text, uint16_t val) {
  a4_text(text);
  pause();
  a4_int(val);
  pause();
  pause();
}

void _show_hex(char *text, uint16_t val) {
  a4_text(text);
  pause();
  a4_hex(val);
  pause();
  pause();
}

void show_all(cdot_time_t *time) {
  _show("year", time->year);
  _show("date", (time->month * 100) + time->date);
  _show("time", (time->hour * 100) + time->minute);
  _show("temp", ((time->temp4c / 4) * 100) + ((time->temp4c & 0x3) * 25));
}

#endif

uint16_t log_addr = 0;

#ifdef DEBUG
int clock_set = 1; // is the clock set already?

void set_clock(cdot_time_t *time) {
  time->year = 2014; // etc
  time->month = 11;
  time->date = 7;
  time->hour = 15;
  time->minute = 5;
  time->second = 0;
  cdot_set(time);
}
#endif

void log_data() {
  cdot_init(); // initialize RTC
  eep_init(); // initialize EEPROM

#ifdef DEBUG
  a4_init(); // initialize display
  a4_text("wake");
  pause();
#endif

  a4_text("wait");

  cdot_time_t time;
  uint8_t packed[16];
  memset(&(packed[0]),0x00,16); // necessary?

  int ret = cdot_read_temp(&time);

#ifdef DEBUG
  if(!clock_set) {
    clock_set = 1;
    a4_text("cset");
    pause();
    set_clock(&time);
  }
#endif

#ifdef DEBUG
  if(!ret) {
      a4_text("crfl");
      pause();
  } else {
    show_all(&time);
  }
#endif

  cdot_pack(&time, packed);
  // now pack temp
  packed[5] = time.temp4c >> 8;
  packed[6] = time.temp4c & 0xFF;
  /*
  int i = 0;
  for(i = 0; i < 16; i++) {
    ret = eep_write_byte(log_addr+i, packed[i]);
  }// FIXME write block
  */
#ifdef DEBUG
  _show_hex("writ",log_addr);
#endif
  eep_write_block(log_addr, packed, 16);
  log_addr += 8;

#ifdef DEBUG
  if(!ret) {
    a4_text("epfl");
    pause();
  }

  a4_text("doze");
  pause();
  a4_clear();
#endif
}

#ifdef DEBUG
void motd() {
  a4_text("rset");
  pause();
}
#endif

int main(void)
{
  // avoid reset race conditions
  prevent_wdt_reset();
  // setup (or re-setup) WDT
  setup_wdt();
  // initialize I2C library
  USI_TWI_Master_Initialise();
  // initialize I2C power
  power_init(I2C_POWER_PIN);

  // before doing anything, wait a bit
  _delay_ms(100);

#ifdef DEBUG
  with_power(I2C_POWER_PIN, &motd);
#endif

  // go into sleep/wake cycle
  for(;;) {
    // waking

    // with power on, log data
    with_power(I2C_POWER_PIN, &log_data);

    // sleeping
    reset_sleep_count();
    while(get_sleep_count() < 3) { // minimal sleep
      go_to_sleep();
    }
  }
}
