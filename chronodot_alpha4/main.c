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

void show_all(cdot_time_t *time) {
  a4_int(time->year);
  pause();
  a4_int((time->month * 100) + time->date);
  pause();
  a4_int((time->hour * 100) + time->minute);
  pause();
  a4_int(time->second);
  pause();
}

int main(void)
{
  //uint8_t cdot_buf[32];

  USI_TWI_Master_Initialise();

  cdot_time_t time;

  a4_begin();
  if(!cdot_init()) {
    a4_text("dead");
    return 0;
  }

  for(;;) {
    if(!cdot_read(&time)) {
      a4_text("fail");
      pause();
    } else {
      cdot_time_t up;
      a4_text("time");
      pause();
      show_all(&time);
      // try pack/unpack
      uint8_t buf[4];
      cdot_pack(&time, &(buf[0]));
      cdot_unpack(&(buf[0]), &up);
      // display the time
      a4_text("pack");
      pause();
      show_all(&up);
    }
  }
}
