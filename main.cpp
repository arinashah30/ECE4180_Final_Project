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
int openBuffer = 0;
int sonarBuffer[] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};

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
    int sum = 0;
    int count = 0;
    for (int i = 0; i < 9; i++) {
        int val = sonarBuffer[i];
        sonarBuffer[i] = sonarBuffer[i+1];
        if (val > 10) {
            sum += val;
            count++;
        }
    }
    sonarBuffer[9] = distance;
    if (distance > 10) {
        sum += distance;
        count++;
    }
    int averageDistance = sum/count;

    if (averageDistance < 200 && !closed) {
        for (int i = 0; i < 10; i++) {
            sonarBuffer[i] = 10;
        }
        closeLid(); //switch order after finding way to stop mid open/close
        closed = 1;
    } else if (averageDistance > 200 && closed) {
        for (int i = 0; i < 10; i++) {
            sonarBuffer[i] = 1000;
        }
        openLid();
        closed = 0;
    }
}

ultrasonic mu(p12, p13, .1, 1, &dist);



// Key hit/release interrupt routine
void fallInterrupt() {
  int key_code= -1;
  int i=0;
  int value=mpr121.read(0x00);
  value +=mpr121.read(0x01)<<8;
  i=0;
  for (i=0; i<12; i++) {
  if (((value>>i)&0x01)==1) key_code=i;
  }
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
                openBuffer = 1;
            } else {
                passcodeFlag = 1;
            }
            cleartimer.start();
            for (int i = 0; i < 4; i++) {
                passcode[i] = NULL;
            }
            currentIndex = 0;
            break;
          default:
            break;
      }
  } else {
      if (currentIndex < 4 && (key_code != -1)) {
          passcode[currentIndex] = key_code;
          currentIndex++;
      }
  }
}



void lockDoor() {
    LOCK.lock();
    lock = 0;
    LOCK.unlock();
}


void unlockDoor() {
    LOCK.lock();
    lock = 1;
    LOCK.unlock();
}


void openLid() {
    unlockDoor();
    for (float s = 0.0; s < 0.5; s += 0.01) {
        A.speed(s);
        B.speed(s);
        Thread::wait(20);
    }
    Thread::wait(500);
    A.stop(0.9);
    B.stop(0.9);
    lockDoor();
    Thread::wait(500);
    A.coast();
    B.coast();
}


void closeLid() {
    unlockDoor();
    for (float s = 0.0; s > -0.5; s -= 0.01) {
        A.speed(s);
        B.speed(s);
        Thread::wait(20);
    }
    Thread::wait(500);
    A.stop(0.9);
    B.stop(0.9);
    lockDoor();
    Thread::wait(500);
    A.coast();
    B.coast();
}


void thread1(void const *args)
{
    while(true) {       // thread loop
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
        if (closetimer.read() >= 10) { // if timer is more than ten seconds, open box again
            closetimer.stop();
            closetimer.reset();
            overrideMode = false;
        }
        Thread::wait(1000);
    }
}

int main() {
  
  interrupt.fall(&fallInterrupt);
  interrupt.mode(PullUp);
  interrupt.disable_irq();
  
  LCD.cls();
  LCD.locate(1, 2);
  LCD.color(BLUE);
  LCD.printf("ENTER PASSCODE\n");
  mu.startUpdates();
  closed = false;
  openLid();
  pc.printf("Beg of main \n", closed);


  Thread t1(thread1); //start thread1
  Thread t2(thread2);// start thread2

  while (1) {
      if (overrideMode) {
          closed = false;
          if (openBuffer) {
              openBuffer = 0;
              openLid();
          }
          
      } else {
        mu.checkDistance();
        openBuffer = 0;
      }
      if (closed) {
        red = 1;
        green = 0;
      } else {
        red = 0;
        green = 1;
      }

      if (cleartimer.read() >= 2) { // if timer is more than two seconds, lock box again
          cleartimer.stop();
          cleartimer.reset();
          passcodeFlag = -1;
      }
      Thread::wait(100);
  }
}