#include <Arduino.h>
#include "Adafruit_ZeroTimer.h"

// bit is 1.67
// stop bit 1.57

#define PIN_TO_MEASURE A0

volatile uint16_t voltageReading = 0;
volatile bool voltageRead = false;

float freq; // [Hz]

Adafruit_ZeroTimer zerotimer = Adafruit_ZeroTimer(3);

void TC3_Handler() {
  Adafruit_ZeroTimer::timerHandler(3);
}

// the timer callback
void TimerCallback0(void)
{
  voltageReading = analogRead(PIN_TO_MEASURE);
  voltageRead = true;
  //Serial.println("voltage read:");
  //Serial.println(voltageRead);
  //Serial.println(voltageReading);
}

void timerSetup(){
  // Set up the flexible divider/compare
  uint16_t divider  = 1;
  uint16_t compare = 0;
  tc_clock_prescaler prescaler = TC_CLOCK_PRESCALER_DIV1;

  if ((freq < 24000000) && (freq > 800)) {
    divider = 1;
    prescaler = TC_CLOCK_PRESCALER_DIV1;
    compare = 48000000/freq;
  } else if (freq > 400) {
    divider = 2;
    prescaler = TC_CLOCK_PRESCALER_DIV2;
    compare = (48000000/2)/freq;
  } else if (freq > 200) {
    divider = 4;
    prescaler = TC_CLOCK_PRESCALER_DIV4;
    compare = (48000000/4)/freq;
  } else if (freq > 100) {
    divider = 8;
    prescaler = TC_CLOCK_PRESCALER_DIV8;
    compare = (48000000/8)/freq;
  } else if (freq > 50) {
    divider = 16;
    prescaler = TC_CLOCK_PRESCALER_DIV16;
    compare = (48000000/16)/freq;
  } else if (freq > 12) {
    divider = 64;
    prescaler = TC_CLOCK_PRESCALER_DIV64;
    compare = (48000000/64)/freq;
  } else if (freq > 3) {
    divider = 256;
    prescaler = TC_CLOCK_PRESCALER_DIV256;
    compare = (48000000/256)/freq;
  } else if (freq >= 0.75) {
    divider = 1024;
    prescaler = TC_CLOCK_PRESCALER_DIV1024;
    compare = (48000000/1024)/freq;
  } else {
    //Serial.println("Invalid frequency");
    while (1) delay(10);
  }
  //Serial.print("Divider:"); Serial.println(divider);
  //Serial.print("Compare:"); Serial.println(compare);
  //Serial.print("Final freq:"); Serial.println((int)(48000000/compare));

  zerotimer.enable(false);
  zerotimer.configure(prescaler,       // prescaler
          TC_COUNTER_SIZE_16BIT,       // bit width of timer/counter
          TC_WAVE_GENERATION_MATCH_PWM // frequency or PWM mode
          );

  zerotimer.setCompare(0, compare);
  zerotimer.setCallback(true, TC_CALLBACK_CC_CHANNEL0, TimerCallback0);
  zerotimer.enable(true);
}

String decodeName(){
  // Timer setup
  freq = 600.0; // [Hz]
  timerSetup();

  Serial.println("Reading name...");

  String fullName;  // Variable to store the full name
  String binaryValue;
  uint16_t decimalValue;
  uint32_t start_time;
  uint8_t bit_counter;

  // Detect idle segment
  start_time = micros();
  while(micros() - start_time < 20000){
    if(voltageReading < 200) start_time = micros();
    //Serial.println(voltageReading);
  }
  while(voltageReading > 200){
    ;
  }
  // Change timer frequency
  //float freq = 600.0; // [Hz]
  //timerSetup(freq);
  // Reset timer
  //Serial.println("Reading bits...");
  //zerotimer.enable(false);
  //zerotimer.enable(true);
  voltageRead = false;
  binaryValue = "0";
  bit_counter = 1;

  start_time = micros();
  while(micros() - start_time < 20000){
    //Serial.println("time:");
    //Serial.println(micros() - start_time);
    //Serial.println("bit counter:");
    //Serial.println(bit_counter);
    while(bit_counter < 10){
      if(voltageRead){
        voltageRead = false;
        bit_counter++;
        //Serial.println(voltageReading);
        if(voltageReading > 500){
          binaryValue += "1";
        }
        else{
          binaryValue += "0";
          start_time = micros();
        }
      }
    }
    // Remove start bit (first bit) and stop bit (last bit)
    binaryValue = binaryValue.substring(1, 10 - 2);
    Serial.println(binaryValue);

    // Convert binary/ASCII value to alphabet character
    decimalValue = 0;
    for (int i=7; i>0; i--)  // for every character in the string  strlen(s) returns the length of a char array
    {
      decimalValue *= 2; // double the result so far
      if (binaryValue[i] == '1') decimalValue++;  //add 1 if needed
    }
    //int decimalValue = atoi(binaryValue, nullptr, 2);
    char alphabetChar = 'A' + decimalValue - 2;
    fullName += alphabetChar;

    binaryValue = "";
    bit_counter = 0;
  }

  Serial.println(fullName);
  return fullName;
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
}

void loop() {
  decodeName();
  //Serial.println(decodeName());
  //timerSetup();
  delay(1000);
}
