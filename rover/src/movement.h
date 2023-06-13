void controlMotors(char motorBuffer[6]){
  const int motor1EN = 6;
  const int motor2EN = 7;

  const int motor1DIR = 9;
  const int motor2DIR = 10;

  digitalWrite(motor1EN, HIGH);
  digitalWrite(motor2EN, HIGH);

  int x1 = (int)motorBuffer[0]; 
  int x2 = (int)motorBuffer[1];
  int x3 = (int)motorBuffer[2];

  int y1 = (int)motorBuffer[3]; 
  int y2 = (int)motorBuffer[4];
  int y3 = (int)motorBuffer[5];

  int xpos = x1*100 + x2*10 + x3;
  int ypos = y1*100 + y2*10 + y3;

  int motor1Speed = map(ypos, 0, 1023, -255, 255);
  int motor2Speed = map(ypos, 0, 1023, -255, 255);
  // ypos only controls the speed of motor

  int turning = map(xpos, 0, 1023, -255, 255);
  // xpos only controls how much it turns

  motor1Speed += turning;
  motor2Speed -= turning; 

  motor1Speed = constrain(motor1Speed, -255, 255);
  motor2Speed = constrain(motor2Speed, -255, 255);

  if (ypos >= 0) {
  // Forward
    digitalWrite(motor1DIR, HIGH); // Set motor 1 direction
    digitalWrite(motor2DIR, HIGH); // Set motor 2 direction
  } else {
  // Reverse
    digitalWrite(motor1DIR, LOW); // Set motor 1 direction
    digitalWrite(motor2DIR, LOW); // Set motor 2 direction
  }

  // Set the motor speeds
  analogWrite(motor1DIR, abs(motor1Speed));
  analogWrite(motor2DIR, abs(motor2Speed));
}

void motorsetup() {
  // Set the motor control pins as outputs
  const int motor1EN = 6;
  const int motor2EN = 7;

  const int motor1DIR = 9;
  const int motor2DIR = 10;

  pinMode(motor1EN, OUTPUT);
  pinMode(motor2EN, OUTPUT);

  pinMode(motor1DIR, OUTPUT);
  pinMode(motor2DIR, OUTPUT);
}
