//V1.3 19.11.2022
// settings and functions to drive a motor driven potentiometer

#define DIR_DELAY 50        // brief delay for abrupt motor changes
#define BUFFER 30           // threshold that needs to be crossed to consider a change on the analog value as a movement
#define MP_TIMECHECK 200    // time interval to check correct motor movements
#define MP_PAUSE 5000       // time to pause checks after movements

//function declaration
void MotorPoti_Zeroize(bool full);


typedef struct 
{
  byte pIN1;         //Pin1 of the Motor (movement order)
  byte pIN2;         //Pin2 of the Motor (movement direction)
  byte pWM;          //Motor Speed (0 to 255)
  bool dir;          //Direction - synchronize motor movement and poti readout
  byte poti;         //Analog pin for position readout
  bool inverted;     //Axis in BMS is set to inverted
  int trimPos_int;   //Internal memory for the current trim position (0..1024)
  int trimPos_ext;   //TrimPos as read from BMS (0..1024)
  int command;       //ID of the command line in BMS Windows App that links a vjoy analog axis
}  Struc_MotorPoti;


Struc_MotorPoti motorPoti[]=
{
     //  PIN1	 PIN2	  pWM	  dir	   poti  inverted    intP     extP   command
     {   2,       3,     254,    true,     A0,    false,     500,    500,     1} //Motor B // Roll
};
const byte motorPotiAnz = sizeof(motorPoti)/sizeof(motorPoti[0]); 


bool error=false;

#ifdef Switches
byte TrimResetButton=99;
#endif

long time_status=0; //time for new status message
long timePauseExt=0;  //pause when poti was moved
long timePauseInt=0;  //pause when BMS trim got moved
long time_motormove=0;  //time since start of motor movement


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
    motorPoti[mp].trimPos_int=512;
    motorPoti[mp].trimPos_ext=512;
  }
  time_status=0;
  timePauseInt=0;
  timePauseExt=0;
  SendMessage("Setup complete",1);
  MotorPoti_Zeroize(true);
  SendMessage("Zeroize complete",1);
  #ifdef Switches
    for (byte x=0;x<numSwitches;x++)
    {
      if (switches[x].intCommand==10) //search TrimReset button PIN
      { TrimResetButton=x; }
    }
  #endif
}

void MotorMoveCW(byte mp)
{
  // always stop motors briefly to prevent abrupt changes
  digitalWrite( motorPoti[mp].pIN1, LOW );
  digitalWrite( motorPoti[mp].pIN2, LOW );
  delay( DIR_DELAY );
  // set the motor speed and direction
  digitalWrite( motorPoti[mp].pIN1, LOW ); 
  analogWrite( motorPoti[mp].pIN2, motorPoti[mp].pWM );
}

void MotorMoveCCW(byte mp)
{
  
  // always stop motors briefly to prevent abrupt changes
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

//check if the motor and the pot work and are syncronized
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
    {
      //pot readout increased. motor and pot work in the same direction
      motorPoti[motor].dir=true;
      return false;
    } 
    else if (tmp<oldPos-BUFFER) 
    {
      //pot readout decreased. motor and pot work in the opposite direction
      motorPoti[motor].dir=false;
      return false;
    } 
    else
    {  
      if (recursive) 
        return true; //if this is the second attempt to check the sync, return an error.
      else
      {
        //no good signal so far. Lets try the other direction
        if (CheckDirection(motor,!richtung,true))   
          {return true;}  //motor didn`t move or poti readout is faulty 
        else  
          {return false;} //motor recovered 
      }
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
    {
      motorPoti[motor].dir=false;
      return false;
    }  
    else if (tmp<oldPos-BUFFER) 
    {
      motorPoti[motor].dir=true;
      return false;
    }
    else
    {
      if (recursive) 
        return true; //if this is the second attempt to check the sync, return an error.
      else
      {
        //no good signal so far. Lets try the other direction
        if (CheckDirection(motor,!richtung,true))   
          {return true;}  //motor didn`t move or poti readout is faulty 
        else  
          {return false;} //motor recovered 
      }
    }
  }
}

void SignalSenden(byte mp)
{
  //send current analog value to BMSAIT Win App
  char buf[9]="        ";
  sprintf (buf, "%03u,%04u", motorPoti[mp].command, motorPoti[mp].trimPos_int);
  SendMessage(buf,4);
  delay(5);
}

