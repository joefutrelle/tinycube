#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include "wdt_sleep.h"

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

uint8_t get_sleep_count() {
  return sleep_count;
}

void reset_sleep_count() {
  sleep_count = 0;
}

void go_to_sleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_mode();
}
