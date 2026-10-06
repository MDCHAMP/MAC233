/**
 *  Motor driver bench test (Adafruit DRV8833, mini rig wiring)
 *  AIN1 -> D10, SLP -> +5V, GND -> GND, AIN2 -> GND, motor on AOUT1/AOUT2,
 *  9V on the snap.
 *
 *  Ramps the motor up, holds at full power, ramps down, then rests, forever.
 *  The built-in LED is on while the motor is being driven.
 **/

#define MOTOR_LOAD_PIN 10

const int ramp_step_delay = 20;  // ms per PWM step: ~5s each way
const int hold_time = 2000;      // ms at full power
const int rest_time = 2000;      // ms off between cycles

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_LOAD_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  analogWrite(MOTOR_LOAD_PIN, 0);
  Serial.println("Motor driver test");
}

void set_duty(int duty) {
  analogWrite(MOTOR_LOAD_PIN, duty);
  digitalWrite(LED_BUILTIN, duty > 0 ? HIGH : LOW);
  if (duty % 32 == 0 || duty == 255) {
    Serial.print("duty: ");
    Serial.println(duty);
  }
}

void loop() {
  for (int duty = 0; duty <= 255; duty++) {
    set_duty(duty);
    delay(ramp_step_delay);
  }
  delay(hold_time);
  for (int duty = 255; duty >= 0; duty--) {
    set_duty(duty);
    delay(ramp_step_delay);
  }
  delay(rest_time);
}
