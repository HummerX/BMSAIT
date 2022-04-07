// settings and functions to drive a motor driven potentiometer
// V1.3.7 26.09.2021

#define DIR_DELAY 50 // brief delay for abrupt motor changes
#define BUFFER 10     // threshold that needs to be crossed to consider a change on the analog value as a movement

#ifdef ESP  
  #include <analogWrite.h>
#endif

//function declaration
void MotorPoti_Zeroize(bool mode);

typedef struct 
{
  byte pIN1;         //Pin1 of the Motor (movement order)
  byte pIN2;         //Pin2 of the Motor (movement direction)
  byte pWM;          //Motor Speed (0 to 255)
  bool dir;          //Direction - synchronize motor movement and poti readout
  byte poti;         //Analog pin for position readout
  int trimPos_int;   //Internal memory for the current trim position (0..1024)
  int trimPos_ext;   //TrimPos as read from BMS (0..1024)
  int command;       //ID of the command line in BMS Windows App that links a vjoy analog axis
}  Struc_MotorPoti;


Struc_MotorPoti motorPoti[]=
{
	  	//  PIN1	 PIN2	  pWM	    dir	   poti 	intP	 extP   command
		 {     5,     6,	  200,	  true,	  A0,  	500,	 500,    1}

};
const byte motorPotiAnz = sizeof(motorPoti)/sizeof(motorPoti[0]); 

bool error=false;
long time_status=0; //time for new status message
long time_pause=0; //pause when poti was moved


void MotorMoveCW(byte mp)
{
  // always stop motors briefly before abrupt changes
  digitalWrite( motorPoti[mp].pIN1, LOW );
  digitalWrite( motorPoti[mp].pIN2, LOW );
  delay( DIR_DELAY );
  // set the motor speed and direction
  digitalWrite( motorPoti[mp].pIN1, LOW ); 
  analogWrite( motorPoti[mp].pIN2, motorPoti[mp].pWM );
}

void MotorMoveCCW(byte mp)
{
  
  // always stop motors briefly before abrupt changes
  digitalWrite( motorPoti[mp].pIN1, LOW );
  digitalWrite( motorPoti[mp].pIN2, LOW );
  delay( DIR_DELAY );
  // set the motor speed and direction
  analogWrite( motorPoti[mp].pIN1, motorPoti[mp].pWM ); 
  digitalWrite( motorPoti[mp].pIN2, LOW ); 
}

void MotorStop(byte mp)
{
  digitalWrite( motorPoti[mp].pIN1, LOW );
  digitalWrite( motorPoti[mp].pIN2, LOW );
  delay(10);
}

bool CheckDirection(byte motor,bool richtung,bool recursive)
{
  int tmp=0;
  int oldPos=analogRead(motorPoti[motor].poti);
  if (richtung)
  {
    //current position is left of center, so check CW movement
    MotorMoveCW(motor);
    delay(1500);
    MotorStop(motor);
    
    //motor moved CW for 1.5 seconds. Now check if the poti readout increased
    tmp=analogRead(motorPoti[motor].poti);

    if (tmp>oldPos+BUFFER) 
      {motorPoti[motor].dir=true;} 
    else if (tmp<oldPos-BUFFER) 
      {motorPoti[motor].dir=false;} 
    else
    {  
      if (recursive) return false;
      
      //no good signal so far. Lets try the other direction
      if (CheckDirection(motor,false,true))   
        {return true;} //motor recovered
      else  
        {return false;} //motor didn`t move or poti readout is faulty  
    }
  }
  else
  {
    //current position is right of center, so check CCW movement
    MotorMoveCCW(motor);
    delay(1500);
    MotorStop(motor);
      
    //motor moved CCW for 1.5 seconds. Now check if the poti readout decreased
    tmp=analogRead(motorPoti[motor].poti);
    if (tmp>oldPos+BUFFER) 
        {motorPoti[motor].dir=false;}  
    else if (tmp<oldPos-BUFFER) 
        {motorPoti[motor].dir=true;} 
    else
    {
      if (recursive) return false;
    
      //no good signal so far. Lets try the other direction
      if (CheckDirection(motor,true,true))   
        {return true;} //motor recovered
      else  
        {return false;} //motor didn`t move or poti readout is faulty  
    }
  }
  return true;
}


