// This module provides functions to read rotary encoders and sends commands to the BMSAIT windows application
// Encoder coding by https://www.nikolaus-lueneburg.de/2016/02/rotary-encoder/

void doEncoderA();
void doEncoderB();

typedef struct //data field structure to define commands for rotary switches with analog reading
{
    byte pin1;              // First encoder PIN
    byte pin2;              // Second encoder PIN
    byte pinShift;          // button of encoder to shift output. Set this to 0 if you don't want to use this
    char commandL[3];       // command to be send on left turn 
    char commandR[3];       // command to be send on right turn 
    char commandLS[3];      // command to be send on left turn in shifted mode
    char commandRS[3];      // command to be send on right turn in shifted mode
} RotEnc;

//             PIN1, PIN2, PIN Shift, CMD1,  CMD2,  CMD3,  CMD4
RotEnc rotEnc = { 2,   3,       4,     "01",  "02",  "03",  "04" };


volatile unsigned int encoderPos = 0;  // a counter for the dial
unsigned int lastReportedPos = 1;      // change management
static boolean rotating = false;       // debounce management

// interrupt service routine vars
boolean A_set = false;
boolean B_set = false;


void SetupEncoder()
{
  pinMode(rotEnc.pin1, INPUT_PULLUP);
  pinMode(rotEnc.pin2, INPUT_PULLUP);
  if (rotEnc.pinShift != 0) { pinMode(rotEnc.pinShift, INPUT_PULLUP); }
  attachInterrupt(digitalPinToInterrupt(rotEnc.pin1), doEncoderA, CHANGE); // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(digitalPinToInterrupt(rotEnc.pin2), doEncoderB, CHANGE); // encoder pin on interrupt 1 (pin 3)
}

void CheckEncoder()
{
  rotating = true;  // reset the debouncer

  if (lastReportedPos != encoderPos)
  {
      bool shft = true;
      if ((rotEnc.pinShift != 0) && (digitalRead(rotEnc.pinShift) == LOW)) { shft = false; }

      if (lastReportedPos < encoderPos)
      {
        if (shft)
        {SendMessage(rotEnc.commandL, 3);}
        else
        {SendMessage(rotEnc.commandLS, 3);}
      }
      else
      {
        if (shft)
        {SendMessage(rotEnc.commandR, 3);}
        else
        {SendMessage(rotEnc.commandRS, 3);}
      }
      lastReportedPos = encoderPos;
  }
}

// Interrupt on A changing state
void doEncoderA()
{
    if (rotating) delay(1);  // wait a little until the bouncing is done
    if (digitalRead(rotEnc.pin1) != A_set) {  // debounce once more
        A_set = !A_set;
        // adjust counter + if A leads B
        if (A_set && !B_set)
            encoderPos += 1;
        rotating = false;  // no more debouncing until loop() hits again
    }
}

// Interrupt on B changing state, same as A above
void doEncoderB() {
    if (rotating) delay(1);
    if (digitalRead(rotEnc.pin2) != B_set) {
        B_set = !B_set;
        //  adjust counter - 1 if B leads A
        if (B_set && !A_set)
            encoderPos -= 1;
        rotating = false;
    }
}
