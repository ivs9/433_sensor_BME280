//Weather sensor 433 MHz
//Author Sarbash
//Version 1.1

void MeasuredValues(void);// reading values

#include <MyBME280.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <RCSwitch.h>

#define BME280_I2cAdd 0x76 //I2C-Adresse
MyBME280 BME(BME280_I2cAdd);
#define TX_PIN 1
RCSwitch mySwitch = RCSwitch();
volatile byte countWdt = 1;
volatile byte countCykl = 0; // transmission cycle counter
volatile int VCC;// напряжение собственное
unsigned long Time;
float Temp, Press,Humidity;

// Watchdog Interrupt Service / Executed when Watchdog Timer expired
ISR (WDT_vect) {
}

void powerDown(int time) {

  if (time > 9 )
    time = 9; //Limit incoming amount to legal settings

  MCUSR &= ~(1 << WDRF); // Clear watch dog reset flag
  WDTCR |= (1 << WDCE) | (1 << WDE); // Set WD_change enable, set WD enable
  WDTCR = (time & 7) | (time & 8) << 2;
  WDTCR |= (1 << WDIE); // enable interrupt
  sleep_mode();
}

// put the system into sleep state
// system wakes up when watchdog times out
void  system_sleep () {
  ADCSRA &= ~(1 << ADEN); // turn off the analogue digital signal converter
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // hibernation is set here
  sleep_enable();
  sleep_mode();                        // The system sleeps here
  sleep_disable();                     // The system continues execution here when timed out
  ADCSRA |= 1<<ADEN; 
}


void setup() {
  countWdt = 1;
//  Serial.begin(115200);
  mySwitch.enableTransmit(TX_PIN);
  mySwitch.setProtocol(1);
  mySwitch.setPulseLength(350);
  mySwitch.setRepeatTransmit(4);
  if (BME.isReady())
  {
    BME.setWeatherMonitoring(); //Setze Parameter fur Wetterueberwachung
    BME.init();
    MeasuredValues();
  }
  else
  {
    mySwitch.send(010101, 32);// Send alarm signal if BME280 is not detected
    while (1); 
  }
  powerDown(WDTO_8S);
}

void loop() {
  if (countWdt == 0 ) {   // interrupt counter
    countWdt = 15L;       // Pause time sec = N * 8s
    BME.startSingleMeas();
    MeasuredValues();
    Time = millis();
    mySwitch.send(Temp, 32); // We send the temperature in each cycle
    if (countCykl == 0) {
      mySwitch.send(Humidity, 32);// together with the temperature at 0 cycle humidity
      }
    if (countCykl == 2) {
      mySwitch.send(Press, 32);  // together with the temperature on the 2nd cycle pressure
      }
    if (countCykl == 4) {
      mySwitch.send(readVcc(), 32);// 4 cycle battery voltage
      }
    countCykl++;
    if (countCykl == 5) {
      countCykl = 0;
    }
  } else {
    countWdt--;
  }
  system_sleep ();
}

void MeasuredValues()
{
  Temp = (BME.readTemp() * 100) + 71000000L;// add the number for subsequent decryption
  Press = BME.readPress()*75 + 72000000L;
  Humidity = BME.readHumidity()*10 + 73000000L;
}

long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  ADMUX = _BV(MUX3) | _BV(MUX2);

  delay(75); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high << 8) | low;

  result = 112530L / result + 74000000L; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}
