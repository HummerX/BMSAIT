/*
 *  SwitecX25 Arduino Library
 *  Guy Carpenter, Clearwater Software - 2012
 *
 *  Licensed under the BSD2 license, see license.txt for details.
 *
 *  All text above must be included in any redistribution.
 */


#ifndef SwitecX27_h
#define SwitecX27_h
#include <Arduino.h>

class SwitecX27
{
 public:
   static const unsigned char pinCount = 4;
   static const unsigned char stateCount = 6;
   unsigned char pins[pinCount];
   unsigned char currentState;    // 6 steps 
   unsigned int currentStep;      //  step we are currently at
   unsigned int targetStep;       //  target we are moving to
   unsigned int steps;            // total steps available
   unsigned long time0;           // time when we entered this state
   unsigned int microDelay;       // microsecs until next state
   unsigned short (*accelTable)[2]; // accel table can be modified.
   unsigned int maxVel;           // fastest vel allowed
   unsigned int vel;              // steps travelled under acceleration
   signed char dir;               // direction -1,0,1  
   boolean stopped;               // true if stopped
   
   SwitecX27(unsigned int steps, unsigned char pin1, unsigned char pin2, unsigned char pin3, unsigned char pin4);
  
   void stepUp();
   void stepDown();
   void zero();
   void center(unsigned int pos); //Mod
   void update();
   void updateBlocking();
   void setPosition(unsigned int pos);
  
 private:
   void advance();
   void writeIO();
};


#endif
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
