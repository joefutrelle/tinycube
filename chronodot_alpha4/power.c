#include <avr/io.h>
#include <util/delay.h>
#include "power.h"

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
  POWER_PORT &= ~_BV(pin);
  _delay_ms(POWER_DELAY);
}

void with_power(int pin, void (*fn)()) {
  power_up(pin);
  (*fn)();
  power_down(pin);
}