void MotorPoti_Zeroize(bool mode)
{
  error=false;
  byte result;
  for (byte mp=0;mp<motorPotiAnz;mp++)
  {
    if ((motorPoti[mp].pIN1==0) || (motorPoti[mp].pIN2==0)) continue;  //skip loop if no Pins were assigned
    
    int initialPos=analogRead(motorPoti[mp].poti);
    
    // check if the direction of motor movement and poti readout matches
    if (initialPos<=512)
      { error=CheckDirection(mp,true,false);}
    else
      { error=CheckDirection(mp,false,false);}
    SendMessage("CheckDir complete",1);  
    if (!error)
    {
      initialPos=analogRead(motorPoti[mp].poti);
      //move motor to center position
      if (initialPos>(512+BUFFER))
      {
        if (motorPoti[mp].dir) MotorMoveCCW(mp); else MotorMoveCW(mp);
        while((analogRead(motorPoti[mp].poti)>=512))
          {delay(1);}
        MotorStop(mp);
      }
      else if (initialPos<(512-BUFFER))
      {
        if (motorPoti[mp].dir) MotorMoveCW(mp); else MotorMoveCCW(mp);
        while((analogRead(motorPoti[mp].poti)<=512))
          {delay(1);}
        MotorStop(mp);

      }
      else
      {} //motor already is in center position
      
    }
    else 
    {
      SendMessage("Fehler beim Initalisieren der MotorPotis",1);
    }
  }
}

void SetupMotorPoti() 
{
  for (byte x=0;x<motorPotiAnz;x++)
  {
    byte mp=x;
    if ((motorPoti[mp].pIN1==0) || (motorPoti[mp].pIN2==0)) continue;  //skip loop if no Pins were assigned
    pinMode( motorPoti[mp].pIN1, OUTPUT );
    digitalWrite( motorPoti[mp].pIN1, LOW );
    pinMode( motorPoti[mp].pIN2, OUTPUT );
    digitalWrite( motorPoti[mp].pIN2, LOW );
  }
  time_status=0;
  time_pause=0;
  SendMessage("Setup complete",1);
  MotorPoti_Zeroize(false);
  SendMessage("Zeroize complete",1);
}

void MotorPoti_FastUpdate()
{}

void SignalSenden(byte mp)
{
  //send current analog value to BMSAIT Win App
  char buf[9]="        ";
  sprintf (buf, "%03u,%04u", motorPoti[mp].command, motorPoti[mp].trimPos_int);
  SendMessage(buf,4);
  delay(5);
}

void CheckBMS(byte pos, byte mp)
{
  //conversion of the trim axis value from BMS (-0.5..0.5) into an analog axis value (0..1024)
  float newVal=atof(datenfeld[pos].wert);
  motorPoti[mp].trimPos_ext= map((newVal*1000),-500,500,1,1023);
  
  //check if the internal trim pos matches the position in BMS
  if (motorPoti[mp].trimPos_ext>(motorPoti[mp].trimPos_int+BUFFER))
  {
    //trim has been moved CW in BMS, position of the motorPoti needs to be adjusted
    motorPoti[mp].trimPos_int=motorPoti[mp].trimPos_ext;
    if (motorPoti[mp].dir) MotorMoveCW(mp); else MotorMoveCCW(mp);

    while(analogRead(motorPoti[mp].poti)<motorPoti[mp].trimPos_int)
      {delay(1);} //warten bis der Motor die richtige Position erreicht hat
    MotorStop(mp); 
  }
  else if (motorPoti[mp].trimPos_ext<(motorPoti[mp].trimPos_int-BUFFER))
  {
    //trim has been moved CCW in BMS, position of the motorPoti needs to be adjusted
    motorPoti[mp].trimPos_int=motorPoti[mp].trimPos_ext;
    if (motorPoti[mp].dir) MotorMoveCCW(mp); else MotorMoveCW(mp);
    
    while(analogRead(motorPoti[mp].poti)>motorPoti[mp].trimPos_int)
      {delay(1);} //wait for motor to move in the correct position
    MotorStop(mp);
  }
}

void CheckInternalMovement(byte mp)
{
  //Prüfen, ob der Motor bewegt wurde
  int trimPos_Motor=analogRead(motorPoti[mp].poti);
  
  if ((trimPos_Motor>(motorPoti[mp].trimPos_int+BUFFER)) || (trimPos_Motor<(motorPoti[mp].trimPos_int-BUFFER)))
  {
    //Poti got moved. Send new value to BMSAIT Win App
      //int difference=motorPoti[mp].trimPos_int-trimPos_Motor;
      //if (difference<0)difference=difference*(-1);
    
    motorPoti[mp].trimPos_int=trimPos_Motor;
    SignalSenden(mp);
    time_pause=(millis()+2000);  //pause check for BMS data if poti was recently moved
  }
}

void UpdateMotorPoti(byte pos) 
{
  long curr_time=millis();
  if (time_status<(curr_time-10000))
  {
    time_status=curr_time;
    if (error)
      {SendMessage("Motorpotis deaktiviert",1);}
    else if (debugmode)
    {
      char buf[9]="        ";
      sprintf (buf, "%03u,%04u", motorPoti[datenfeld[pos].target].command, motorPoti[datenfeld[pos].target].trimPos_int);
      SendMessage(buf,1);
    }
    else
    {}
  }
  
  if (!error) //only continue if no error occured
  {  
    //check external movement
    if ((curr_time>time_pause)&&(!debugmode))   //pause check for BMS data if poti was recently moved
      {//CheckBMS(pos,datenfeld[pos].target);
        } 

    //check internal movement
    CheckInternalMovement(datenfeld[pos].target);
  }
}
