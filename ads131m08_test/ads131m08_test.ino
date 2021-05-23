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

  adc.writeReg(ADS131_CLOCK,0b1111111100011010); //
  uint16_t clkreg = adc.readReg(ADS131_CLOCK);
  Serial.print("CLOCK: ");
  Serial.println(clkreg,BIN);
  
  //adc.setGain(64);
  
  uint16_t gainreg = adc.readReg(ADS131_GAIN1);
  Serial.print("GAIN1: ");
  Serial.println(gainreg, BIN);
  
  //adc.globalChop(true,2);
  
  uint16_t id = adc.readReg(ADS131_ID);
  Serial.print("ID: ");
  Serial.println(id, HEX);
  
  uint16_t stat = adc.readReg(ADS131_STATUS);
  Serial.print("Status: ");
  Serial.println(stat, HEX);

  uint16_t Mode = adc.readReg(ADS131_MODE);
  Serial.print("Mode: ");
  Serial.println(Mode, HEX);

  Serial.println("Starting in 3...");
  delay(1000);
  Serial.println("2...");
  delay(1000);
  Serial.println("1...");
  delay(1000);
  
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, LOW);
  delay(1);
    digitalWrite(RESET, HIGH);

}

char outputarr[64];

void loop() {
 if(digitalRead(DRDY_PIN)) {

    //Serial.println(adc.readChannelSingle(0)); //Just read the first one
    int32_t channelArr[8];
    adc.readAllChannels(channelArr);
    sprintf(outputarr, "%d|%d|%d|%d|%d|%d|%d|%d\r\n",
              channelArr[0], 
              channelArr[1], 
              channelArr[2],
              channelArr[3],
              channelArr[4],
              channelArr[5],
              channelArr[6],
              channelArr[7]);
    
  
    Serial.print("Channel Readings: ");
    Serial.print(outputarr);
  }
  else {
  }
}
