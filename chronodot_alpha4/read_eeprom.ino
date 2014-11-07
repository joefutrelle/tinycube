#include <Wire.h>

#define EEP_ADDR 0x50

void dump();

void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  while(!Serial)
    ;
    
  Serial.println("Hello");
  
  dump();
}

void sendAddr(uint16_t addr) {
  Wire.beginTransmission(EEP_ADDR);
  Wire.write(addr >> 8);
  Wire.write(addr & 0xFF);
  Wire.endTransmission();
}

void dump()
{
  char buf[32];
  for(uint16_t memAddr = 0; memAddr < 512; memAddr += 32) {
    sendAddr(memAddr);
    Wire.requestFrom(EEP_ADDR, 32);
    for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 8 && Wire.available(); j++) {
        sprintf(buf," %02x", Wire.read());
        Serial.print(buf);
      }
      Serial.println();
    }
  }
}

void loop() { }
