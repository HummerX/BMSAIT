// V1.3.7 26.09.2021
// This module provides functions to read rotary encoders and sends commands to the BMSAIT windows application
// Encoder coding by https://www.nikolaus-lueneburg.de/2016/02/rotary-encoder/

void doEncoderA();
void doEncoderB();

typedef struct //data field structure to define commands for rotary switches with analog reading
{
    byte pin1;              // First encoder PIN
    byte pin2;              // Second encoder PIN
    byte pinShift;          // button of encoder to shift output. Set this to 0 if you don't want to use this
    byte interrupt;         // set interrupt channel
    char commandL[3];       // command to be send on left turn 
    char commandR[3];       // command to be send on right turn 
    char commandLS[3];      // command to be send on left turn in shifted mode
    char commandRS[3];      // command to be send on right turn in shifted mode
    volatile unsigned int encoderPos;  // a counter for the dial
    unsigned int lastReportedPos; // change management
    boolean rotating;  // debounce management
} RotEnc;

RotEnc rotEnc[]=
{
  //PIN1, PIN2, PIN Shift, Interrupt, CMD1,  CMD2,  CMD3,  CMD4  Pos  last rotate
     {2,     3,      0,         1,    "45",  "46",  "00",  "00",  0,    1,   false}
};
const byte RotEncCount = sizeof(rotEnc)/sizeof(rotEnc[0]);


// interrupt service routine vars
boolean A_set = false;            
boolean B_set = false;


void SetupEncoder() 
{
  for (byte enc=0;enc<RotEncCount;enc++)
  {
    pinMode(rotEnc[enc].pin1, INPUT_PULLUP); 
    pinMode(rotEnc[enc].pin2, INPUT_PULLUP); 
    if (rotEnc[enc].pinShift!=0) {pinMode(rotEnc[enc].pinShift, INPUT_PULLUP); }
    if (rotEnc[enc].interrupt==1)
    {
    attachInterrupt(digitalPinToInterrupt(rotEnc[enc].pin1), doEncoderA, CHANGE); // encoder pin on interrupt 0 (pin 2)
    attachInterrupt(digitalPinToInterrupt(rotEnc[enc].pin2), doEncoderB, CHANGE); // encoder pin on interrupt 1 (pin 3)
    }
    else if (rotEnc[enc].interrupt==2)
    {
    attachInterrupt(digitalPinToInterrupt(rotEnc[enc].pin1), doEncoderA, CHANGE); // encoder pin on interrupt 0 (pin 2)
    //attachInterrupt(digitalPinToInterrupt(rotEnc[enc].pin2), doEncoderB, CHANGE); // encoder pin on interrupt 1 (pin 3)
    }
    else if (rotEnc[enc].interrupt==3)
    {
    //attachInterrupt(digitalPinToInterrupt(rotEnc[enc].pin1), doEncoderA, CHANGE); // encoder pin on interrupt 0 (pin 2)
    attachInterrupt(digitalPinToInterrupt(rotEnc[enc].pin2), doEncoderB, CHANGE); // encoder pin on interrupt 1 (pin 3)
    }
  }
}

void CheckEncoder()
{ 
  for (byte x=0;x<RotEncCount;x++)
  {
    rotEnc[x].rotating = true;  // reset the debouncer

    if (rotEnc[x].lastReportedPos != rotEnc[x].encoderPos)
    {
      if (rotEnc[x].lastReportedPos < rotEnc[x].encoderPos)
      {
        if (digitalRead(rotEnc[x].pinShift)==HIGH)
          {
            SendMessage(rotEnc[x].commandL,3);
          }
        else
          {
            SendMessage(rotEnc[x].commandLS,3);
          }
      }
      else
      {
        if (digitalRead(rotEnc[x].pinShift)==HIGH)
          {
            SendMessage(rotEnc[x].commandR,3);
          }
        else
          {
            SendMessage(rotEnc[x].commandRS,3);
          }
      }
      rotEnc[x].lastReportedPos = rotEnc[x].encoderPos;
    }
  }
}

// Interrupt on A changing state
void doEncoderA()
{
  if (rotEnc[0].rotating) delay(1);  // wait a little until the bouncing is done
  if( digitalRead(rotEnc[0].pin1) != A_set )   // debounce once more
  {
    A_set = !A_set;
    if (A_set)
    {
      if(!digitalRead(rotEnc[0].pin2) ) 
         rotEnc[0].encoderPos += 1;
      else 
        rotEnc[0].encoderPos -= 1;
    }
  }
  rotEnc[0].rotating = false;  // no more debouncing until loop() hits again
}

// Interrupt on B changing state
void doEncoderB()
{
  if (rotEnc[1].rotating) delay(1);  // wait a little until the bouncing is done
  if( digitalRead(rotEnc[1].pin1) != B_set )   // debounce once more
  {
    B_set = !B_set;
    if (B_set)
    {
      if(!digitalRead(rotEnc[1].pin2) ) 
         rotEnc[1].encoderPos += 1;
      else 
        rotEnc[1].encoderPos -= 1;
    }
  }
  rotEnc[1].rotating = false;  // no more debouncing until loop() hits again
}
