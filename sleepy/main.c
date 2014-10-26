#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "i2c.h"
#include "alpha4.h"

void pause() {
  int i;
  for(i = 0; i < 8; i++) {
    _delay_ms(200);
  }
}

// cribbing from http://electronics.stackexchange.com/a/74850

void prevent_wdt_reset() {
  if(MCUSR & _BV(WDRF)){            // If a reset was caused by the Watchdog Timer...
    MCUSR &= ~_BV(WDRF);                 // Clear the WDT reset flag
    WDTCR |= (_BV(WDCE) | _BV(WDE));   // Enable the WD Change Bit
    WDTCR = 0x00;                      // Disable the WDT
  }
}

void setup_wdt() {
  cli(); // with global interrupts disabled,
  // Set up Watch Dog Timer for Inactivity
  WDTCR |= (_BV(WDCE) | _BV(WDE));   // Enable the WD Change Bit
  WDTCR =   _BV(WDIE) |              // Enable WDT Interrupt
    _BV(WDP3) | _BV(WDP0); // ~8s (depending on scaling)
  sei(); // now re-enable global interrupts
}

volatile uint8_t sleep_count = 0;

// ISR for watchdog timer increments sleep counter;
ISR(WDT_vect) {
  sleep_count++;
}

#define POWER_PIN PB1

void power_init() {
  // we will power peripherals via an output pin
  DDRB |= _BV(POWER_PIN);
}

void power_up() {
  // power up peripherals
  PORTB |= _BV(POWER_PIN);
  // wait a moment
  _delay_ms(5);
}

void power_down() {
  // power down peripherals
  PORTB &= ~_BV(POWER_PIN);
  // wait a moment
  _delay_ms(5);
}

int main(void) {
  // avoid reset race conditions
  prevent_wdt_reset();
  // setup (or re-setup) WDT
  setup_wdt();

  // initialize peripherals
  USI_TWI_Master_Initialise();
  // power them up
  power_init();
  power_up();
  // start a4 display
  a4_begin();
  a4_clear();
  // say hello
  a4_text("helo");
  pause();

  // go into sleep/wake cycle
  for(;;) {
    // prepare to sleep
    a4_text("doze");
    pause();

    // attempt to set sleep mode and sleep
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    // power down peripherals
    power_down();
    // now go to sleep
    sleep_mode();

    // CPU is now sleeping.

    // control returns upon wakeup
    // power up peripherals
    power_up();
    // display a wakeup message
    a4_begin();
    a4_text("wake");
    pause();

    // here's where to actually do something
    // that requires being awake
  }

  // done
  return 0;
}
