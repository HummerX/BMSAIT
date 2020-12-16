// settings and functions to drive servo motors (directly connected to the arduino)
//target= reference link to the line of the servodata table of this module
//ref2= not used
//ref3= not used
//ref4= not used
//ref5= not used

#include <Servo.h>

#define SERVODELAY 200

typedef struct
{
byte pIN;           //PIN the motor is connected to
byte p_ug;          //min arc for the motor (must not be less than 0!)
byte p_og;          //max arc for the motor (must not be more than 180!)
int a_ug;           //min absolut value that might be displayed (i.e.   0 for a speed indicator)
int a_og;           //max absolut value that might be displayed (i.e. 800 for a speed indicator)
int last;           //previous value
unsigned long lu;   //last update
} Servodata;


Servodata servodata[] =
{// pIN  p_ug  p_og  a_ug  a_og  last  lu
    {8,   0,   180,   0,   800,    0,   0},  // example speed indicator: {Servo on PIN=9, min arc =0, max arc=180, lowest possible value=0, highest value=800, past value=0, lastUpdate=0}
};
const int SERVOZAHL = sizeof(servodata)/sizeof(servodata[0]);
Servo servo[SERVOZAHL];

void SetupServo()
{  
  for (byte lauf=0;lauf<SERVOZAHL;lauf++)
  {
    servo[lauf].attach(servodata[lauf].pIN);
    servo[lauf].write(servodata[lauf].p_ug);
    delay(1000);
    servo[lauf].write(servodata[lauf].p_og);
    delay(1000);
    servo[lauf].write(90);
    delay(200);
    servo[lauf].detach();
  }
}

void Servo_Zeroize(void)
{
  for (byte lauf=0;lauf<SERVOZAHL;lauf++)
  {
    if (!servo[lauf].attached())
        {servo[lauf].attach(servodata[lauf].pIN);}  //reactivate servo
    servo[lauf].write(servodata[lauf].p_ug);
    delay(1000);
    servo[lauf].write(servodata[lauf].p_og);
    delay(1000);
    servo[lauf].write(servodata[lauf].p_og - servodata[lauf].p_ug);
    delay(200);
    servo[lauf].detach();
  }
}


void UpdateServo(byte id)
{
  if (servodata[datenfeld[id].target].lu + 5000 < millis())
  {
    servo[datenfeld[id].target].detach();                        //disable servo if no new signal for more than 5 seconds
  }  
  else if (servodata[datenfeld[id].target].lu + SERVODELAY < millis())
  {
    servodata[datenfeld[id].target].lu = millis();  //pause servo
    
    if (servodata[datenfeld[id].target].last != atoi(datenfeld[id].wert))
    {
      servodata[datenfeld[id].target].last = atoi(datenfeld[id].wert); 
      if (!servo[datenfeld[id].target].attached())
        {servo[datenfeld[id].target].attach(servodata[datenfeld[id].target].pIN);}  //reactivate servo
      uint16_t winkel = map(servodata[datenfeld[id].target].last, servodata[datenfeld[id].target].a_ug, servodata[datenfeld[id].target].a_og, servodata[datenfeld[id].target].p_ug, servodata[datenfeld[id].target].p_og); 
      servo[datenfeld[id].target].write(winkel);          
    }
  }
}
