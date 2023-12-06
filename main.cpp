/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
//#include "platform/mbed_thread.h"
#include "uLCD_4DGL.h"
#include "rtos.h"
#include <cstring>
#include <stdio.h>
#include <string.h>
#include "motordriver.h"
#include "ultrasonic.h"
#include <mpr121.h>
#include <string>

// Blinking rate in milliseconds
#define BLINKING_RATE_MS 500




DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
//DigitalOut Ctrl(p8); //Solenoid output control bit
PwmOut red(p23);//"R" pin
PwmOut blue(p25);//"G" pin
PwmOut green(p24);//"B" pin
uLCD_4DGL LCD(p9,p10,p15);
Timer closetimer;
Timer cleartimer;

// Touchpad Code
// Create the interrupt receiver object on pin 26
InterruptIn interrupt(p26);
// Setup the i2c bus on pins 9 and 10
I2C i2c(p28, p27);
// Setup the Mpr121:
// constructor(i2c object, i2c address of the mpr121)
Mpr121 mpr121(&i2c, Mpr121::ADD_VSS);
//End of touchpad code


volatile bool closed;
Mutex lcd_mutex;
char passcode[4];
char correctPasscode[] = {1,2,3,4};
int currentIndex = 0;
int numberBuffer = -1;
int passcodeFlag = -1;
int overrideMode = 0;

Motor A(p22, p6, p5, 1); // pwm, fwd, rev, can brake
Motor B(p21, p7, p8, 1); // pwm, fwd, rev, can brake
DigitalOut lock(p11); // solenoid
Mutex LOCK;

Serial pc(USBTX, USBRX);
void closeLid();
void openLid();


void dist(int distance)
{
    //put code here to execute when the distance has changed
    pc.printf("Distance %d mm\r\n", distance);

    if (distance < 200 && !closed) {
        closeLid(); //switch order after finding way to stop mid open/close
        closed = 1;
    } else if (distance > 200 && closed) {
        openLid();
        closed = 0;
    }
}

ultrasonic mu(p12, p13, .1, 1, &dist);    //Set the trigger pin to p6 and the echo pin to p7
                                        //have updates every .1 seconds and a timeout after 1
                                        //second, and call dist when the distance changes



// Key hit/release interrupt routine
void fallInterrupt() {
  int key_code= -1;
  int i=0;
  int value=mpr121.read(0x00);
  value +=mpr121.read(0x01)<<8;
  //pc.printf("Inside touchpad interrupt \n");
  // LED demo mod
  i=0;
  // puts key number out to LEDs for demo
  for (i=0; i<12; i++) {
  if (((value>>i)&0x01)==1) key_code=i;
  }
  //pc.printf("KEYCODE: %d \n", key_code);
  if (key_code > 9) {
      switch (key_code) {
          case 10: //clear entire passcode typed
            currentIndex = 0;
            for (int i = 0; i < 4; i++) {
                passcode[i] = NULL;
            }
            break;
          case 11: // enter
            bool equal = 1;

            for (int j = 0; j < 4; j++) {
                if (passcode[j] != correctPasscode[j]) {
                    equal = 0;
                } 
            }
            if (equal) {
                closed = 0;
                passcodeFlag = 0;
                closetimer.start();
                overrideMode = 1;
                openLid();
            } else {
                passcodeFlag = 1;
            }
            cleartimer.start();
            for (int i = 0; i < 4; i++) {
                passcode[i] = NULL;
            }
            currentIndex = 0;
            // if (strcmp(passcode, correctPasscode) == 0) { // check if passcode is equal
            //     closed = 0; //unlocked
            //     openLid();
            //     // lcd_mutex.lock();
            //     // LCD.locate(1, 3);
            //     // LCD.color(GREEN);
            //     // LCD.printf("CORRECT");
            //     // lcd_mutex.unlock();
            //     currentIndex = 0; //reset passcode
            //     for (int j = 0; j < 4; j++) {
            //         passcode[j] = NULL;
            //     }
            // } else {
            //     // lcd_mutex.lock();
            //     // LCD.locate(1, 3);
            //     // LCD.color(RED);
            //     // LCD.printf("INCORRECT");
            //     // lcd_mutex.unlock();
            // }
            break;
          default:
            break;
      }
  } else {
      if (currentIndex < 4 && (key_code != -1)) {
          passcode[currentIndex] = key_code;
          currentIndex++;
      }
      
      

    //   lcd_mutex.lock();
    //   LCD.locate(1 + currentIndex, 5);
    //   LCD.color(WHITE);
    //   LCD.printf("%c", passcode[currentIndex]);
    //   lcd_mutex.unlock();
  }
}



