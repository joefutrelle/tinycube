#pragma once
#include <avr/io.h>

#define DS3231_ADDR 0x68

typedef struct __attribute__((packed)) {
  uint16_t year;
  uint8_t month;
  uint8_t date;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t day;
  int16_t temp4c;
} cdot_time_t;

uint8_t cdot_init();
uint8_t cdot_read(cdot_time_t *time);
uint8_t cdot_read_temp(cdot_time_t *time);
void cdot_set(cdot_time_t *time);
void cdot_pack(cdot_time_t *time, uint8_t *buf);
void cdot_unpack(uint8_t *buf, cdot_time_t *time);
