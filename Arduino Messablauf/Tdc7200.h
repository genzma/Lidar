#ifndef TDC7200_H
#define TDC7200_H

// parts of this code taken from https://github.com/TAPR/TICC/tree/master/TICC

// TDC7200 register addresses
const int CONFIG1 =    0x00;           // default 0x00
const int CONFIG2 =   0x01;           // default 0x40
const int INT_STATUS =    0x02;           // default 0x00
const int INT_MASK =    0x03;           // default 0x07
const int COARSE_CNTR_OVF_H = 0x04;         // default 0xff
const int COARSE_CNTR_OVF_L = 0x05;         // default 0xff
const int CLOCK_CNTR_OVF_H =  0x06;         // default 0xff
const int CLOCK_CNTR_OVF_L =  0x07;         // default 0xff
const int CLOCK_CNTR_STOP_MASK_H = 0x08;  // default 0x00
const int CLOCK_CNTR_STOP_MASK_L = 0x09;  // default 0x00
// gap from 0x0A thru 0x0F...
const int TIME1 =   0x10;           // default 0x00_0000
const int CLOCK_COUNT1 =  0x11;           // default 0x00_0000
const int TIME2 =   0x12;           // default 0x00_0000
const int CLOCK_COUNT2 =  0x13;           // default 0x00_0000
const int TIME3 =   0x14;           // default 0x00_0000
const int CLOCK_COUNT3 =  0x15;           // default 0x00_0000
const int TIME4 =   0x16;           // default 0x00_0000
const int CLOCK_COUNT4 =  0x17;           // default 0x00_0000
const int TIME5 =   0x18;           // default 0x00_0000
const int CLOCK_COUNT5 =  0x19;           // default 0x00_0000
const int TIME6 =   0x1A;           // default 0x00_0000
const int CALIBRATION1 =  0x1B;           // default 0x00_0000
const int CALIBRATION2 =  0x1C;           // default 0x00_0000

/* Chip properties:
   Most Significant Bit is clocked first (MSBFIRST)
   clock is low when idle
   data is clocked on the rising edge of the clock (seems to be SPI_MODE0)
   max clock speed: 20 MHz
*/

class Tdc7200 {

  private:

    uint32_t SPIfrequency;
    int slaveSelect;

    uint32_t time1 = 0;
    uint32_t time2 = 0;
    uint32_t clockCount1 = 0;
    uint32_t cal1 = 0;
    uint32_t cal2 = 0;

  public:

    Tdc7200(uint32_t spi, int slave) {
      SPIfrequency = spi;
      slaveSelect = slave;
    }

    uint32_t getTime1() {
      return time1;
    }

    uint32_t getTime2() {
      return time2;
    }

    uint32_t getClockCount1() {
      return clockCount1;
    }

    uint32_t getCal1() {
      return cal1;
    }

    uint32_t getCal2() {
      return cal2;
    }

    void configureRegisters(byte measurementMode) {

      // stetze CONFIG1 Register
      // Bit [2,1] für Messurement mode: 00 = mode1 ## 01 = mode2
      
      if (measurementMode == 1) {
        writeReg(CONFIG1, B00100001);
      } else {
        writeReg(CONFIG1, B00100011);
      }

      //setzte CONFIG2 Register B01000xxx, letzte 3 Bit entsprechen Anzahl Stops je Messung
      writeReg(CONFIG2, B01000000);

      /*READ IF CONFIG1/2 CORRECT
        Serial.print("CONFIG1: "); Serial.println(readReg8(CONFIG1), BIN);
        Serial.print("CONFIG2: "); Serial.println(readReg8(CONFIG2), BIN);
      */
    }

    void readMeasRegisters(byte measurementMode) {

      if (measurementMode == 1) {
        // Register auslesen
        time1 = readReg24(TIME1);
        cal1 = readReg24(CALIBRATION1);
        cal2 = readReg24(CALIBRATION2);
      }

      if (measurementMode == 2) {
        // Register auslesen
        time1 = readReg24(TIME1);
        time2 = readReg24(TIME2);
        clockCount1 = readReg24(CLOCK_COUNT1);
        cal1 = readReg24(CALIBRATION1);
        cal2 = readReg24(CALIBRATION2);
      }
    }

  private:
    
    //READ 8bit Register
    byte readReg8(byte address) {
      byte inByte = 0;
      SPI.beginTransaction(SPISettings(SPIfrequency, MSBFIRST, SPI_MODE0));
      // take the chip select low to select the device:
      digitalWrite(slaveSelect, LOW);
      SPI.transfer(address & 0x1f);
      inByte = SPI.transfer(address);
      delayMicroseconds(5);
      digitalWrite(slaveSelect, HIGH);
      SPI.endTransaction();
      return inByte;
    }

    //READ 24bit Register
    uint32_t readReg24(byte address) {
      uint32_t long value = 0;
      // CSB needs to be toggled between 24-bit register reads
      SPI.beginTransaction(SPISettings(SPIfrequency, MSBFIRST, SPI_MODE0));
      digitalWrite(slaveSelect, LOW);
      SPI.transfer(address & 0x1f);
      uint16_t msb = SPI.transfer(address);
      uint16_t mid = SPI.transfer(address);
      uint16_t lsb = SPI.transfer(address);
      value = (msb << 16) + (mid << 8) + lsb;
      delayMicroseconds(5);
      digitalWrite(slaveSelect, HIGH);
      SPI.endTransaction();
      delayMicroseconds(5);
      return value;
    }

    //WRITE REGISTER
    void writeReg(byte address, byte value) {
      uint16_t test = 0;
      // take the chip select low to select the device:
      SPI.beginTransaction(SPISettings(SPIfrequency, MSBFIRST, SPI_MODE0));
      digitalWrite(slaveSelect, LOW);
      // Force Address bit 6 to one for a write
      SPI.transfer16((address | 0x40) << 8 | value);
      delayMicroseconds(5);
      digitalWrite(slaveSelect, HIGH);
      SPI.endTransaction();
    }
};

#endif  /* TDC7200_H */
