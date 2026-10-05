/**
 *  Original Author: D S Brennan (github.com/dsbrennan)
 *  Created: 29/08/2023
 *  Updated: 10/08/2026 (M D Champneys)
 *
 *  Copyright 2023 - 2026, MIT Licence
 **/

//////////////////////////////////////////////////////////////
//              DO NOT CHANGE THESE VALUES                  //
//////////////////////////////////////////////////////////////
#include <Servo.h>                                          //
// pins: do not change these values                         //
#define SYSTEM_READY_LED_PIN 4                              //
#define CRANK_ACTIVITY_LED_PIN 5                            //
#define MOTOR_ACTIVITY_LED_PIN 6                            //
#define WHEEL_ACTIVITY_LED_PIN 7                            //
#define MOTOR_LOAD_PIN 10                                   //
#define CRANK_PIN 2                                         //
#define WHEEL_PIN 3                                         //
#define ESC_PIN 9                                           //
// safety: do not change these values                       //
#define CRANK_PASS_ACTIVITY_DELAY 500                       //
#define CRANK_PASS_MAXIMUM_DELAY 3000                       //
#define WHEEL_PASS_ACTIVITY_DELAY 500                       //
#define WHEEL_PASS_MINIMUM_DELAY 200                        //
#define WHEEL_PASS_MAXIMUM_DELAY 3000                       //
#define WHEEL_MAXIMUM_SPEED 25.0                            //
// bike                                                     //
#define WHEEL_CIRCUMFERENCE 2.0734                          //
const float pedal_threshold_rpm = 20;                       //
// debug                                                    //
#define MESSAGE_MAXIMUM_INTERVAL 1000                       //
// big-rig esc parameters                                   //
const int esc_neutral = 1100;                               //
const int esc_full_power = 2000;                            //
Servo esc;                                                  //
// count variables                                          //
unsigned volatile int crank_rotations_counter = 0;          //
unsigned volatile int wheel_rotations_counter = 0;          //
unsigned volatile long crank_interrupt_current_time = 0;    //
unsigned volatile long crank_interrupt_previous_time = 0;   //
unsigned volatile long wheel_interrupt_current_time = 0;    //
unsigned volatile long wheel_interrupt_previous_time = 0;   //
unsigned long message_previous_time = 0;                    //
// output variables                                         //
// motor_power is remembered between loops so it ramps      //
float motor_power = 0;                                      //
const int startup_time = 3000;                              //
//////////////////////////////////////////////////////////////

/*
  System Setup
  ---------------------
*/
void setup() {
  // serial
  Serial.begin(9600);
  // status LEDs
  pinMode(SYSTEM_READY_LED_PIN, OUTPUT);
  digitalWrite(SYSTEM_READY_LED_PIN, LOW);
  pinMode(CRANK_ACTIVITY_LED_PIN, OUTPUT);
  digitalWrite(CRANK_ACTIVITY_LED_PIN, LOW);
  pinMode(MOTOR_ACTIVITY_LED_PIN, OUTPUT);
  digitalWrite(MOTOR_ACTIVITY_LED_PIN, LOW);
  pinMode(WHEEL_ACTIVITY_LED_PIN, OUTPUT);
  digitalWrite(WHEEL_ACTIVITY_LED_PIN, LOW);
  // motor driver
  pinMode(MOTOR_LOAD_PIN, OUTPUT);
  // motor output
  esc.attach(ESC_PIN, esc_neutral, esc_full_power);
  motor_demand(0);
  // pause system until ready
  delay(startup_time);
  // only start listening to the sensors now the system is ready, so that
  // anything that happens during start up is never recorded
  // crank sensor
  pinMode(CRANK_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CRANK_PIN), crankInterrupt, FALLING);
  // wheel sensor
  pinMode(WHEEL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(WHEEL_PIN), wheelInterrupt, FALLING);
  // activate system ready LED
  digitalWrite(SYSTEM_READY_LED_PIN, HIGH);
  Serial.println("System Ready");
}

