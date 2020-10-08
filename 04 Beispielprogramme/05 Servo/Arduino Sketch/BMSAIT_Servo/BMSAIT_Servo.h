// settings and functions to drive servo motors (directly connected to the arduino)

#include <Servo.h>


typedef struct
{
  byte PIN;       //PIN the motor is connected to
  byte p_ug;      //min arc for the motor (must not be less than 0!)
  byte p_og;      //max arc for the motor (must not be more than 180!)
  int a_ug;       //min absolut value that might be displayed (i.e.   0 for a speed indicator)
  int a_og;       //max absolut value that might be displayed (i.e. 800 for a speed indicator)
  int last;       //previous value
  long lu;        //last update
}Servodata;



#define SERVODELAY 200

//IMPORTANT:  You need to calibrate each servo to prevent damage.
//during startup, each servo will be moved to the min/max value and stay there for 1 second. If the servo is vibrating in a min or max position, the position is out of bounds. 
//in this case, you need to increase the min value (p_ug) or reduce the max value (p_og) in the following table until the servo will not vibrate in the min/max positions

Servodata servodata[] =
{
// PIN  p_ug   p_og   a_ug   a_og   lu
   {2,    10,   175,     0,   110,   0}  // example rpm indicator: {Servo on PIN=2, min arc =10, max arc=175, lowest possible data value=0% RPM, highest possible data value=110% RPM, past value=0}
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
    servo[datenfeld[id].ziel].detach();                        //disable servo if no new signal arrived for more than 5 seconds
  }  
  else if (servodata[datenfeld[id].ziel].lu + SERVODELAY < millis())  
  {
    //check for new servo movement
    servodata[datenfeld[id].ziel].lu = millis(); 
    int val = atoi(datenfeld[id].wert);
    
    if (servodata[datenfeld[id].ziel].last != val)
    {
      //initiate new movement
      servodata[datenfeld[id].ziel].last = val; 
      if (!servo[datenfeld[id].ziel].attached())
        {servo[datenfeld[id].ziel].attach(servodata[datenfeld[id].ziel].PIN);}  //reactivate servo
      //calculate the new arc position for the servo
      uint16_t winkel = map(servodata[datenfeld[id].ziel].last, servodata[datenfeld[id].ziel].a_ug, servodata[datenfeld[id].ziel].a_og, servodata[datenfeld[id].ziel].p_ug, servodata[datenfeld[id].ziel].p_og); 
      //move servo
      servo[datenfeld[id].ziel].write(winkel);          
    }
  }
}
