#include "ADS131M08.h"

#define MISO_PIN              23
#define MOSI_PIN              19
#define SCK_PIN               18
#define CS_PIN                5

#define VSPI_MISO             MISO_PIN
#define VSPI_MOSI             MOSI_PIN
#define VSPI_SCLK             SCK_PIN
#define VSPI_SS               CS_PIN

#define DRDY_PIN              21
#define XTAL_PIN              22

int CLKOUT  =                 8192000; //XTAL speed (50% duty cycle PWM)

//ADS131M08 adc(CS_PIN,XTAL_PIN,DRDY_PIN,2000000);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Setting SPI");
  adc.spi = new SPIClass(VSPI);
  Serial.println("Booting SPI");
  adc.init(CLKOUT);

  delay(100);
  uint16_t stat = adc.readReg(ADS131_STATUS);
  Serial.print("Status: ");
  Serial.println(stat);
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
