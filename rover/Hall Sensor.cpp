void setup() {
  Serial.begin(9600);  // Initialize serial communication
}

void loop() {
  Serial.println(polarity());
  Serial.print(" ");
  Serial.print("Strength:");
  Serial.print(analogRead(A0));  //connect it to pin A0 
  Serial.print(" ");
  Serial.print("Poplarity:");
  Serial.print(" ");
  delay(100);
}

String polarity(){
  double sensorValue= analogRead(A0);
  String polarity="None";
  if (sensorValue>= 790 ){  //Threshold voltage, can be changed

    polarity="N";
  }
  else if(sensorValue<= 800){  //Threshold voltage, can be changed
    polarity="S";
  }
    return polarity;
}