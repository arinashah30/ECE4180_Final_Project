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

Our project is a planter designed to protect crops and plants from being eaten or destroyed by wild animals. The planter box uses Sonar to detect when a person, animal, or object gets too close to it. When it detects something is nearby, it closes by using the motor to shut the sliding lid and then activating the solenoid to use as a lock. If a user wants to open the box, they can type in the passcode on the touch keypad. The typed numbers will appear on the uLCD screen, and the user will receive a message on whether the passcode was correct or incorrect when they hit enter. If the passcode is correct, the box will open by unlocking the solenoid and using the motor to open the sliding lid. The box will then close again automatically after ten seconds. There is also an LED that shows the status of the planter, green for open and red for closed.

![4180FinalProject drawio (1)](https://github.com/arinashah30/ECE4180_Final_Project/assets/101674938/4457f872-9a46-4f8a-b496-00fa9ad4b117)
![IMG_7611](https://github.com/arinashah30/ECE4180_Final_Project/assets/101674938/0d07a770-15a0-48f0-bb6c-28af0a078d63)
![IMG_7613](https://github.com/arinashah30/ECE4180_Final_Project/assets/101674938/0f29ae3b-5d51-45d8-a0cf-73675615ddab)
![IMG_7615](https://github.com/arinashah30/ECE4180_Final_Project/assets/101674938/d228f55f-193e-4c3c-a4c3-3353f31ea48d)
![IMG_7618](https://github.com/arinashah30/ECE4180_Final_Project/assets/101674938/db75d898-8ce6-47a8-b4d9-fae05cfbf435)
![IMG_7619](https://github.com/arinashah30/ECE4180_Final_Project/assets/101674938/71b3250e-6476-40b4-9050-4fda3f6f8c27)



**Instructions**

Setup:
1. Download the .bin file to an mbed LPC1768 microcontroller.
2. Set up hardware components according to schematic below.
3. Place lid on planter in closed position.

Running the program:
1. Press the blue button on the mbed to reset/run the program.
2. The planter will close if you get close to it.
3. To open the planter when you're nearby, type in the passcode using the touch keypad.
4. To clear the keypad, press key 10. To enter the code you've typed, press key 11.
5. If you entered the correct passcode, the box will open for 10 seconds.

**Software**

Libraries Used:
- 4DGL-uLCD-SE: http://os.mbed.com/users/4180_1/code/4DGL-uLCD-SE/
- HC_SR04_Ultrasonic_Library: http://developer.mbed.org/users/ejteb/code/HC_SR04_Ultrasonic_Library/
- Motordriver: http://os.mbed.com/users/littlexc/code/Motordriver/
- mbed RTOS: http://os.mbed.com/users/mbed_official/code/mbed-rtos/
- MPR121 touch keypad: import the mpr121.cpp and mpr121.h files in the repo to your project


**Schematic**

<img width="526" alt="Screenshot 2023-12-07 at 4 56 42 PM" src="https://github.com/arinashah30/ECE4180_Final_Project/assets/101674938/d64cb50c-0daa-49c2-9d86-c3782d6b328c">


**Demo**

https://youtu.be/GgX_ZeZfH3Y


**Summary of Results**
- 5 output devices and 2 input devices controlled by Mbed LPC1768​

- Concurrent threads used to synchronize actions and simplify functionality of the product​

- Sonar was difficult to work with and gave inaccurate measurements at times​

- Mechanical issues with gears and lining up with tracks

**Future Ideas**

- Further perfect planter box design and hide hardware components for a cleaner look​

- Replace Sonar with LIDAR for more accurate readings​

- Add a speaker to sound an alarm when someone is nearby​

- Use Bluetooth module to type in the passcode from your phone​

- Allow the user to change the passcode​

- Custom time limit for how long the box stays open
