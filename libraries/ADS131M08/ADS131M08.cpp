#include "ADS131M08.h"

ADS131M08::ADS131M08(int cs, int xtal, int drdy, int clk) {

    CS = cs; XTAL = xtal; DRDY = drdy; //You don't have to use DRDY, can also read off the ADS131_STATUS register.
    SpiClk = clk;
}

void ADS131M08::init(int clkin) {

    Serial.println("Setting pin configuration");
        
    pinMode(CS, OUTPUT); digitalWrite(CS, HIGH);
    pinMode(DRDY, INPUT_PULLUP);
    
    spi->begin();

    Serial.println("Setting oscillator");

    ledcSetup(2, clkin, 8);
    ledcAttachPin(XTAL, 2); //Simulate 8.192Mhz crystal with PWM. This needs to be started as soon as possible after powering on
    ledcWrite(2,127);

    Serial.println("SPI Ready...");

}

void ADS131M08::readChannels(int8_t * channelArrPtr, int8_t channelArrLen, int32_t * outputArrPtr) {
    
    uint32_t rawDataArr[10];

    // Get data
    spiCommFrame(&rawDataArr[0]);
    
    // Save the decoded data for each of the channels
    for (int8_t i = 0; i<channelArrLen; i++) {
        *outputArrPtr = twoCompDeco(rawDataArr[*channelArrPtr+1]);
        outputArrPtr++;
        channelArrPtr++;
    }
}

int32_t ADS131M08::readChannelSingle(int8_t channel) {
    /* Returns raw value from a single channel
        channel input from 0-7
    */
    
    int32_t outputArr[1];
    int8_t channelArr[1] = {channel};

    readChannels(&channelArr[0], 1, &outputArr[0]);

    return outputArr[0];
}

bool ADS131M08::globalChop(bool enabled, uint8_t log2delay) {
    /* Function to configure global chop mode for the ADS131M04.

        INPUTS:
        enabled - Whether to enable global-chop mode.
        log2delay   - Base 2 log of the desired delay in modulator clocks periods
        before measurment begins
        Possible values are between and including 1 and 16, to give delays
        between 2 and 65536 clock periods respectively
        For more information, refer to the datasheet.

        Returns true if settings were written succesfully.
    */

    uint8_t delayRegData = log2delay - 1;

    // Get current settings for current detect mode from the CFG register
    uint16_t currentDetSett = (readReg(ADS131_CFG) << 8) >>8;
    
    uint16_t newRegData = (delayRegData << 12) + (enabled << 8) + currentDetSett;

    return writeReg(ADS131_CFG, newRegData);
}

bool ADS131M08::writeReg(uint8_t reg, uint16_t data) {
    /* Writes the content of data to the register reg
        Returns true if successful
    */
    
    uint8_t commandPref = 0x06;

    // Make command word using syntax found in data sheet
    uint16_t commandWord = (commandPref<<12) + (reg<<7);

    digitalWrite(CS, LOW);
    spi->beginTransaction(SPISettings(SpiClk, MSBFIRST, SPI_MODE1));

    spiTransferWord(commandWord);
    
    spiTransferWord(data);

    // Send 4 empty words
 
    for (uint8_t i=0; i<4; i++) {
        spiTransferWord();
    }

    spi->endTransaction();
    digitalWrite(CS, HIGH);

    // Get response
    uint32_t responseArr[10];
    spiCommFrame(&responseArr[0]);

    if ( ( (0x04<<12) + (reg<<7) ) == responseArr[0]) {
        return true;
    } else {
        return false;
    }
}

uint16_t ADS131M08::readReg(uint8_t reg) {
    /* Reads the content of single register found at address reg
        Returns register value
    */
    
    uint8_t commandPref = 0x0A;

    // Make command word using syntax found in data sheet
    uint16_t commandWord = (commandPref << 12) + (reg << 7);

    uint32_t responseArr[10];

    // Use first frame to send command
    spiCommFrame(&responseArr[0], commandWord);

    // Read response
    spiCommFrame(&responseArr[0]);

    return responseArr[0] >> 16;
}

uint32_t ADS131M08::spiTransferWord(uint16_t inputData) {
    /* Transfer a 24 bit word
        Data returned is MSB aligned
    */ 

    uint32_t data = spi->transfer(inputData >> 8);
    data <<= 8;
    data |= spi->transfer((inputData<<8) >> 8);
    data <<= 8;
    data |= spi->transfer(0x00);

    return data << 8;
}

void ADS131M08::spiCommFrame(uint32_t * outPtr, uint16_t command) {
    // Saves all the data of a communication frame to an array with pointer outPtr

    digitalWrite(CS, LOW);

    spi->beginTransaction(SPISettings(SpiClk, MSBFIRST, SPI_MODE1));

    // Send the command in the first word
    *outPtr = spiTransferWord(command);

    // For the next 8 words, just read the data
    for (uint8_t i=1; i < 9; i++) {
        outPtr++;
        *outPtr = spiTransferWord() >> 8;
    }

    // Save CRC bits
    outPtr++;
    *outPtr = spiTransferWord();

    spi->endTransaction();

    digitalWrite(CS, HIGH);
}

int32_t ADS131M08::twoCompDeco(uint32_t data) {
    // Take the two's complement of the data

    data <<= 8;
    int32_t dataInt = (int)data;

    return dataInt/pow(2,8);
}

bool ADS131M08::setGain(int gain) { // apply gain to all channels (1 to 128, base 2 (1,2,4,8,16,32,64,128))
    uint16_t writegain = 0;
    if(gain == 1 ) {
        writegain = 0b0000000000000000;
    }
    else if (gain == 2) {
        writegain = 0b0001000100010001;
    }
    else if (gain == 4) {
        writegain = 0b0010001000100010;
    }
    else if (gain == 8) { 
        writegain = 0b0011001100110011;
    }
    else if (gain == 16) { 
        writegain = 0b0100010001000100;
    }
    else if (gain == 32) {
        writegain = 0b0101010101010101;
    }
    else if (gain == 64) {
        writegain = 0b0110011001100110;
    }
    else if (gain == 128) {
        writegain = 0b0111011101110111;
    }
    else {
        return false;
    }
    writeReg(ADS131_GAIN1, writegain);
    writeReg(ADS131_GAIN2, writegain);

    return true;
}