void lockDoor() {
    //pc.printf("Inside lock door \n");
    LOCK.lock();
    lock = 0;
    LOCK.unlock();
}


void unlockDoor() {
    //pc.printf("Inside unlock door \n");
    LOCK.lock();
    lock = 1;
    LOCK.unlock();
}


void openLid() {
    //pc.printf("Inside open lid \n");
    unlockDoor();
    for (float s = 0.0; s < 1.0; s += 0.01) {
        A.speed(s);
        Thread::wait(20);
    }
    Thread::wait(500);
    A.stop(0.9);
    lockDoor();
    Thread::wait(500);
    A.coast();
}


void closeLid() {
    //pc.printf("Inside close lid \n");
    unlockDoor();
    for (float s = 0.0; s > -1.0; s -= 0.01) {
        A.speed(s);
        Thread::wait(20);
    }
    Thread::wait(500);
    A.stop(0.9);
    lockDoor();
    Thread::wait(500);
    A.coast();
}


void thread1(void const *args)
{
    while(true) {       // thread loop
    //pc.printf("Inside thread 1 \n");
        lcd_mutex.lock();
        LCD.color(WHITE);
        for (int i = 0; i < 4; i++) {
            LCD.locate(1 + i, 5);
            if (passcode[i] != NULL) {
                LCD.printf("%d", passcode[i]);
            } else {
                LCD.printf("_");
            }            
        }
        LCD.locate(1, 4);
        if (passcodeFlag == 1) {
            LCD.color(RED);
            LCD.printf("INCORRECT");
        } else if (passcodeFlag == 0) {
            LCD.color(GREEN);
            LCD.printf("CORRECT");
        } else {
            LCD.printf("          ");
        }
        lcd_mutex.unlock();
        Thread::wait(100);
    }
}

void thread2(void const *args)
{
    while(true) {       // thread loop
    //pc.printf("Inside thread 2 \n");
        if (closetimer.read() >= 10) { // if timer is more than ten seconds, open box again
            closetimer.stop();
            closetimer.reset();
            overrideMode = false;
        }
        Thread::wait(1000);
    }
}

int main() {
  //led1 = 1;
  pc.printf("Start of main");
  
  interrupt.fall(&fallInterrupt);
  interrupt.mode(PullUp);
  interrupt.disable_irq();
  
  LCD.cls();
  LCD.locate(1, 2);
  LCD.color(BLUE);
  LCD.printf("ENTER PASSCODE\n");
  mu.startUpdates();
  closed = false;
  //openLid();
  pc.printf("Beg of main \n", closed);


  Thread t1(thread1); //start thread1
  Thread t2(thread2);

  while (1) {
      //pc.printf("Closed: %d \n", closed);
      if (overrideMode) {
          closed = false;
      } else {
        mu.checkDistance();
      }
      if (closed) {
        red = 1;
        green = 0;
      } else {
        red = 0;
        green = 1;
      }

      if (cleartimer.read() >= 2) { // if timer is more than ten seconds, lock box again
          cleartimer.stop();
          cleartimer.reset();
          passcodeFlag = -1;
      }

      for (int i = 0; i < 4; i++) {
          pc.printf("PASSCODE %d: %d \n", i, passcode[i]);
          pc.printf("CORRECT PASSCODE %d: %d \n", i, correctPasscode[i]);
      }

      Thread::wait(100);
  }
}