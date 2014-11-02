#pragma once

void power_init(int pin);
void power_up(int pin);
void power_down(int pin);

void with_power(int pin, void (*fn)());
