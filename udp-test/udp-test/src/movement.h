void controlMotors(char motorBuffer[6]){
    const int motor1EN = 0;
    const int motor2EN = 1;

    const int motor1DIR = 2;
    const int motor2DIR = 3;

    digitalWrite(motor1EN, HIGH);
    digitalWrite(motor2EN, HIGH);

    int y1 = (int)motorBuffer[0] - 48; 
    int y2 = (int)motorBuffer[1] - 48;
    int y3 = (int)motorBuffer[2] - 48;

    int x1 = (int)motorBuffer[3] - 48; 
    int x2 = (int)motorBuffer[4] - 48;
    int x3 = (int)motorBuffer[5] - 48;

    int xpos = x1*100 + x2*10 + x3;
    Serial.println("xpos:");
    Serial.println(xpos);

    int ypos = y1*100 + y2*10 + y3;
    Serial.println("ypos:");
    Serial.println(ypos);

    int motor1Speed = abs(ypos);
    int motor2Speed = abs(ypos);
    // ypos only controls the speed of motor

    int turning = xpos;
    // xpos only controls how much it turns

    if(xpos > 500 && ypos > 500){
      digitalWrite(motor1DIR, HIGH);
      digitalWrite(motor2DIR, HIGH); 

      motor1Speed = map(motor1Speed, 500, 999, 130, 255);
      motor2Speed = map(motor2Speed, 500, 999, 130, 255);
      turning = map(turning, 500, 999, 130, 255);

      motor1Speed = motor1Speed/3 + turning; 
      motor2Speed = motor2Speed/3 + turning/2;
    }
    else if(xpos < 500 && ypos > 500){
      digitalWrite(motor1DIR, HIGH);
      digitalWrite(motor2DIR, HIGH); 

      motor1Speed = motor1Speed/3 + (999-turning)/2; 
      motor2Speed = motor2Speed/3 + turning; 
    }
    else if(xpos > 500 && ypos < 500){
      digitalWrite(motor1DIR, LOW);
      digitalWrite(motor2DIR, LOW); 

      motor1Speed = motor1Speed/3 + turning; 
      motor2Speed = motor2Speed/3 + turning/2; 
    }

    else if(xpos < 500 && ypos < 500){
    // both motors reverse
      digitalWrite(motor1DIR, LOW);
      digitalWrite(motor2DIR, LOW); 

      motor1Speed = motor1Speed/3 + turning; 
      motor2Speed = motor2Speed/3 + turning/2; 
    }
    else{
      motor1Speed = 0;
      motor2Speed = 0;
    }

    if(motor1Speed > 0 && motor2Speed > 0){
      motor1Speed = map(motor1Speed, 0, 1332, 130, 255);
      motor2Speed= map(motor2Speed, 0, 1332, 130, 255);
    }

    // motor1Speed = map(motor1Speed, 0, 1498.5, 0, 255);
    // motor2Speed = map(motor2Speed, 0, 1498.5, 0, 255);
    // depends on joystick inputs

    motor1Speed = constrain(motor1Speed, 0, 255);
    motor2Speed = constrain(motor2Speed, 0, 255);

    Serial.println("motor1Speed:");
    Serial.println(motor1Speed);

    Serial.println("motor2Speed");
    Serial.println(motor2Speed);

    // Set the motor speeds
    analogWrite(motor1EN, abs(motor1Speed));
    analogWrite(motor2EN, abs(motor2Speed));
}

void motorSetup() {
  // Set the motor control pins as outputs
   const int motor1EN = 5;
   const int motor2EN = 6;

   const int motor1DIR = 9;
   const int motor2DIR = 10;
  pinMode(motor1EN, OUTPUT);
  pinMode(motor2EN, OUTPUT);

  pinMode(motor1DIR, OUTPUT);
  pinMode(motor2DIR, OUTPUT);
}

