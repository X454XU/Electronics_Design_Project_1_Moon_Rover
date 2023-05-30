// Contains functions required to measure polarity of alien's magnetic field

// Hall effect sensor pin
const int hallSensorPin = 2;

void setup() {
  // Set the hall sensor pin as input
  pinMode(hallSensorPin, INPUT);
  
  // Start serial communication
  Serial.begin(9600);
}

void loop() {
  // Read the state of the hall sensor pin
  int hallSensorState = digitalRead(hallSensorPin);

  // Check the polarity based on the sensor state
  if (hallSensorState == HIGH) {
    // Polarity is positive
    Serial.println("Positive polarity");
  } else {
    // Polarity is negative
    Serial.println("Negative polarity");
  }

  // Delay for a certain duration
  delay(500);
}
