// settings and functions to drive servo motors (directly connected to the arduino)

#include <Servo.h>


typedef struct
{
byte PIN;       //PIN the motor is connected to
byte p_ug;      //min arc for the motor (must not be less than 0!)
byte p_og;      //max arc for the motor (must not be more than 180!)
int a_ug;      //min absolut value that might be displayed (i.e.   0 for a speed indicator)
int a_og;      //max absolut value that might be displayed (i.e. 800 for a speed indicator)
int last;      //previous value
long lu;        //last update
}Servodata;



#define SERVODELAY 200


Servodata servodata[] =
{
  {8, 0, 180, 0, 800,0},  // example speed indicator: {Servo on PIN=9, min arc =0, max arc=180, lowest possible value=0, highest value=800, past value=0}
};
const int SERVOZAHL = sizeof(servodata)/sizeof(servodata[0]);
Servo servo[SERVOZAHL];

SetupServo()
{  
  for (byte lauf=0;lauf<SERVOZAHL;lauf++)
  {
    servo[lauf].attach(servodata[lauf].PIN);
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
        {servo[lauf].attach(servodata[lauf].PIN);}  //reactivate servo
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
  if (servodata[datenfeld[id].ziel].lu + 5000 < millis())
  {
    servo[datenfeld[id].ziel].detach();                        //disable servo if no new signal for more than 5 seconds
  }  
  else if (servodata[datenfeld[id].ziel].lu + SERVODELAY < millis())
  {
    servodata[datenfeld[id].ziel].lu = millis();  //pause servo
    
    if (servodata[datenfeld[id].ziel].last != atoi(datenfeld[id].wert))
    {
      servodata[datenfeld[id].ziel].last = atoi(datenfeld[id].wert); 
      if (!servo[datenfeld[id].ziel].attached())
        {servo[datenfeld[id].ziel].attach(servodata[datenfeld[id].ziel].PIN);}  //reactivate servo
      uint16_t winkel = map(servodata[datenfeld[id].ziel].last, servodata[datenfeld[id].ziel].a_ug, servodata[datenfeld[id].ziel].a_og, servodata[datenfeld[id].ziel].p_ug, servodata[datenfeld[id].ziel].p_og); 
      servo[datenfeld[id].ziel].write(winkel);          
    }
  }
}
