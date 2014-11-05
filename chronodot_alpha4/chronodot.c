#include <string.h>
#include <util/delay.h>
#include "chronodot.h"
#include "i2c.h"

// interpret binary-coded-decimal for two-digit numbers
#define BCD2INT(b) ((((b) & 0xF0) >> 4) * 10 + ((b) & 0x0F))
#define INT2BCD(i) ((((i) / 10) << 4) | ((i) % 10))

#define CTL_REG 0x0E
#define STATUS_REG 0x0F
#define CONV 5
#define BSY 2
#define CTL_DEFAULT 0b00011100

// used for setting the clock
uint8_t set_reg(uint8_t addr, uint8_t value) {
  uint8_t buf[] = {
    addr,
    value
  };
  return TX(DS3231_ADDR,buf,2);
}

// used to transmit register before receiving
uint8_t send_reg_addr(uint8_t addr) {
  uint8_t buf[] = {
    addr
  };
  return TX(DS3231_ADDR,buf,1);
}

// set up clock
uint8_t cdot_init() {
  // clear /EOSC bit
  // Sometimes necessary to ensure that the clock
  // keeps running on just battery power. Once set,
  // it shouldn't need to be reset but it's a good
  // idea to make sure.
  //uint8_t ctl_reg = 0b00011100;
  return set_reg(CTL_REG, CTL_DEFAULT);
}

uint8_t cdot_raw_read(size_t addr, uint8_t *buf, int n) {
  if(!send_reg_addr(addr))
    return FALSE;
  return RX(DS3231_ADDR,buf,n);
}

uint8_t get_reg(size_t addr) {
  uint8_t buf[1];
  if(cdot_raw_read(addr, &(buf[0]), 1)) {
    return buf[0];
  }
  return FALSE;
}

// read the clock, populating the provided time
uint8_t cdot_read(cdot_time_t *time) {
  uint8_t buf[8];
  //
  if(!cdot_raw_read(0x00, &(buf[0]), 7))
    return FALSE;

  int i = 0;
  uint8_t raw_second = buf[i++];
  uint8_t raw_minute = buf[i++];
  uint8_t raw_hour = buf[i++];
  uint8_t raw_day = buf[i++];
  uint8_t raw_date = buf[i++];
  uint8_t raw_month = buf[i++];
  uint8_t raw_year = buf[i++];
  
  time->year = BCD2INT(raw_year) + 2000;
  time->second = BCD2INT(raw_second);
  time->minute = BCD2INT(raw_minute);
  time->hour = ((raw_hour & 0x20) >> 5) * 20 + ((raw_hour & 0x10) >> 4) * 10 + (raw_hour & 0x0F); // assume 24hr mode

  time->month = BCD2INT(raw_month);
  time->date = BCD2INT(raw_date);
  time->day = raw_day & 0x03;

  // temperature
  cdot_raw_read(0x11, &(buf[0]), 2);
  int8_t tempMsb = buf[0];
  uint8_t tempLsb = buf[1];
  time->temp4c = (tempMsb * 4) + (tempLsb >> 6);

  return TRUE;
}

uint8_t cdot_read_temp(cdot_time_t *time) {
  // wait until not BSY
  while(get_reg(STATUS_REG) & _BV(BSY))
    ;
  // initiate conversion
  set_reg(CTL_REG, CTL_DEFAULT | _BV(CONV)); // CONV
  // wait until CONV goes to zero
  while(get_reg(CTL_REG) & _BV(CONV))
    ;
  return cdot_read(time);
}

// set the clock. erases any existing setting. ignores temperature
// as that is a read-only register.
void cdot_set(cdot_time_t *time) {
  int i = 0;
  set_reg(i++, INT2BCD(time->second));
  set_reg(i++, INT2BCD(time->minute));
  set_reg(i++, INT2BCD(time->hour)); // assume 24-hour
  set_reg(i++, time->day);
  set_reg(i++, INT2BCD(time->date));
  set_reg(i++, INT2BCD(time->month));
  set_reg(i++, INT2BCD(time->year - 2000));
}

// slightly more compact 5-byte representation
void cdot_pack(cdot_time_t *time, uint8_t *buf) {
  // buf: yyyy yyym / mmmd dddd / 0DDD 00HH / HHHH MMMM / MMSS SSSS
  // y = year, m = monh, d = date, D=day of week, H=hours, M=minutes, S=seconds
  uint16_t y = time->year - 2000;
  buf[0] = (y << 1) | ((time->month >> 3) & 0x01);
  buf[1] = (time->month << 5) | (time->date & 0x1F);
  buf[2] = (time->day << 4) | ((time->hour >> 4) & 0x03);
  buf[3] = (time->hour << 4) | ((time->minute >> 2) & 0x0F);
  buf[4] = (time->minute << 6) | (time->second & 0x3F);
}

void cdot_unpack(uint8_t *buf, cdot_time_t *time) {
  time->year = (buf[0] >> 1) + 2000;
  time->month = ((buf[0] & 0x01) << 3) | (buf[1] >> 5);
  time->date = buf[1] & 0x1F;
  time->day = buf[2] >> 4;
  time->hour = ((buf[2] & 0x03) << 4) | (buf[3] >> 4);
  time->minute = ((buf[3] & 0x0F) << 2) | (buf[4] >> 6);
  time->second = buf[4] & 0x3F;
}
