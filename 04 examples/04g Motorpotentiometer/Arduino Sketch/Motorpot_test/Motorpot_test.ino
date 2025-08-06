// settings and functions to drive a motor driven potentiometer

#define DIR_DELAY 50 // brief delay for abrupt motor changes
#define BUFFER 10     // threshold that needs to be crossed to consider a change on the analog value as a movement

//function declaration
void MotorPoti_Zeroize();

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
		 {     5,     6,	  200,	  true,	  A1,  	500,	 500,    1}
    ,{     9,    10,    200,    true,   A0,   500,   500,    1}

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
    Serial.print("Motor ");
    Serial.print(motor);
    Serial.println(" initial move CW ok");
    //motor moved CW for 1.5 seconds. Now check if the poti readout increased
    tmp=analogRead(motorPoti[motor].poti);
    Serial.print("Pos after 1.5 sec movement: ");
    Serial.println(tmp);
    
    if (tmp>oldPos+BUFFER) 
      {
        motorPoti[motor].dir=true;
        return false;
      } 
    else if (tmp<oldPos-BUFFER) 
      {
        motorPoti[motor].dir=false;
        return false;
      } 
    else
    {  
      if (recursive) return true;
      
      //no good signal so far. Lets try the other direction
      if (CheckDirection(motor,false,true))   
        {return true;} //motor didn`t move or poti readout is faulty  
      else  
        {return false;} //motor recovered
    }
  }
  else
  {
    //current position is right of center, so check CCW movement
    MotorMoveCCW(motor);
    delay(1500);
    MotorStop(motor);
    Serial.print("Motor ");
    Serial.print(motor);
    Serial.println(" initial move CCW ok");  
    
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
      if (recursive) return true;
    
      //no good signal so far. Lets try the other direction
      if (CheckDirection(motor,true,true))   
        {
          return true;              
          Serial.print("Motor ");
          Serial.print(motor);
          Serial.println(" no good movement"); //motor didn`t move or poti readout is faulty  
        } 
      else  
      {
        return false;   //motor recovered in second try
      }
    }
  }
  return true;
}

void MotorPoti_Zeroize()
{
  error=false;
  byte result;
  for (byte mp=0;mp<motorPotiAnz;mp++)
  {
    if ((motorPoti[mp].pIN1==0) || (motorPoti[mp].pIN2==0)) continue;  //skip loop if no Pins were assigned
    
    int initialPos=analogRead(motorPoti[mp].poti);
    Serial.print("Initial Pos: ");
    Serial.println(initialPos);
    // check if the direction of motor movement and poti readout matches
    if (initialPos<=512)
      { error=CheckDirection(mp,true,false);}
    else
      { error=CheckDirection(mp,false,false);}
    Serial.print("Motor ");
    Serial.print(mp);
    Serial.println(" CheckDir complete");  
    if (!error)
    {
      unsigned short x=0;
      initialPos=analogRead(motorPoti[mp].poti);
      //move motor to center position
      if (initialPos>(512+BUFFER))
      {
        Serial.print("Zerorize initial Pos: ");
        Serial.println(initialPos);
        if (motorPoti[mp].dir) MotorMoveCCW(mp); else MotorMoveCW(mp);
        while((motorPoti[mp].trimPos_int>=512))
        {
          delay(1);
          motorPoti[mp].trimPos_int=analogRead(motorPoti[mp].poti);
          x++;
          if (x==1000)
          {
            Serial.print("Motor ");
            Serial.print(mp);
            Serial.print(" position ");
            Serial.println(motorPoti[mp].trimPos_int);
            x=0;
            if (motorPoti[mp].trimPos_int-512>motorPoti[mp].trimPos_ext) //check new distance to center. It's supposed to decrease
            {
              error=true;           //distance increased or stagnated
              motorPoti[mp].trimPos_int=512+BUFFER;
            }
            else
            {
              motorPoti[mp].trimPos_ext=motorPoti[mp].trimPos_int-512;
            }
          }
        }
        MotorStop(mp);
      }
      else if (initialPos<(512-BUFFER))
      {
        if (motorPoti[mp].dir) MotorMoveCW(mp); else MotorMoveCCW(mp);
        while(motorPoti[mp].trimPos_int<=512)
        {          
          delay(1);
          motorPoti[mp].trimPos_int=analogRead(motorPoti[mp].poti); //value is supposed to increase
          x++;
          if (x==1000)
          {
            Serial.print("Motor ");
            Serial.print(mp);
            Serial.print(" position ");
            Serial.println(motorPoti[mp].trimPos_int);
            x=0;
            if (512-motorPoti[mp].trimPos_int>motorPoti[mp].trimPos_ext) //check new distance to center. It's supposed to decrease
            {
              error=true;           //distance increased or stagnated
              motorPoti[mp].trimPos_int=512-BUFFER;
            }
            else
            {
              motorPoti[mp].trimPos_ext=512-motorPoti[mp].trimPos_int;
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
      Serial.println("Error: Motor setup failed");
    }
  }
}

void Spuelung()
{
  while (Serial.available()){Serial.read();}
}


void setup() 
{
  Serial.begin(57600);
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
  Serial.println("Setup complete");
  MotorPoti_Zeroize();
  Serial.println("Zeroize complete");
}

void ReadMessage()
{
  byte motID=0;
  unsigned int motPos =0;
  
  delay(2);
  if (Serial.available()<3)
  {
    Spuelung();
    return;
  }
  else
  {
    char c =Serial.read();
    if ((c-48)>=motorPotiAnz)
    {
      Spuelung();
      Serial.print("Error 2 (");
      Serial.print(c);
      Serial.println(")");
      return;
    }
    else
    {motID=c-48;}
    
    c =Serial.read();
    if (c==',')
    {
      motPos=Serial.parseInt();
      if ((motPos>0) && (motPos<1024))
      {
      Serial.print("Moving Trim ");
      Serial.print(motID);
      Serial.print(" in position ");
      Serial.println(motPos);
      motorPoti[motID].trimPos_ext=motPos;
      }
      else
      {
        Spuelung();
        Serial.println("Error 3");
        return;
      }
    }
    else
    {
      Spuelung();
      Serial.println("Error 4");
      return;
    }
    
    
  }  
}

void SignalSenden(byte mp)
{
  //send current analog value to BMSAIT Win App
  Serial.print("Motor ");
  Serial.print(mp);
  Serial.print(" manual pos ");
  Serial.println(motorPoti[mp].trimPos_int);
  delay(5);
}

void CheckExternal(byte mp)
{

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

void CheckInternal(byte mp)
{
  //Prüfen, ob der Motor bewegt wurde
  int trimPos_Motor=analogRead(motorPoti[mp].poti);
  
  if ((trimPos_Motor>(motorPoti[mp].trimPos_int+BUFFER)) || (trimPos_Motor<(motorPoti[mp].trimPos_int-BUFFER)))
  {  
    motorPoti[mp].trimPos_int=trimPos_Motor;
    SignalSenden(mp);
    motorPoti[mp].trimPos_ext=motorPoti[mp].trimPos_int;
    time_pause=(millis()+2000);  //pause check for BMS data if poti was recently moved
  }
}

void loop() 
{
  long curr_time=millis();
  
  if (!error) //only continue if no error occurred
  {  
    //check for debug message (set new trim pos)
    ReadMessage(); 
    for(byte x=0;x<motorPotiAnz;x++)
    {
      //check external movement
      CheckExternal(x);
      //check internal movement
      CheckInternal(x);
    }
  }
}
