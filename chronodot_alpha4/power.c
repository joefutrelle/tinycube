#include <avr/io.h>
#include <util/delay.h>

#define POWER_DD DDRB
#define POWER_PORT PORTB

#define POWER_DELAY 5

void power_init(int pin) {
  POWER_DD |= _BV(pin);
}

void power_up(int pin) {
  POWER_PORT |= _BV(pin);
  _delay_ms(POWER_DELAY);
}

void power_down(int pin) {
  PORTB &= ~_BV(pin);
  _delay_ms(5);
}
