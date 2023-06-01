// Contains functions required to decode alien's name

#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1024

#define RADIO_RECEIVER_PIN A0

const std::vector<char> alphabetLookupTable = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

void setTimerFrequency(uint32_t sampling_frequency) {
  uint16_t compareValue = (CPU_HZ / (TIMER_PRESCALER_DIV * sampling_frequency)) - 1;
  TcCount16* TC = (TcCount16*) TC3;
  // Make sure the count is in a proportional position to where it was
  // to prevent any jitter or disconnect when changing the compare value.
  TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
  TC->CC[0].reg = compareValue;
  Serial.println(TC->COUNT.reg);
  Serial.println(TC->CC[0].reg);
  while (TC->STATUS.bit.SYNCBUSY == 1);
}

void startTimer(int frequencyHz) {
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3);
  while (GCLK->STATUS.bit.SYNCBUSY == 1); // wait for sync

  TcCount16* TC = (TcCount16*) TC3;

  TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  // Use the 16-bit timer
  TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  // Use match mode so that the timer counter resets when the count matches the compare register
  TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  // Set prescaler to 1024
  TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  setTimerFrequency(frequencyHz);

  // Enable the compare interrupt
  TC->INTENSET.reg = 0;
  TC->INTENSET.bit.MC0 = 1;

  NVIC_EnableIRQ(TC3_IRQn);

  TC->CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
}

uint16_t TC3_Handler() {
  TcCount16* TC = (TcCount16*) TC3;
  // If this interrupt is due to the compare register matching the timer count
  // we toggle the LED.
  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;
    // Write callback here!!!
    return analogRead(RADIO_RECEIVER_PIN);
  }
}

string decodeName(){
    const uint32_t signal_frequency = 100000; // [Hz]
    const uint32_t sampling_frequency = 200000; // [Hz]

    // Sample incoming signal
    pinMode(RADIO_RECEIVER_PIN, INPUT);

    startTimer(sampling_frequency);

    // Convert recorded signal into frequency domain

    // Decode recorded signal   
  
  std::string decodedcharacter; 
    bool isStartBitDetected = false;
    std::string binaryValue;

    while (true) {
        uint16_t analogValue = TC3_Handler(); //I'm assuming this is the analog value from the RADIO RECIEVER PIN

        // Detect start bit (0)
        if (isStartBitDetected == false) {
            if (analogValue == 0) {
                isStartBitDetected = true;
                binaryValue.clear(); //erases any bits that have been added before and will start adding bits until 10 bits are present
            }
            continue;
        }

        // Collect 10-bit binary/ASCII value
        binaryValue += std::to_string(analogValue);

        // Check if full 10 bits have been received
        if (binaryValue.length() >= 10) {
            // Remove start bit (first bit) and stop bit (last bit)
            binaryValue = binaryValue.substr(1, 10 - 2);
            
            // Convert binary/ASCII value to alphabet character
            int decimalValue = std::stoi(binaryValue, nullptr, 2);
            if (decimalValue >= 2 && decimalValue <= 27) {
                char alphabetChar = alphabetLookupTable[decimalValue - 2];
                decodedcharacter += alphabetChar;
            }
            
            // Reset for the next binary value
            isStartBitDetected = false;
        }

        // Detect stop bit (1)
        if (analogValue == 1 && !isStartBitDetected) {
            break;  // End of signal
        }
    }
    Serial.printIn(decodedcharacter);
    return decodedcharacter;  
  

    //return name;
}
