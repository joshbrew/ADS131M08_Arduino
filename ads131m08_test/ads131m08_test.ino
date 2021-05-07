#include "ADS131M08.h"

//Joshua Brewster, Jacob Tinkhauser

#define MISO_PIN              19
#define MOSI_PIN              23
#define SCK_PIN               18
#define CS_PIN                5

#define VSPI_MISO             MISO_PIN
#define VSPI_MOSI             MOSI_PIN
#define VSPI_SCLK             SCK_PIN
#define VSPI_SS               CS_PIN

#define DRDY_PIN              21
#define XTAL_PIN              22
#define RESET                 17

int CLKOUT  =                 8192000; //XTAL speed (50% duty cycle PWM)

ADS131M08 adc(CS_PIN,XTAL_PIN,DRDY_PIN,8192000);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Setting SPI");
  adc.spi = new SPIClass(VSPI);
  Serial.println("Booting SPI");
  adc.init(CLKOUT);

  delay(100);
  uint16_t id = adc.readReg(ADS131_ID);
  Serial.print("ID: ");
  Serial.println(id, HEX);
  
  uint16_t stat = adc.readReg(ADS131_STATUS);
  Serial.print("Status: ");
  Serial.println(stat, HEX);

  uint16_t Mode = adc.readReg(ADS131_MODE);
  Serial.print("Mode: ");
  Serial.println(Mode, HEX);

  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, LOW);
  delay(1);
    digitalWrite(RESET, HIGH);

}

void loop() {
 if(digitalRead(DRDY_PIN)) {
    Serial.print("Channel 0 Reading: ");
    Serial.println(adc.readChannelSingle(0)); //Just read the first one
  }
  else {
  }
}
