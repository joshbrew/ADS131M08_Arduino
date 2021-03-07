#include "ADS131M08.h"

ADS131M08 adc;

void setup() {
  // put your setup code here, to run once:
  adc.spi = new SPIClass(VSPI);
  adc.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t stat = adc.readReg(ADS131_STATUS);

  uint8_t drdy = stat >> 8;
  if(drdy == 255) { //All channels ready to sample
    Serial.println(adc.readChannelSingle(0)); //Just read the first one
  }
  else {
  }
}
