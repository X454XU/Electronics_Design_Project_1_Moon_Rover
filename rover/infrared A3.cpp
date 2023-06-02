const int sensorPin = A3; // connect sensor to analog input 0

double timer = 0;
double Prev_Timer = 0;
double period = 0;
double Current_Value = 0;
double Prev_Value = 0;
double average = 0;

void setup()
{
 pinMode(30, OUTPUT);
 pinMode(sensorPin, INPUT);
 Serial.begin(1000000);
}
void loop()
{
 for(int i = 0; i < 100; i++){
  Current_Value = analogRead(A3);
  if(Current_Value < 100 && Prev_Value > 100){
    timer = millis();
    period = timer - Prev_Timer;
    pretimer = timer;
    average = average + period;
  }
  else{
    i--;
  }
  Prev_Value = Current_Value;
 }
  average = average/100;
  Serial.print("The time interval is ");
  Serial.print(average);
  Serial.println(" s");
}
