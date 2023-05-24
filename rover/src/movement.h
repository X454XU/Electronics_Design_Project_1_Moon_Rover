// Contains functions required to control movement of rover

const int motor1IN1 = 4;
const int motor1IN2 = 5;
const int motor1PWM = 6;
const int motor2IN1 = 7;
const int motor2IN2 = 8;
const int motor2PWM = 9;

// first character of string: chooses which motors to use
// second-fourth characters of string: distance travelled (correlates to number of turns)

void directionMotor(String inputString, char& motor, int &distance){
  inputString.trim();
  motor = inputString.charAt(0);
  distsance = inputString.substring(1).toInt();
}

void controlMotors(char motor, int distance) {
  // Control left motor
  if (motor == 'L') {
    motor1Speed = distance;
    motor2Speed = 0;
    turning = 0;
    Serial.print("Moving left motor by ");
    Serial.println(distance);
  }
  // Control right motor
  else if (motor == 'R') {
    motor1Speed = 0;
    motor2Speed = distance;
    turning = 0;
    Serial.print("Moving right motor by ");
    Serial.println(distance);
  }
  // Control both motors
  else if (motor == 'B') {
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

  motorSpeed1 += turning;
  motorSpeed 2 += turning;

  motorSpeed1 = constrain(motorSpeed1, 0, 255); 
  motorSpeed2 = constrain(motorSpeed2, 0, 255);

  analogWrite(motor1PWM, motorSpeed1);
  digitalWrite(motor1IN1, HIGH);
  digitalWrite(motor1IN2, LOW);

  analogWrite(motor2PWM, motorSpeed2);
  digitalWrite(motor2IN1, HIGH);
  digitalWrite(motor2IN2, LOW);
}

void setup() {
  // Set the motor control pins as outputs
  pinMode(motor1IN1, OUTPUT);
  pinMode(motor1IN2, OUTPUT);
  pinMode(motor1PWM, OUTPUT);
  pinMode(motor2IN1, OUTPUT);
  pinMode(motor2IN2, OUTPUT);
  pinMode(motor2PWM, OUTPUT);

  // Initialize the serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  if(Serial.available() > 0){
    String input = Serial.realStringUntil('\n');
    char motorSelection; 
    // L for left motor (only turns right)
    // R for right motor (only turns left)
    // B for both (straight)
    int distance; 
    // 3 digit string (e.g. 100) 
    parseInputString(input, motorSelection, distance);
    controlMotors(motorSelection, distance);
  }

  delay(100);
}
