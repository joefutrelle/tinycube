#pragma once
#include <avr/io.h>

void prevent_wdt_reset();
void setup_wdt();
void go_to_sleep();
uint8_t get_sleep_count();
void reset_sleep_count();
