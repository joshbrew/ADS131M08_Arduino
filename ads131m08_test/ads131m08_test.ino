#include "ADS131M08.h"

#define MISO_PIN              19
#define MOSI_PIN              23
#define SCK_PIN               18
#define CS_PIN                5

#define VSPI_MISO             MISO
#define VSPI_MOSI             MOSI
#define VSPI_SCLK             SCK
#define VSPI_SS               SS_PIN

#define DRDY_PIN              21
#define XTAL_PIN              22

ADS131M08 adc(CS_PIN,XTAL_PIN,DRDY_PIN,1000000);

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
