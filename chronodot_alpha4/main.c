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
      // display the year
      a4_int((time.hour * 100) + time.minute);
      pause();
    }
  }
}
