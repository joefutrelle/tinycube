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
#include <util/delay.h>
#include "USI_TWI_Master.h"
#include "alpha4.h"
#include "chronodot.h"
#include "eeprom.h"

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

int main(void)
{
  cdot_time_t time;

  // initialize peripherals
  USI_TWI_Master_Initialise();

  a4_begin(); // initialize display
  if(!cdot_init()) { // initialize RTC
    a4_text("dead");
    return 0;
  }
  eep_init(); // initialize EEPROM

  for(;;) {
    if(!cdot_read(&time)) {
      a4_text("fail");
      pause();
    } else {
      show_all(&time);
      // now do eeprom stuff
      uint16_t addr = time.minute;
      uint8_t b = time.second;
      uint8_t c = 0;
      _show_hex("writ", (addr << 8) | b);
      eep_write_byte(addr, b);
      c = eep_read_byte(addr);
      _show_hex("read", (addr << 8) | c);
    }
  }
}
