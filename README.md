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

Our project is a box designed to protect crops and plants from being eaten or destroyed by wild animals. The box uses Sonar to detect when a person, animal, or object gets too close to it. When it detects something is nearby, it closes by using the motor to shut the sliding lid and then activating the solenoid to use as a lock. If a user wants to open the box, they can type in the passcode on the touch keypad. If the passcode is correct, the box will open by unlocking the solenoid and using the motor to open the sliding lid. The box will then close again automatically after ten seconds.

**Instructions**
1. Download the .bin file to an mbed LPC1768 microcontroller.
2. Set up hardware components according to schematic below.
3. Place lid on planter in closed position.
4. Press the blue button on mbed to run the program.

**Schematic**

**Demo**
