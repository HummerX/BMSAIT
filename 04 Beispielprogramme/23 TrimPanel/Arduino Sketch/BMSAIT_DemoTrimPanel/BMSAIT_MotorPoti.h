// settings and functions to drive a motor driven potentiometer

#define DIR_DELAY 50 // brief delay for abrupt motor changes
#define BUFFER 10     // threshold that needs to be crossed to consider a change on the analog value as a movement

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
	  	//PIN1	PIN2	 pWM	  dir	   poti 	intP	 extP   command
		 {   9,    10,	 254,	  true,	  A0,  	500,	 500,    67} //Motor B // Roll
    ,{   6,     5,   254,   true,   A1,   500,   500,    66} //Motor A // Pitch
};
const byte motorPotiAnz = sizeof(motorPoti)/sizeof(motorPoti[0]); 

bool error=false;
bool reSet=false;
long time_status=0; //time for new status message
long time_pause_int=0; //pause when poti was moved
long time_pause_ext=0; //pause when BMS value changed


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

void MotorPoti_Zeroize()
{
  error=false;
  for (byte mp=0;mp<motorPotiAnz;mp++)
  {
    if ((motorPoti[mp].pIN1==0) || (motorPoti[mp].pIN2==0)) continue;  //skip loop if no Pins were assigned
  
    int oldPos=analogRead(motorPoti[mp].poti);
    int tmp=0;
    // check if the direction of motor movement and poti readout matches
    if (oldPos<=512)
    {
      //current position is left of center, so check CW movement
      MotorMoveCW(mp);
      delay(1000);
      MotorStop(mp);
    
      //motor moved CW for 1 second. Now check if the poti readout increased
      tmp=analogRead(motorPoti[mp].poti);
      if (tmp>oldPos+BUFFER) 
          {motorPoti[mp].dir=true;} 
      else if (tmp<oldPos-BUFFER) 
          {motorPoti[mp].dir=false;} 
      else
          {error=true;} //motor didn`t move or poti readout is faulty  
    }
    else
    {
      //current position is right of center, so check CCW movement
      MotorMoveCCW(mp);
      delay(1000);
      MotorStop(mp);
      
      //motor moved CCW for 1 second. Now check if the poti readout decreased
      tmp=analogRead(motorPoti[mp].poti);
      if (tmp>oldPos+BUFFER) 
          {motorPoti[mp].dir=false;}
      else if (tmp<oldPos-BUFFER) 
          {motorPoti[mp].dir=true;} 
      else
          {error=true;} //motor didn`t move or poti readout is faulty
    }
        
    if (!error)
    {
      //move motor to center position
      if (tmp>(512+BUFFER))
      {
        if (motorPoti[mp].dir) {MotorMoveCCW(mp);} else {MotorMoveCW(mp);}
        
        while((analogRead(motorPoti[mp].poti)>=512))
          {delay(1);}
        MotorStop(mp);
      }
      else if (tmp<(512-BUFFER))
      {
        if (motorPoti[mp].dir) {MotorMoveCW(mp);} else {MotorMoveCCW(mp);}
        
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

void SignalSenden(byte mp)
{
  //send current analog value to BMSAIT Win App
  char buf[9]="        ";
  sprintf (buf, "%03u,%04u", motorPoti[mp].command, motorPoti[mp].trimPos_int);
  SendMessage(buf,4);
  delay(10);
  if (testmode){SendMessage(buf,1);}
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
  time_status=0;  //memorize time till next position broadcast in testmode
  time_pause_int=0; //memorize time of last change
  time_pause_ext=0; //memorize time of last change
  MotorPoti_Zeroize();
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
      {delay(1);} //wait for motor to move in the correct position
    MotorStop(mp);
    
    time_pause_ext=(millis()+500);  //no checks for internal changes if BMS trim value recently changed
  }
  else if (motorPoti[mp].trimPos_ext<(motorPoti[mp].trimPos_int-BUFFER))
  {
    //trim has been moved CCW in BMS, position of the motorPoti needs to be adjusted
    motorPoti[mp].trimPos_int=motorPoti[mp].trimPos_ext;
    if (motorPoti[mp].dir) MotorMoveCCW(mp); else MotorMoveCW(mp);
    
    while(analogRead(motorPoti[mp].poti)>motorPoti[mp].trimPos_int)
      {delay(1);} //wait for motor to move in the correct position
    MotorStop(mp);

    time_pause_ext=(millis()+500);  //no checks for internal changes if BMS trim value recently changed
  }
}

void CheckInternalMovement(byte mp)     //check if pilot turned the trim knob
{ 
  int trimPos_Motor=analogRead(motorPoti[mp].poti);
   
  if ((trimPos_Motor>(motorPoti[mp].trimPos_int+BUFFER)) || (trimPos_Motor<(motorPoti[mp].trimPos_int-BUFFER)))
  {
    //trim knob got moved. Send new value to BMSAIT Win App 
    motorPoti[mp].trimPos_int=trimPos_Motor;
    SignalSenden(mp);
    time_pause_int=(millis()+1000);  //no checks for changes in BMS data if trim knob was recently moved
  }
}

void UpdateMotorPoti(byte pos) 
{
  long curr_time=millis();

  //mod
  if (reSet) //Center all trim knobs and anlog axis
  {
      for (byte mp=0;mp<motorPotiAnz;mp++)
      {
        memcpy(datenfeld[pos].wert, "0.0", sizeof("0.0"));
        CheckBMS(pos, mp);
        SignalSenden(mp);
      }
      reSet=false;
  }
  //mod
  
  if (time_status<(curr_time-10000))
  {
    time_status=curr_time;
    if (error)
      {SendMessage("Motorpotis deaktiviert",1);}
    else if (testmode)
    {
      char buf[9]="        ";
      sprintf (buf, "%03u,%04u", motorPoti[datenfeld[pos].ziel].command, motorPoti[datenfeld[pos].ziel].trimPos_int);
      SendMessage(buf,1);
    }
    else
    {}
  }
  
  if (!error) //only continue if no error occured
  {  
    //check external movement
    if ((curr_time>time_pause_int)&&(!testmode))   //pause check for BMS data if poti was moved recently 
      {CheckBMS(pos,datenfeld[pos].ziel);} 

    //check internal movement
    if (curr_time>time_pause_ext)
      {CheckInternalMovement(datenfeld[pos].ziel);}
  }
}