/*
  Main Loop
  ---------------------
*/
void loop() {
  // get current reference time
  unsigned long current_loop_time = millis();

  // show crank activity LED
  digitalWrite(CRANK_ACTIVITY_LED_PIN, (current_loop_time - crank_interrupt_current_time <= CRANK_PASS_ACTIVITY_DELAY ? HIGH : LOW));

  // show wheel activity LED
  digitalWrite(WHEEL_ACTIVITY_LED_PIN, (current_loop_time - wheel_interrupt_current_time <= WHEEL_PASS_ACTIVITY_DELAY ? HIGH : LOW));

  // calculate crank speed, which stays at zero until the sensor has been
  // passed twice, as one pass on its own cannot tell us how long a turn took
  float crank_rpm = 0;
  float crank_rotation_time = crank_interrupt_current_time - crank_interrupt_previous_time;
  if (crank_interrupt_previous_time > 0 && crank_rotation_time > 1
      && current_loop_time - crank_interrupt_current_time < CRANK_PASS_MAXIMUM_DELAY) {
    crank_rpm = 60.0 / (crank_rotation_time / 1000);
  }

  // calculate wheel speed the same way
  float wheel_kmph = 0;
  float wheel_rotation_time = wheel_interrupt_current_time - wheel_interrupt_previous_time;
  if (wheel_interrupt_previous_time > 0 && wheel_rotation_time > 1
      && current_loop_time - wheel_interrupt_current_time < WHEEL_PASS_MAXIMUM_DELAY) {
    float wheel_rpm = 60.0 / (wheel_rotation_time / 1000);
    wheel_kmph = (wheel_rpm * 60 * WHEEL_CIRCUMFERENCE) / 1000;
  }

  // your control system code goes here!
  //////////////////////////////////////////////////////////////
  // A simple example of a control system.
  // only assist the rider while they are pedalling,
  // and only below the speed limit.

  // how quickly the motor ramps up and down, per loop
  const float motor_step_up = 0.008;
  const float motor_step_down = 0.01;

  if (
      // the cranks are turning fast enough to count as pedalling
      crank_rpm > pedal_threshold_rpm
      // and the bike has not yet reached the 25 km/h assist limit
      && wheel_kmph <= WHEEL_MAXIMUM_SPEED
  ) {
    // ramp the motor up
    digitalWrite(MOTOR_ACTIVITY_LED_PIN, HIGH);
    motor_power = motor_power + motor_step_up;
  } else {
    // ramp the motor down
    digitalWrite(MOTOR_ACTIVITY_LED_PIN, LOW);
    motor_power = motor_power - motor_step_down;
  }
  motor_power = constrain(motor_power, 0.0, 1.0); // change the power output to the motor
  motor_demand(motor_power); // send that power to the motor
  //////////////////////////////////////////////////////////////

  // display message
  if (current_loop_time - message_previous_time >= MESSAGE_MAXIMUM_INTERVAL) {
    message_previous_time = current_loop_time;
    Serial.print("motor demand: ");
    Serial.print(motor_power);
    Serial.print(" crank count: ");
    Serial.print(crank_rotations_counter);
    Serial.print(" wheel count: ");
    Serial.print(wheel_rotations_counter);
    Serial.print(" crank rpm: ");
    Serial.print(crank_rpm);
    Serial.print(" wheel kmph: ");
    Serial.println(wheel_kmph);
  }
  // pause the system for 0.025s
  delay(25);
}

/*
  Crank Sensor Interrupt
  ---------------------
  this runs on its own the moment a magnet passes the sensor,
  whatever the main loop is doing at the time
*/
void crankInterrupt() {
  crank_rotations_counter = crank_rotations_counter + 1;
  crank_interrupt_previous_time = crank_interrupt_current_time;
  crank_interrupt_current_time = millis();
}

/*
  Wheel Sensor Interrupt
  ---------------------
  this runs on its own the moment a magnet passes the sensor,
  whatever the main loop is doing at the time. a second pass
  within 200ms is ignored, in case the sensor bounces
*/
void wheelInterrupt() {
  if (millis() - wheel_interrupt_current_time > WHEEL_PASS_MINIMUM_DELAY){
    wheel_rotations_counter = wheel_rotations_counter + 1;
    wheel_interrupt_previous_time = wheel_interrupt_current_time;
    wheel_interrupt_current_time = millis();
  }
}

/*
  Motor Demand Function
  ---------------------
  demand is a number between 0 (off) and 1 (full power)
*/
void motor_demand(float demand) {
  // never send more or less than the motor can take
  demand = constrain(demand, 0.0, 1.0);
  // servo signal: the ESC on the big rig, the speedometer needle on the mini rig
  esc.writeMicroseconds(esc_neutral + demand * (esc_full_power - esc_neutral));
  // pwm signal: the motor driver on the mini rig
  analogWrite(MOTOR_LOAD_PIN, demand * 255);
}
