#include <string.h>
#include "chronodot.h"
#include "i2c.h"

#define BCD2INT(b) (((b & 0xF0) >> 4) * 10 + (b & 0x0F))

// used for setting
uint8_t set_reg(uint8_t addr, uint8_t value) {
  uint8_t buf[] = {
    i2c_write_mode(DS3231_ADDR),
    addr,
    value
  };
  return TX(buf,3);
}

uint8_t send_reg_addr(uint8_t addr) {
  uint8_t buf[] = {
    i2c_write_mode(DS3231_ADDR),
    addr
  };
  return TX(buf,2);
}

uint8_t cdot_init() {
  // clear /EOSC bit
  // Sometimes necessary to ensure that the clock
  // keeps running on just battery power. Once set,
  // it shouldn't need to be reset but it's a good
  // idea to make sure.
  return set_reg(0x0E,0b00011100);
}

uint8_t cdot_raw_read(size_t addr, uint8_t *buf, int n) {
  if(!send_reg_addr(addr))
    return FALSE;
  int i = 0;
  buf[i++] = i2c_read_mode(DS3231_ADDR);
  while(i < n) {
    buf[i++] = 0xFF;
  }
  return TX(buf,n+1);
}

uint8_t cdot_read(cdot_time_t *time) {
  uint8_t buf[20];
  //
  if(!cdot_raw_read(0x00, &(buf[0]), 8))
    return FALSE;

  int i = 1;
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
  time->day = raw_day;

  // temperature
  cdot_raw_read(0x11, &(buf[0]), 2);
  int8_t tempMsb = buf[2];
  uint8_t tempLsb = buf[3];
  time->temp4c = (tempMsb << 2) | (tempLsb >> 6);

  return TRUE;
}

