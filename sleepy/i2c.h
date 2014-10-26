#pragma once
#include "USI_TWI_Master.h"

// i2c helper macros
#define i2c_write_mode(a) (a << 1)
#define i2c_read_mode(a) ((a << 1) | 0x1)

#define TX(a,b,n) USI_TWI_Start_Transceiver_With_Data(i2c_write_mode(a),b,n)
#define RX(a,b,n) USI_TWI_Start_Transceiver_With_Data(i2c_read_mode(a),b,n)
