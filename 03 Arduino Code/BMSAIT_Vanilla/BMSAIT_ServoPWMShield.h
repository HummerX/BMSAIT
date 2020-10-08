// settings and functions to drive servo motors (via pwm motor shield)

#include <Adafruit_PWMServoDriver.h>

typedef struct
{
byte port;      //channel of the pwm shield the motor is connected to
int minPulse;   //min pulse for the motor 
int maxPulse;   //max pulse for the motor 
int a_ug;       //min absolut value that might be displayed (i.e.   0 for a speed indicator)
int a_og;       //max absolut value that might be displayed (i.e. 600 for a speed indicator)
int last;       //previous value
long lu;        //last update
}ServodataPWM;

//IMPORTANT:  You need to calibrate each servo to prevent damage.
//during calibration, each servo will be moved to the min/max value and stay there for 1 second. If the servo is vibrating in a min or max position, the position is out of bounds. 
//in this case, you need to increase the min value (minPulse) or reduce the max value (maxPulse) in the following table until the servo does not vibrate in the min/max positions

ServodataPWM servodataPWM[] =
{
// Channel  minPulse   maxPulse   a_ug   a_og   last  lu
    {0,       160,       605,      0,    600,     0,   0}  // example RPM: {Servo on Channel 0, min pulse length (around 150)=160, max pulse length (around 600)=605, lowest possible value=0, highest value=103, past value=0, last update=0}
};
const int SERVOZAHLPWM = sizeof(servodataPWM)/sizeof(servodataPWM[0]);


#define SERVODELAYPWM 500

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver( 0x40 );


void SetupServoPWM()
{  
  pwm.begin();
  pwm.setPWMFreq(60);
  for (byte x=0;x<=SERVOZAHLPWM;x++) //move all servos to center position
  {  
    int pulselength = map(90, 0, 180, servodataPWM[x].minPulse, servodataPWM[x].maxPulse);
    pwm.setPWM(x, 0, pulselength);
    delay(200);
  }
}

void ServoPWM_Zeroize(void)
{
  for (byte x=0;x<=SERVOZAHLPWM;x++) //move all servos to min position
  {  
    int pulselength = map(0, 0, 180, servodataPWM[x].minPulse, servodataPWM[x].maxPulse);
    pwm.setPWM(x, 0, pulselength);
    delay(200);
  }
  delay(1000);
  for (byte x=0;x<=SERVOZAHLPWM;x++) //move all servos to max position
  {  
    int pulselength = map(180, 0, 180, servodataPWM[x].minPulse, servodataPWM[x].maxPulse);
    pwm.setPWM(x, 0, pulselength);
    delay(200);
  }
  delay(1000);
  for (byte x=0;x<=SERVOZAHLPWM;x++) //move all servos to center position
  {  
    int pulselength = map(90, 0, 180, servodataPWM[x].minPulse, servodataPWM[x].maxPulse);
    pwm.setPWM(x, 0, pulselength);
    delay(200);
  }
}

void UpdateServoPWM(int d)
{
  byte servoID=datenfeld[d].ziel;
  
  if (servodataPWM[servoID].lu+5000<millis())
  {
    pwm.sleep();         //set servo to sleep mode if no new signal arrived since 5 seconds
    if (testmode){SendMessage("PWM went to sleep",1);}
  }  
    
  if (servodataPWM[servoID].lu + SERVODELAYPWM < millis())
  {

    if (servodataPWM[servoID].last != atoi(datenfeld[d].wert))
    {
      pwm.wakeup();                       //wake up servo
      servodataPWM[servoID].lu = millis();
      servodataPWM[servoID].last = atoi(datenfeld[d].wert); 

      //calculate pulselength 
      uint16_t pulselength=0;
      if (servodataPWM[servoID].last<servodataPWM[servoID].a_ug)
        {pulselength=servodataPWM[servoID].minPulse;}
      else if (servodataPWM[servoID].last>servodataPWM[servoID].a_og)
        {pulselength=servodataPWM[servoID].maxPulse;}
      else
        {pulselength = map(servodataPWM[servoID].last, servodataPWM[servoID].a_ug, servodataPWM[servoID].a_og, servodataPWM[servoID].minPulse, servodataPWM[servoID].maxPulse);}
      
      //move servo
      pwm.setPWM(servodataPWM[servoID].port, 0, pulselength);         
    }
  }
}
