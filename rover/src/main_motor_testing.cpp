#include <SPI.h>
#include "movement.h"

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only.
  }

  motorsetup();

  // You can put additional commands in here
  char motorBuffer[6] = {'0','0','1','0','0','1'}; // Write input here
  controlMotors(motorBuffer);
}

void loop() {

}