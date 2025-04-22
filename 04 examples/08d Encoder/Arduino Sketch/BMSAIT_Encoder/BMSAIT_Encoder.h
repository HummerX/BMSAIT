//V1.3.7 26.09.2021
// This module provides functions to read rotary encoders and send commands to the BMSAIT windows application
// define any attached input controls (buttons, switches) and the command to be send back to Windows.
// standard Arduino boards (Uno, Nano) only support two interrupt channels, so the number of encoders in this code is limited to 2 devices.
// If you use other boards (Due,Mega) the code needs to be amended to add additional "doEncoderX()"functions for each encoder

void doEncoderA();
void doEncoderB();

typedef struct //data field structure to define commands for rotary switches with analog reading
{
  byte pin1;             // PIN1 (A)
  byte pin2;             // PIN2 (B)
  byte pinShift;         // button of encoder to shift output
  char commandL[3];      // command to be send on left turn 
  char commandR[3];      // command to be send on right turn 
  char commandLS[3];      // command to be send on left turn in shifted mode
  char commandRS[3];      // command to be send on right turn in shifted mode
  volatile unsigned int encoderPos;  // a counter for the dial
  unsigned int lastReportedPos; // change management
  boolean rotating;  // debounce management
} RotEnc;

RotEnc rotEnc[]=
{
  //PIN1, PIN2, PIN Shift, CMD1,  CMD2,  CMD3,  CMD4 Pos  last rotate
  {  2,    4,       5,     "1",   "2",   "3",   "4",  0,  1,  false}     //demo encoder
  //,{  3,    6,       7,     "5",   "6",   "7",   "8",  0,  1,  false}  //optional second encoder
};
const byte RotEncCount = sizeof(rotEnc)/sizeof(rotEnc[0]); 

// interrupt service routine vars
boolean A_set = false;            
boolean B_set = false;


void SetupEncoder() 
{
  for (byte x=0;x<RotEncCount;x++)
  {
    pinMode(rotEnc[x].pin1, INPUT_PULLUP); 
    pinMode(rotEnc[x].pin2, INPUT_PULLUP); 
    if (rotEnc[x].pinShift!=0) {pinMode(rotEnc[x].pinShift, INPUT_PULLUP); }
    
  }
  attachInterrupt(digitalPinToInterrupt(rotEnc[0].pin1), doEncoderA, CHANGE); // encoder pin on interrupt 0 (pin 2) 
  if (RotEncCount==2) attachInterrupt(digitalPinToInterrupt(rotEnc[1].pin1), doEncoderB, CHANGE); // encoder pin on interrupt 1 (pin 3) 
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
  if (rotEnc[0].rotating) delay(1);  // wait a little until the bouncing is done
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
