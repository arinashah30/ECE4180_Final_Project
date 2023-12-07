# ECE4180 Final Project: Crop Protection Planter

Names: Arina Shah, Zane Stokesberry, Tomi Ajayi

Parts List:
- mbed
- custom 3D printed box with sliding lid
- sonar
- uLCD
- RGB LED
- solenoid
- 2 DC motors
- MPR121 touch keypad
- external 5V power

**Project Idea**

Our project is a planter designed to protect crops and plants from being eaten or destroyed by wild animals. The planter box uses Sonar to detect when a person, animal, or object gets too close to it. When it detects something is nearby, it closes by using the motor to shut the sliding lid and then activating the solenoid to use as a lock. If a user wants to open the box, they can type in the passcode on the touch keypad. If the passcode is correct, the box will open by unlocking the solenoid and using the motor to open the sliding lid. The box will then close again automatically after ten seconds.

**Instructions**
1. Download the .bin file to an mbed LPC1768 microcontroller.
2. Set up hardware components according to schematic below.
3. Place lid on planter in closed position.
4. Press the blue button on mbed to run the program.

**Software**

Libraries Used:
- 4DGL-uLCD-SE: http://os.mbed.com/users/4180_1/code/4DGL-uLCD-SE/
- HC_SR04_Ultrasonic_Library: http://developer.mbed.org/users/ejteb/code/HC_SR04_Ultrasonic_Library/
- Motordriver: http://os.mbed.com/users/littlexc/code/Motordriver/
- mbed RTOS: http://os.mbed.com/users/mbed_official/code/mbed-rtos/
- MPR121 touch keypad: import the mpr121.cpp and mpr121.h files in the repo to your project

**Schematic**

<img width="623" alt="Screenshot 2023-12-07 at 4 20 43 PM" src="https://github.com/arinashah30/ECE4180_Final_Project/assets/101674938/a44d3694-1979-442f-9ab8-88a9ce8651d2">

**Demo**
