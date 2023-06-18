// Contains functions required to decode alien's name
#include <string>

#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1024

#define RADIO_RECEIVER_PIN A0


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

uint16_t Timercount_Handler() {
    TcCount16* TC = (TcCount16*) TC3;
    // Read sensor value
    if(TC->INTFLAG.bit.MC0 == 1){
      TC->INTFLAG.bit.MC0 = 1;
      uint16_t analogValue = analogRead(RADIO_RECEIVER_PIN);
      uint16_t THRESHOLD = 10;
    // Determine whether to return '0' or '1' based on the current analog value
      if (analogValue > THRESHOLD) {
          return 1;
    } else {
        return 0;
    }
    }
    return 0;
    
}


std::string decodeName() {
    const uint32_t signal_frequency = 100000; // [Hz]
    const uint32_t sampling_frequency = 200000; // [Hz]

    // Sample incoming signal
    pinMode(RADIO_RECEIVER_PIN, INPUT);
    startTimer(sampling_frequency);

  
    bool isStartBitDetected = false;
    std::string binaryValue;

    int repetitionCount = 0;
    std::string lastRepetition;

    while (true) {
        uint16_t analogValue = Timercount_Handler();

        // Detect start bit (0)
        if (isStartBitDetected == false) {
            if (analogValue == 0) {
                isStartBitDetected = true;
                binaryValue.clear();
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
                char alphabetChar = 'A' + decimalValue - 2;
                fullName += alphabetChar;
            }

            // Reset for the next binary value
            isStartBitDetected = false;
        }

        // Detect stop bit (1)
        if (analogValue == 1 && !isStartBitDetected) {
            if (fullName == lastRepetition) {
                repetitionCount++;
                if (repetitionCount == 2) {
                    Serial.println("Name: ");
                    Serial.println(fullName.c_str());
                    fullName.clear();
                    repetitionCount = 0;
                    lastRepetition.clear();
                }
            } else {
                lastRepetition = fullName;
            }
        }
    }

    return fullName;
}





