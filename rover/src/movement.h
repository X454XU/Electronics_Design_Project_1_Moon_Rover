// Contains functions required to control movement of rover

const int motor1IN1 = 4;
const int motor1IN2 = 5;
const int motor1PWM = 6;
const int motor2IN1 = 7;
const int motor2IN2 = 8;
const int motor2PWM = 9;

// first character of string: chooses which motors to use
// second-fourth characters of string: distance travelled (correlates to number of turns)

void controlMotors(char direction, char[3] motorBuffer) {
  // TODO: convert motorBuffer to int distance
  uint8_t distance;
  
  int firstdigit = (int)motorBuffer[0]; 
  int seconddigit = (int)motorBuffer[1];
  int thirddigit = (int)motorBuffer[2];

  distance = firstdigit*100 + seconddigit*10 + thirddigit;

  int motor1Speed, motor2Speed, turning;

  // Control left motor
  if (direction == 'L') {
    motor1Speed = distance;
    motor2Speed = 0;
    turning = 0;
    Serial.print("Moving left motor by ");
    Serial.println(distance);
  }
  // Control right motor
  else if (direction == 'R') {
    motor1Speed = 0;
    motor2Speed = distance;
    turning = 0;
    Serial.print("Moving right motor by ");
    Serial.println(distance);
  }
  // Control both motors
  else if (direction == 'B') {
    motor1Speed = distance;
    motor2Speed = distance;
    Serial.print("Moving both motors by ");
    Serial.println(distance);
  }
  // Invalid motor selection
  else {
    motor1Speed = 0;
    motor2Speed = 0;
    turning = 0;
    Serial.println("Invalid motor selection");
  }

  motor1Speed += turning;
  motor2Speed += turning;

  motor1Speed = constrain(motor1Speed, 0, 255); 
  motor2Speed = constrain(motor2Speed, 0, 255);

  analogWrite(motor1PWM, motor1Speed);
  digitalWrite(motor1IN1, HIGH);
  digitalWrite(motor1IN2, LOW);

  analogWrite(motor2PWM, motor2Speed);
  digitalWrite(motor2IN1, HIGH);
  digitalWrite(motor2IN2, LOW);

  // Initialize the serial communication for debugging
  Serial.begin(9600);
  delay(100);
}

void motorsetup() {
  // Set the motor control pins as outputs
  pinMode(motor1IN1, OUTPUT);
  pinMode(motor1IN2, OUTPUT);
  pinMode(motor1PWM, OUTPUT);
  pinMode(motor2IN1, OUTPUT);
  pinMode(motor2IN2, OUTPUT);
  pinMode(motor2PWM, OUTPUT);
}
