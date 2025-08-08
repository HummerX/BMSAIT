/*
 *  SwitecX25 Arduino Library
 *  Guy Carpenter, Clearwater Software - 2012
 *
 *  Licensed under the BSD2 license, see license.txt for details.
 *
 *  All text above must be included in any redistribution.
 */
 
#include <Arduino.h>

#include "SwitecX27cont.h"

// During zeroing we will step the motor CCW 
// with a fixed step period defined by RESET_STEP_MICROSEC
#define RESET_STEP_MICROSEC 800

// This table defines the acceleration curve as a list of (step, delay) pairs.
// 1st value is the cumulative step count since starting from rest, 2nd value is delay in microseconds.
// 1st value in each subsequent row must be > 1st value in previous row
// The delay in the last row determines the maximum angular velocity.
static unsigned short defaultAccelTable[][2] = {
//  {   20, 3000},
//  {   50, 1500},
//  {  100, 1000},
//  {  150,  800},
//  {  300,  600}
  {   20, 5000},
  {   50, 3000},
  {  100, 1500},
  {  150, 1000},
  {  300,  800}
};
#define DEFAULT_ACCEL_TABLE_SIZE (sizeof(defaultAccelTable)/sizeof(*defaultAccelTable))

// experimentation suggests that 400uS is about the step limit 
// with my hand-made needles made by cutting up aluminium from
// floppy disk sliders.  A lighter needle will go faster.
  
// State  3 2 1 0   Value
// 0      1 0 0 1   0x9
// 1      0 0 0 1   0x1
// 2      0 1 1 1   0x7
// 3      0 1 1 0   0x6
// 4      1 1 1 0   0xE
// 5      1 0 0 0   0x8
static byte stateMap[] = {0x9, 0x1, 0x7, 0x6, 0xE, 0x8};

SwitecX27::SwitecX27(unsigned int steps, unsigned char pin1, unsigned char pin2, unsigned char pin3, unsigned char pin4)
{
  this->currentState = 0;
  this->steps = steps;
  this->pins[0] = pin1;
  this->pins[1] = pin2;
  this->pins[2] = pin3;
  this->pins[3] = pin4;
  for (int i=0;i<pinCount;i++) {
    pinMode(pins[i], OUTPUT);
  }
  
  dir = 0;
  vel = 0; 
  stopped = true;
  currentStep = 0;
  targetStep = 0;
  
  accelTable = defaultAccelTable;
  maxVel = defaultAccelTable[DEFAULT_ACCEL_TABLE_SIZE-1][0]; // last value in table.
}

void SwitecX27::writeIO()
{

  byte mask = stateMap[currentState];  
  for (int i=0;i<pinCount;i++) {
    digitalWrite(pins[i], mask & 0x1);
    mask >>= 1;
  }
}

void SwitecX27::stepUp()
{
  if (currentStep < steps) {
    currentStep++;
    currentState = (currentState + 1) % stateCount;
    writeIO();
  }
}

void SwitecX27::stepDown()
{ 
  if (currentStep > 0) {
    currentStep--;
    currentState = (currentState + 5) % stateCount;
    writeIO();
  }
}

void SwitecX27::zero()
{
  currentStep = steps - 1;
  for (unsigned int i=0;i<steps;i++) {
    stepDown();
    delayMicroseconds(RESET_STEP_MICROSEC);
  }
  currentStep = 0;
  targetStep = 0;
  vel = 0;
  dir = 0;
}

void SwitecX27::center(unsigned int pos)
{
  currentStep = pos;
  targetStep = pos;
  vel = 0;
  dir = 0;
}

// This function determines the speed and accel
// characteristics of the motor.  Ultimately it 
// steps the motor once (up or down) and computes
// the delay until the next step.  Because it gets
// called once per step per motor, the calculations
// here need to be as light-weight as possible, so
// we are avoiding floating-point arithmetic.
//
// To model acceleration we maintain vel, which indirectly represents
// velocity as the number of motor steps travelled under acceleration
// since starting.  This value is used to look up the corresponding
// delay in accelTable.  So from a standing start, vel is incremented
// once each step until it reaches maxVel.  Under deceleration 
// vel is decremented once each step until it reaches zero.

void SwitecX27::advance()
{
  // detect stopped state
  if (currentStep==targetStep && vel==0) {
    stopped = true;
    dir = 0;
    time0 = micros();
    return;
  }
  
  // if stopped, determine direction
  if (vel==0) {
    dir = currentStep<targetStep ? 1 : -1;
    // do not set to 0 or it could go negative in case 2 below
    vel = 1; 
  }
  
  if (dir>0) {
    stepUp();
  } else {
    stepDown();
  }
  
  // determine delta, number of steps in current direction to target.
  // may be negative if we are headed away from target
  int delta = dir>0 ? targetStep-currentStep : currentStep-targetStep;
  
  if (delta>0) {
    // case 1 : moving towards target (maybe under accel or decel)
    if (delta < vel) {
      // time to declerate
      vel--;
    } else if (vel < maxVel) {
      // accelerating
      vel++;
    } else {
      // at full speed - stay there
    }
  } else {
    // case 2 : at or moving away from target (slow down!)
    vel--;
  }
    
  // vel now defines delay
  unsigned char i = 0;
  // this is why vel must not be greater than the last vel in the table.
  while (accelTable[i][0]<vel) {
    i++;
  }
  microDelay = accelTable[i][1];
  time0 = micros();
}

void SwitecX27::setPosition(unsigned int pos)
{
  // pos is unsigned so don't need to check for <0
  targetStep = pos;
  if (stopped) {
    // reset the timer to avoid possible time overflow giving spurious deltas
    stopped = false;
    time0 = micros();
    microDelay = 0;
  }
}

void SwitecX27::update()
{
  if (!stopped) {
    unsigned long delta = micros() - time0;
    if (delta >= microDelay) {
      advance();
    }
  }
}


//This updateMethod is blocking, it will give you smoother movements, but your application will wait for it to finish
void SwitecX27::updateBlocking()
{
  while (!stopped) {
    unsigned long delta = micros() - time0;
    if (delta >= microDelay) {
      advance();
    }
  }
}

/*

 Software License Agreement (BSD License)

Copyright (c) 2012, Clearwater Software
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions
are met:

 * Redistributions of source code must retain the above copyright 
   notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in 
   the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
