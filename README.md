# MAC 233 — Arduino Labs

This repository contains all the documentation, code and information you will need for the MAC233 Arduino programming course.

## Timeline

The Arduino labs run over seven weeks, in two halves: four standalone lab activities, then three weeks building a control system for your mini-bike rig.

![Course timeline: weeks 3 to 6 are standalone lab activities — Arduino introduction, LED, Servo, Hall Effect Sensors — and weeks 7 to 9 are spent developing the control system.](images/timeline.png)

Each lab builds on the one before it, so work through them in order.

## Getting started

You will need the [Arduino IDE](https://www.arduino.cc/en/software) and an **Arduino Nano Every**.

In the IDE, go to `Tools` → `Board` → `Select Other Board and Port`, search for `nano every`, and select both the board **and** the port — selecting only the board will let you compile, but not upload.

Keep the [Nano Every pinout diagram](https://docs.arduino.cc/resources/pinouts/ABX00028-full-pinout.pdf) to hand: every lab refers to it when telling you which pin to wire something to.

If you have not used the Arduino IDE before, Lab 1 walks through all of this step by step, start there!

## Labs

During weeks 3-6 we will complete standalone lab activities. All the required information can be found below. If you finish a lab early, feel free to move onto the next one. 

| Week | Lab | Sketch | Worksheet | Slides |
|---|---|---|---|---|
| 3 | Introduction to Arduino / Blink | [ino](labs/lab-1-blink/lab-1-blink.ino) | [pdf](labs/lab-1-blink/docs/lab-1-blink.pdf) | [pdf](labs/lab-1-blink/slides/slides.pdf) |
| 4 | External LED | [ino](labs/lab-2-led/lab-2-led.ino) | [pdf](labs/lab-2-led/docs/lab-2-led.pdf) | [pdf](labs/lab-2-led/slides/slides.pdf) |
| 5 | Servo | [ino](labs/lab-3-servo/lab-3-servo.ino) | [pdf](labs/lab-3-servo/docs/lab-3-servo.pdf) | [pdf](labs/lab-3-servo/slides/slides.pdf) |
| 6 | Hall effect sensors | [ino](labs/lab-4-hall-effect-sensor/lab-4-hall-effect-sensor.ino) | [pdf](labs/lab-4-hall-effect-sensor/docs/lab-4-hall-effect-sensor.pdf) | [pdf](labs/lab-4-hall-effect-sensor/slides/slides.pdf) |



## Control system

During weeks 7-9, you will work on developing a control system for your e-bike. By week 7 you should have completed the manufacturing for your mini-bike rig and so we will use this to test our control systems.

The starting sketch is here: [control-system.ino](control-system/control-system.ino). Worksheets and slides for these weeks will follow.

## Licence and attribution

This material was originally written by Dr Dan Brennan ([github.com/dsbrennan](https://github.com/dsbrennan)) and is now maintained by Dr Max Champneys (max.champneys@sheffield.ac.uk).

Released under the MIT Licence.