void MotorPoti_Zeroize(bool full)
{
  error=false;
  byte result;
  for (byte mp=0;mp<motorPotiAnz;mp++)
  {
    if ((motorPoti[mp].pIN1==0) || (motorPoti[mp].pIN2==0)) continue;  //skip loop if no Pins were assigned
    
    
    if (full) // full zeroize procedure including a sync check of motor and pot
    {
      int initialPos=analogRead(motorPoti[mp].poti);

      if (initialPos<=512)
        { error=CheckDirection(mp,true,false);}
      else
        { error=CheckDirection(mp,false,false);}
      
      if (debugmode) SendMessage("CheckDir complete",1);  
    }
    
    if (!error)
    {
      //move motor to center position
      motorPoti[mp].trimPos_int=analogRead(motorPoti[mp].poti);
      unsigned int wayToGo=abs(motorPoti[mp].trimPos_int-512); //determine distance to travel
      
      if (motorPoti[mp].trimPos_int>(512+BUFFER))  //motorpot is above center position
      {
        if (motorPoti[mp].dir) MotorMoveCCW(mp); else MotorMoveCW(mp); //start motor travel
        
        time_motormove=millis()+MP_TIMECHECK;
        while(motorPoti[mp].trimPos_int>=512)
        {
          motorPoti[mp].trimPos_int=analogRead(motorPoti[mp].poti);
          delay(1);
          if (millis()>time_motormove) 
          {
            if (abs(motorPoti[mp].trimPos_int-512)>=wayToGo)  //motor couldn't move or moved in the wrong direction
            {
              if (debugmode){SendMessage("Wrong direction or blocked",1);}
              break;
            }
            else
            {
              wayToGo=abs(motorPoti[mp].trimPos_int-512);
              time_motormove=millis()+MP_TIMECHECK;
            }
          }         
        }
        MotorStop(mp);
        motorPoti[mp].trimPos_int=analogRead(motorPoti[mp].poti);
        motorPoti[mp].trimPos_ext=motorPoti[mp].trimPos_int;
      }
      else if (motorPoti[mp].trimPos_int<(512-BUFFER)) //motorpot is below center position
      {
        if (motorPoti[mp].dir) MotorMoveCW(mp); else MotorMoveCCW(mp); //start motor travel
        time_motormove=millis()+MP_TIMECHECK;
        
        while(motorPoti[mp].trimPos_int<=512)
        {
          motorPoti[mp].trimPos_int=analogRead(motorPoti[mp].poti);
          delay(1);
          if (millis()>time_motormove) 
          {
            if (abs(motorPoti[mp].trimPos_int-512)>=wayToGo)  //motor couldn't move or did move in a wrong direction
            {
              //error=true;
              if (debugmode){SendMessage("Wrong direction or blocked",1);}
              break;
            }
            else
            {
              wayToGo=abs(motorPoti[mp].trimPos_int-512);
              time_motormove=millis()+MP_TIMECHECK;
            }
          }    
        }
        MotorStop(mp);
      }
      else
      {} //motor already is in center position
      
    }
    else 
    {
      SendMessage("MotorPot initialization failed",1);
    }
  }
}

void ReadNewValue(byte pos)
{
  byte mp=datenfeld[pos].target;
  //conversion of the trim axis value from BMS (-0.5..0.5) into an analog axis value (0..1024)
  float newVal=atof(datenfeld[pos].wert);
  motorPoti[mp].trimPos_ext= map((newVal*100),-50,50,1,1023);
  if (motorPoti[mp].inverted) motorPoti[mp].trimPos_ext=1024-motorPoti[mp].trimPos_ext; //if BMS axis is inverted, Arduino range needs to be inverted, too
}

void MP_SendFeedBack(byte mp)
{
  SERIALCOM.print(VAR_BEGIN);
  SERIALCOM.print('t');
  SERIALCOM.print("BMS: ");
  SERIALCOM.print(motorPoti[mp].trimPos_ext);
  SERIALCOM.println(VAR_ENDE);
  SERIALCOM.print(VAR_BEGIN);
  SERIALCOM.print('t');
  SERIALCOM.print("Intern: ");
  SERIALCOM.print(motorPoti[mp].trimPos_int);
  SERIALCOM.println(VAR_ENDE);  
}

