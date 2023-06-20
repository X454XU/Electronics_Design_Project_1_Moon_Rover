#define IR_RECEIVER_PIN A1

// Contains functions required to read alien's age

uint32_t detectPulse(uint16_t avg_voltage){
    uint32_t time;
    uint16_t start_time = millis();

    uint16_t current_voltage, previous_voltage;

    current_voltage = analogRead(IR_RECEIVER_PIN);

    do{
        previous_voltage = current_voltage;
        current_voltage = analogRead(IR_RECEIVER_PIN);
    } while((!(current_voltage > avg_voltage && previous_voltage <= avg_voltage)) || millis() - start_time < 4000);

    time = micros();

    return time;
}

uint16_t readAge(){
    const uint16_t calibration_time = 2000; // [ms]
    const uint16_t pulses = 2000; // number of pulses to be recorded 

    uint16_t age; // [years]

    uint32_t voltage_sum = 0;
    uint16_t avg_voltage, calibration_ctr = 0;
    uint32_t start_time, end_time;

    pinMode(IR_RECEIVER_PIN, INPUT);

    // Calibrate average voltage value
    start_time = millis();

    while(millis() - start_time < calibration_time){
        voltage_sum += analogRead(IR_RECEIVER_PIN);
        calibration_ctr++;
    }

    avg_voltage = ceil(voltage_sum / calibration_ctr);

    // Measure duration of a given number of pulses
    start_time = detectPulse(avg_voltage);

    for(int i = 0; i < pulses; i++) end_time = detectPulse(avg_voltage);

    // Calculate age
    age = round((end_time - start_time) / pulses / 10);

    return age;
}