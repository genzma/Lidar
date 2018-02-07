#include <SPI.h>
#include "Tdc7200.h"

// Messparameter definieren
const int  clockForSPI  = 20000000;
const float clockPeriod = 71428.57143; //Periodendauer des externen Oszillators in ps
const byte calPeriods   = 10;
const byte measurementMode = 1;

//Interrupt Variable definieren

volatile byte state = LOW;

// Zählervariablen definieren

int gemessenePixel = 0;
int messungenProPixel = 0;
int nanCounter = 0;
int interruptProPixel = 0;
float rndTOF = 0;

// Variablen für Zwischenergebnisse definieren

float calCount = 0;
float normLSB = 0;
float TOF = 0;

// Pin Definition

const byte enablePin = 3;       //TDC_01
const byte startPin = 4;        //TDC_03
const byte stopPin = 7;         //TDC_04
const byte triggerPin = 8;      //TDC_02
const byte slaveSelectPin = 10; //TDC_11
const byte interruptPin = 2;    // Pin 2 und 3 auf UNO für Interrupts

// Erzeugung Instanz der Klasse Tdc7200

Tdc7200 chip1(clockForSPI, slaveSelectPin);

void setup() {

  noInterrupts();

  //Initialize SPI
  SPI.begin();

  //Initialize Serial
  Serial.begin(115200);

  //Initialize Outputs
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
  pinMode(startPin, OUTPUT);
  digitalWrite(startPin, LOW);
  pinMode(stopPin, OUTPUT);
  digitalWrite(stopPin, LOW);
  pinMode(slaveSelectPin, OUTPUT);
  digitalWrite(slaveSelectPin, HIGH);
  //Initialize INPUTS
  pinMode(triggerPin, INPUT);

  //Initialisiere Interrupt-Pin
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), itrDmdTrigger, RISING);

  enableTDC();
  
  interrupts();

}

void loop() {

  // setzte CONFIG Register des TDC
  chip1.configureRegisters(measurementMode);

  // warte auf TDC Trigger Signal
  waitUntilTdcReady();

  // werte möglichen Interrupt aus, falls ja: gib TOF Mittelwert aus
  checkIfNewPixel();

  // sende Start- (Stop-) Signal an TDC zur Zeitmessung
  noInterrupts();
  pulsePin(startPin); // bisher nur für Pin 4 und 7 definiert
  //pulsePin(stopPin);
  interrupts();

  delayMicroseconds(5);

  // lies Messergebnisse aus TDC Registern aus
  chip1.readMeasRegisters(measurementMode);

  // berechne TOF aus Messergebnissen
  computeTOF();

  // gib Zwischenwerte aus
  //printInterimResults();

  // Anzahl Messungen hochzählen
  messungenProPixel++;

  //TOF in Mittelwert rndTOF einpflegen
  computeAverage();
}

void itrDmdTrigger() {
  interruptProPixel++;
  //Serial.println("Interrupt");
  state = HIGH;
}

void enableTDC() {

  //STARTUP OF TDC7200
  digitalWrite(enablePin, LOW);
  delay(20); //Sensor needs 300microseconds to startup, 1.5ms recommended
  digitalWrite(enablePin, HIGH);
  delay(20);
}

void checkIfNewPixel() {

  if (state == HIGH) {

    //Messablauf für neuen Pixel einstellen
    if (rndTOF > 0) {
      Serial.print(rndTOF, 0);
    } else {
      Serial.print(0);
    }

    Serial.print(" ");
    Serial.print(messungenProPixel);
    Serial.print(" ");
    Serial.println(nanCounter);

    rndTOF = 0;
    messungenProPixel = 0;
    interruptProPixel = 0;
    nanCounter = 0;
    gemessenePixel++;

    state = LOW;
  }
}

void waitUntilTdcReady() {
  while (digitalRead(triggerPin) == HIGH) {
    delayMicroseconds(5);
    Serial.println("Warte bis TDC-Trigger");
  }
}

void pulsePin(byte pin) {
  
  cli () ;
  if (pin == 4) {
    PIND = 0x10 ;
    __asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
    __asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
    __asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
    PIND = 0x10 ;
  }

  if (pin == 7) {
    PIND = B10000000;
    __asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
    __asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
    __asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
    PIND = B10000000;
  }
  sei () ;
}

void computeTOF() {
  if (measurementMode == 1) {
    calCount = ((float)chip1.getCal2() - (float)chip1.getCal1()) / (calPeriods - 1);
    normLSB  = clockPeriod / calCount;
    TOF = normLSB * ((float)chip1.getTime1());
  }

  if (measurementMode == 2) {
    calCount = ((float)chip1.getCal2() - (float)chip1.getCal2()) / (calPeriods - 1);
    normLSB  = clockPeriod / calCount;
    TOF = normLSB * ((float)chip1.getTime1() - (float)chip1.getTime2()) + (float)chip1.getClockCount1() * clockPeriod ;
  }
}

void printInterimResults() {

  Serial.println("***");
  Serial.print("time1: "); Serial.println(chip1.getTime1());
  Serial.print("time2: "); Serial.println(chip1.getTime2());
  Serial.print("ClockCount1: "); Serial.println(chip1.getClockCount1());
  Serial.print("cal1: "); Serial.println(chip1.getCal1());
  Serial.print("cal2: "); Serial.println(chip1.getCal2());
  Serial.print("calCount: "); Serial.println(calCount, 6);
  Serial.print("normLSB: "); Serial.println(normLSB, 6);
  Serial.print("TOF: "); Serial.println(TOF);
  
}

void computeAverage() {
  if (!isnan(TOF)) {
    rndTOF = ((messungenProPixel - 1) * rndTOF + TOF) / messungenProPixel;
  } else {
    nanCounter++;
  }
}