void CheckExternalMovement(byte mp)
{

  //check if the internal trim pos matches the position in BMS
  if (abs(motorPoti[mp].trimPos_ext-motorPoti[mp].trimPos_int)>BUFFER)  
  {
    if (debugmode) {SendMessage("Extern",1);}
    if (debugmode) {MP_SendFeedBack(mp);}
    bool MotDirection=true;
    if (motorPoti[mp].trimPos_ext>motorPoti[mp].trimPos_int){MotDirection=false;}
    if (motorPoti[mp].dir){!MotDirection;}
    //trim has been moved CW in BMS, position of the motorPoti needs to be adjusted
    if (MotDirection) MotorMoveCW(mp); else MotorMoveCCW(mp);
      
    unsigned int wayToGo=abs(motorPoti[mp].trimPos_int - motorPoti[mp].trimPos_ext); //distance to travel
    time_motormove=millis()+MP_TIMECHECK;
    
    if (debugmode)
    {
      SERIALCOM.print(VAR_BEGIN);
      SERIALCOM.print('t');
      SERIALCOM.print(mp);
      SERIALCOM.print("WTG: ");
      SERIALCOM.print(wayToGo);
      SERIALCOM.println(VAR_ENDE);   
      MP_SendFeedBack(mp);
    }
       
    while(abs(motorPoti[mp].trimPos_int-motorPoti[mp].trimPos_ext)>0 ) //PRÜFEN! Reicht es aus bis zum Buffer zu gehen oder soll man auf 0 runter?
    {
      delay(1);
      motorPoti[mp].trimPos_int=analogRead(motorPoti[mp].poti);
      
      if (millis()>time_motormove)
      {
        if (wayToGo<=abs(motorPoti[mp].trimPos_int-motorPoti[mp].trimPos_ext))  //distance to travel stopped decreasing->error 
        {
          if (debugmode)
          {
            SERIALCOM.print(VAR_BEGIN);
            SERIALCOM.print('t');
            SERIALCOM.print(mp);
            SERIALCOM.print("Runaway!");
            SERIALCOM.println(VAR_ENDE);   
            MP_SendFeedBack(mp);
          }
          
          break;
        }
        time_motormove=millis()+MP_TIMECHECK; //set time for next check
        wayToGo=abs(motorPoti[mp].trimPos_int-motorPoti[mp].trimPos_ext);  //memorize current distance to travel
      }
    } //warten bis der Motor die richtige Position erreicht hat
    
    MotorStop(mp); 
    motorPoti[mp].trimPos_int=analogRead(motorPoti[mp].poti);
    motorPoti[mp].trimPos_int=motorPoti[mp].trimPos_ext; //sync int and ext values to prepare next check loop
    timePauseInt=(millis()+MP_PAUSE);  //pause check for manual trim movement after BMS just got processed
  }
}

void CheckInternalMovement(byte mp)
{
  //Prüfen, ob der Motor bewegt wurde
  delay(1);
  int trimPos_Motor=analogRead(motorPoti[mp].poti);
  
  if (abs(trimPos_Motor-motorPoti[mp].trimPos_int)>BUFFER)
  {
    motorPoti[mp].trimPos_int=trimPos_Motor;
    if (debugmode) {SendMessage("Intern",1);}
    if (debugmode) {MP_SendFeedBack(mp);}
    
    //Poti got moved. Send new value to BMSAIT Win App  
    SignalSenden(mp);
    timePauseExt=(millis()+MP_PAUSE);  //pause check for BMS data if poti was recently moved
    motorPoti[mp].trimPos_ext=motorPoti[mp].trimPos_int;
  }
}

//reset Trim internal/external pos to center
void TrimReset(byte pos)
{
  motorPoti[pos].trimPos_int=512;
  SignalSenden(pos);
  motorPoti[pos].trimPos_ext=motorPoti[pos].trimPos_int;
  timePauseExt=(millis()+MP_PAUSE);  //pause check for BMS data if poti was recently moved
}

void MotorPoti_FastUpdate()
{}

void UpdateMotorPoti(byte pos) 
{
  long curr_time=millis();
  if (time_status<(curr_time-10000)) //status check every 10 Seconds
  {
    time_status=curr_time;
    if (error)
      {SendMessage("Motorpotis deaktiviert",1);}
    
    if (debugmode)
      {MP_SendFeedBack(pos);}
  }
  
  if (!error) //only continue if no error occured
  { 
    #ifdef Switches
      if (TrimResetButton!=99)
      {
        if (digitalRead(switches[TrimResetButton].pIN)==LOW)
        {
          for (byte x=0;x<motorPotiAnz;x++)
          { TrimReset(x);}  
        }
      }
    #endif
    
    if (curr_time>timePauseExt)   //pause check for BMS data if poti was recently moved
    {
      //check external movement
      ReadNewValue(pos);
      CheckExternalMovement(datenfeld[pos].target);
    }
     
    if (curr_time>timePauseInt)
    { 
      //check internal movement
      CheckInternalMovement(datenfeld[pos].target);
   }
  }
}
