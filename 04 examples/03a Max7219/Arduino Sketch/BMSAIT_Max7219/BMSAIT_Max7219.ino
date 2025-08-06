// Arduino sketch to send/receive data from the Falcon BMS Shared Memory via the BMS-Arduino Interface Tool and control devices in home cockpits
// Version: 1.3.15   18.4.25
// Robin "Hummer" Bruns


//basic settings
  #include <Arduino.h>

  #define DATENLAENGE 8       // maximum length of a data set
  #define MESSAGEBEGIN 255    // this byte marks the beginning of a new message from the Windows app
  #define HANDSHAKE 128       // this byte marks an identification request from the Windows app
  #define SWITCHPOSITION 150  // this byte marks a request to send current switch positions
  #define CALIBRATE 160       // this byte marks a request to reset motors to initial position
  #define ZEROIZE   161       // this byte marks a request to fast zeroize motors 
  #define STARTPULL 170       // this byte marks a request to start the PULL logic on the arduino
  #define ENDPULL 180         // this byte marks a request to end the PULL logic on the arduino
  #define TESTON  190         // activates readback of data variables
  #define READBACKON 191      // activates testmode with readback of data variables 
  #define TESTOFF 200         // deactivates testmode
  #define VAR_BEGIN '<'       // char to mark the begin of the actual data
  #define VAR_ENDE '>'        // char to mark the end of the actual data
  #define TYP_ANFANG '{'      // char to mark the begin of the type definition in a message string
  #define TYP_ENDE '}'        // char to mark the begin of the type definition in a message string
  
//Function headers
  void ReadData();
  void UpdateInput(bool all);
  void UpdateOutput();
  void PullRequest(byte v);
  void ReadResponse();     
  void DebugReadback(byte posID);
  void SendSysCommand(const char text[]); 
  void SendMessage(const char message[], byte option);
  void ResetMotors();
  void FastUpdate();
  bool CheckForSysCommand(byte SysCmd);
  
//Struct definitions
  typedef struct //data field structure for storage of data variables
  {
    char bezeichnung[6];            //short description (max. 5 characters)
    char ID[5];                     //only required for PULL-operation. The ID for this data from "BMSAIT-Variablen.csv". this will tell the windows app what kind of data you are requesting
    char format;                    //only required for PULL-operation. Data type according to  "BMSAIT-Variablen.csv" (y=byte, i=integer, f=float, s=string, b=bool, 1=Byte Array, 2=Int Array, 3=string Array, 4=float Array) 
    byte typ;                       //indicates the type of device that will be used to output his data (10: LED, 20: LCD, 30: 7-Segment MAX7219, 40: Servomotor)
    byte target;                    //this byte links the variable to a specific device (i.e. PIN for LED output, the address for motors on a pwm shield or to differentiate between multiple devices within one module)
    byte ref2;                      //the use of reference byte depends on the output type. Ref2 can set the line on LCD displays
    byte ref3;                      //the use of reference byte depends on the output type. Ref3 can set the length if a data value to display on LCD or 7-segment displays
    byte ref4;                      //the use of reference byte depends on the output type. Ref4 can set the position of data on LCD / 7Segment displays. if set, the data will be offset by this number of characters
    byte ref5;                      //the use of reference byte depends on the output type. Ref5 can mark the position of the decimal point on 7-Segment displays
    char request[10];               //precompiled pull request message
    char wert[DATENLAENGE];         //contains the current value
  } Datenfeld;

  typedef struct              //container to store incoming data from the Windows app
  {
    byte varNr;                     //position for this data in the local file container 
    char typ;                       //file format of this data 
    char wert[DATENLAENGE];         //data
  } Uebertragung;
  Uebertragung neuer_wert;

//Load user settings
  #include "BMSAIT_UserConfig.h"   //load the user settings 

//Global variables
  byte inputByte_0;           //container for incoming serial message
  byte inputByte_1;           //container for incoming serial message
  byte state =0;              //marker to memorize the current position in a message string
  byte Uebertragung_pos=0;    //counts how many chars have already been read from an incoming data stram. Used to prevent an overflow of data variables. 
  bool debugmode=false;        //debugmode on/off
  bool readbackmode=false;        //Readbackmode on/off
  unsigned long lastInput =0; //last successful transmission
  
  #ifdef DUE_NATIVE
    #define SERIALCOM SerialUSB   //enable communication over the native port of the DUE
  #else
    #define SERIALCOM Serial      //standard serial connection
  #endif
  
//settings for PULL logic
  bool pull= false;           //flag that defines wether to use push or pull logic (true=pull ; false=push). Can be switched by commands of the F4SMExporter App
  unsigned long lastPoll=0;            //timestamp of the last data request during pull logic
  byte last_request=99;        //marker for the last variable that was requested via pull logic in the last loop 
//settings for PULL logic


//switch and signal settings
#ifdef Switches                
  #include "BMSAIT_Switches.h" 
#endif  
 
 //magnetic held switches
#ifdef MagSwitch                
  #include "BMSAIT_MagSwitch.h" 
#endif  

//button matrix settings
#ifdef ButtonMatrix               
  #include "BMSAIT_Buttonmatrix.h"
#endif  

//rotary encoders
#ifdef RotEncoder               
  #include "BMSAIT_Encoder.h"
#endif  

//analog axis settings
#ifdef AnalogAxis 
  #include "BMSAIT_Analogachse.h"
#endif 

//LED settings
#ifdef LED                             
  #include "BMSAIT_LED.h"
#endif

//LED matrix settings
#ifdef LEDMatrix                             
  #include "BMSAIT_LEDMatrix.h"
#endif

//LCD settings
#ifdef LCD                              
  #include "BMSAIT_LCD.h"
#endif    

//7Segment settings MAX 7219 tube
#ifdef SSegMAX7219                              
  #include "BMSAIT_SSegMAX7219.h"
#endif                                

//7Segment settings TM1637 tube
#ifdef SSegTM1637                            
  #include "BMSAIT_SSegTM1637.h"
#endif  

//DotMatrix settings SLx2016 tube
#ifdef SLx2016                            
  #include "BMSAIT_SLx2016.h"
#endif  

//Servo settings
#ifdef ServoMotor                       
  #include "BMSAIT_Servo.h"               
#endif                            

//PWM Shield for Servos settings
#ifdef ServoPWM                       
  #include "BMSAIT_ServoPWMShield.h"               
#endif  

//Steppermotor settings
#ifdef StepperBYJ                  
  #include "BMSAIT_Stepper28BYJ48.h"
#endif                                

//Steppermotor settings
#ifdef StepperX27                 
  #include "BMSAIT_StepperX27.h"
#endif  

//Steppermotor ControllerBoard settings
#ifdef StepperVID                   
  #include "BMSAIT_StepperVID.h"
#endif  

//Compass setting
#ifdef CompassX27                  
  #include "BMSAIT_CompassX27.h"
#endif  

//motor poti settings
#ifdef MotorPoti 
  #include "BMSAIT_MotorPoti.h"
#endif 

#ifdef OLED
  #include "BMSAIT_OLED.h"
#endif

#ifdef SpeedBrake                             
  #include "BMSAIT_SBI.h"
#endif

#ifdef FuelFlowIndicator
  #include "BMSAIT_FF.h"
#endif

#ifdef DED_PFL                             
  #include "BMSAIT_DED_PFL.h"
#endif

#ifdef Lighting                             
  #include "BMSAIT_Lighting.h"
#endif

//example how to add your own project to this sketch. 
#ifdef NewDevice                
  #include "BMSAIT_Placeholder.h"
#endif  


//********************
//Main Program
//********************

///Setup of BMSAIT
void setup()
{
  SERIALCOM.begin(BAUDRATE);
                                      
  #ifdef LED                            //LEDs setup begin
    SetupLED();
  #endif                                //LEDs setup end
  
  #ifdef LEDMatrix                      //LEDMatrix setup begin
    SetupLEDMatrix();
  #endif                                //LEDMatrix setup end
 
  #ifdef LCD                            //LCD setup begin
   SetupLCD();
  #endif                                //LCD setup end
  
  #ifdef SSegMAX7219                    //7-segment-display MAX7219 setup begin
   SetupMax7219();
  #endif                                //7-segment-display MAX7219 setup end

  #ifdef SSegTM1637                     //7-segment-display TM1637 setup begin
   SetupTM1637();
  #endif                                //7-segment-display TM1637 setup end

  #ifdef SLx2016                        //Dotmatrix-display SLx2016 setup begin
   SetupSLx2016();
  #endif                                //Dotmatrix-display SLx2016 setup end
                              
  #ifdef ServoMotor                     //servo setup begin
   SetupServo();
  #endif                                //servo setup end
  
  #ifdef ServoPWM                       //servo PWM shield setup begin
    SetupServoPWM();          
  #endif                                //servo PWM shield setup end
    
  #ifdef StepperBYJ                     //stepper setup begin
   //SetupStepperBYJ();
  #endif                                //stepper setup end

  #ifdef StepperX27                     //stepper setup begin
   SetupStepperX27();
  #endif                                //stepper setup end
  
  #ifdef StepperVID                     //stepper on controller board setup begin
   SetupStepperVID();
  #endif                                //stepper setup end

  #ifdef CompassX27                     //Compass setup start
   SetupCompassX27();
  #endif                                //Compass setup end

  #ifdef MotorPoti                      //MotorPoti setup begin
    SetupMotorPoti();
  #endif                                //MotorPoti setup end
  
  #ifdef OLED                           //OLED setup begin
    SetupOLED();
  #endif                                //OLED setup end
  
  #ifdef SpeedBrake                     //SBI setup begin
    SetupSBI();
  #endif                                //SBI setup end
  
  #ifdef FuelFlowIndicator              //FFI setup begin
   SetupFFI();
  #endif                                //FFI setup end
  
  #ifdef DED_PFL                        //DED setup begin
    SetupDED();
  #endif                                //DED setup end

  #ifdef Switches                       //Input controller setup begin
   SetupSwitches();
  #endif                                //Input controller setup end
  
  #ifdef MagSwitch                       //Input controller setup begin
   SetupMagSwitch();
  #endif                                //Input controller setup end  
    
  #ifdef ButtonMatrix                   //Buttonmatrix setup begin
   SetupButtonMatrix();
  #endif                                //Buttonmatrix setup end
  
  #ifdef RotEncoder                     //Rotary encoder setup begin
   SetupEncoder();
  #endif                                //Rotary encoder setup end

  #ifdef AnalogAxis                     //Analog axis setup begin
   SetupAnalog();
  #endif                                //Analog axis setup end  

  #ifdef Lighting                       //Lighting axis setup begin     
   SetupLighting();
  #endif                                //Lighting axis setup end 

  #ifdef NewDevice                      //Example. Use this placeholder to activate your own projects
    SetupNewDevice();
  #endif                                //Example end

 //precompile pull messages
  char pos[2]={0,0};
  char nachricht[10]={0};
  for (byte var=0;var<variableCount;var++)
  {                                          
    itoa(var,pos,10);  //write data container position as character
    if (var<10) 
    {
      nachricht[0]='0'; 
      nachricht[1]=pos[0];
    }
    else 
    {
      nachricht[0]=pos[0]; 
      nachricht[1]=pos[1];
    }
    nachricht[2]=',';
    nachricht[3]=datenfeld[var].format;  //add the variable type
    nachricht[4]=',';
    for (byte lauf=0;lauf<4;lauf++)
      {nachricht[5+lauf]=datenfeld[var].ID[lauf];} //add the variable ID
    nachricht[9]='\0';
    strcpy(datenfeld[var].request,nachricht);
  }
}

/// Main loop
void loop()              
{
  FastUpdate();
  delay(1);
  FastUpdate();
  ReadData();  //get new data from PC
  FastUpdate();
  UpdateInput(false); //check input devices (switches, buttons)
  FastUpdate();
  UpdateOutput(); //update outputs (LEDs, motors, displays)
  FastUpdate();
}


//********************
//Functions
//********************

/// reset motor positions to default values
void ResetMotors(bool full)
{
#ifdef ServoMotor                       
  Servo_Zeroize(full);            
#endif                            

#ifdef ServoPWM                       
  ServoPWM_Zeroize(full);              
#endif  

#ifdef StepperVID
  StepperVID_Zeroize(full);   
#endif
 
#ifdef StepperX27
  StepperX27_Zeroize(full);  
#endif
 
#ifdef Stepper28BYJ48
  Stepper28BYJ48_Zeroize(full);  
#endif

#ifdef CompassX27
  CompassX27_Zeroize(full);
#endif 

#ifdef MotorPoti
  MotorPoti_Zeroize(full); 
#endif
}

///Call for another update of the motors to allow for fast, fluent movement
void FastUpdate()
{
#ifdef ServoMotor                       
  Servo_FastUpdate();            
#endif                            

#ifdef ServoPWM                       
  ServoPWM_FastUpdate();              
#endif 

#ifdef StepperVID
  StepperVID_FastUpdate();   
#endif
 
#ifdef StepperX27
  StepperX27_FastUpdate();  
#endif
 
#ifdef Stepper28BYJ48
  Stepper28BYJ48_FastUpdate();  
#endif 

#ifdef CompassX27
  CompassX27_FastUpdate();
#endif      

#ifdef MotorPoti
  MotorPoti_FastUpdate(); 
#endif 

#ifdef MagSwitch
  MagSwitch_FastUpdate(); 
#endif  
}

///check for fresh sharedMem data 
void ReadData()
{
  if (pull)  //send data requests if PULL mode is active
  {
    if (millis()-POLLTIME>lastPoll) //reduce the time between data request to prevent spamming (default POLLTIME 200 --> max of 5 attempts per second)
    {
      for (byte v=0;v<variableCount;v++)  //request update of each data variable
        {PullRequest(v);}
      lastPoll=millis();
    }
  }   
  else  
  {
    while(SERIALCOM.available())
    { 
      #ifdef PRIORITIZE_OUTPUT
      UpdateOutput();   //throw in another update if outputs are priorized 
      #endif
      #ifdef PRIORITIZE_INPUT
      UpdateInput(false);   //throw in another update if inputs are priorized 
      #endif
      ReadResponse();   //check for new data from the windows app
    }   
    if (millis()-POLLTIME>lastPoll) //reduce the number of attempts to get new data (default POLLTIME 200 --> max of 5 attempts per second)
    {
      SendMessage("",5); // reqest new data
      lastPoll=millis();
    }       
  }
}


///loop through all data variables and update output devices
void UpdateOutput()
{
  #ifdef DED_PFL  
    UpdateDED();  
  #endif
  
  #ifdef MagSwitch  
    UpdateMagSwitches();            
  #endif 					
 for (byte x=0;x<variableCount;x++)     //loop through the data container. For each entry, the respective output will be updated
  {
    switch (datenfeld[x].typ)
    {
      #ifdef LED
        case 10: //LED (PIN is wired to GND)
          UpdateLED(x, true); 
          break;
        case 11: //LED (PIN is wired to Vcc)
          UpdateLED(x, false); 
          break;
      #endif 
            
      #ifdef LEDMatrix
        case 12: //LEDMatrix
          UpdateLEDMatrix(x); 
          break;
      #endif    
      
      #ifdef LCD
        case 20: //LCD
          Update_LCD(x);
          break;
      #endif
        
      #ifdef SSegMAX7219 
        case 30:  //7-Segment display Max7219
          UpdateMAX7219(x);
          break;
      #endif
      
      #ifdef SSegTM1637
        case 31:   //7-Segment display TM1637
          UpdateTM1637(x);
          break;
      #endif

      #ifdef SLx2016
        case 32:   //matrix display SLx2016
          UpdateSLx2016(x);
          break;
      #endif
      
      #ifdef ServoMotor
        case 40: //standard Servos (i.e. SG90)
          UpdateServo(x);
          break;
      #endif
      
      #ifdef ServoPWM  
        case 41: //Servo on PWM Shield
          UpdateServoPWM(x);
          break;
      #endif
      
      #ifdef StepperBYJ 
        case 50: //Stepper Motor
          UpdateStepperBYJ(x);
          break;
      #endif    
   
      #ifdef StepperX27
        case 51: //X27.168 directly connected to the arduino          
          UpdateStepperX27(x);
          break;
      #endif
    
      #ifdef StepperVID
        case 52: //one to four Servos on a VID6606 board
          UpdateStepperVID(x);
          break;
      #endif

      #ifdef CompassX27
        case 53: //compass driven by X-class stepper motor
          UpdateCompassX27(x);
          break;
      #endif
      
      #ifdef MotorPoti
        case 60: //MotorPoti
          UpdateMotorPoti(x);
          break;      
      #endif

      #ifdef OLED
        case 70: //generic OLED
          UpdateOLED(x);
          break;      
      #endif
      
      #ifdef SpeedBrake
        case 71: //Speedbrake OLED
          UpdateSBI(x);
          break;      
      #endif
      
      #ifdef FuelFlowIndicator
        case 72: //FFI OLED
          UpdateFFI(x);
          break;      
      #endif

      #ifdef Lighting
        case 80: //Backlighting
          UpdateLighting(x);
          break;      
      #endif
      
      #ifdef NewDevice  //define this flag in the top of BMSAIT_UserConfig.h to activate this block ("#define newDevice")
        case 69: //assign this type to a variable in the data container to call a new method
          UpdateNewDevice(x);  //program a new method void Update_newDevice(int p){command1;command2;...}to enable your device 
        break;
      #endif

      default:
        //not implemented
        break;
    }
  }  
}


void UpdateInput(bool all)
{
  #ifdef Switches              
   CheckSwitches(all);            //check switch positions and initiate commands if switches were moved
  #endif
                                 
  #ifdef ButtonMatrix         
   ButtonMatrixRead();         //check button matrix for changes and initiate commands if switches were moved 
  #endif
  
  #ifdef RotEncoder              
   CheckEncoder();            //check encoders and initiate commands if they were moved
  #endif
  
  #ifdef AnalogAxis      
   ReadAnalogAxis(all);           //check analog axis for changes and initiate commands if switches were moved
  #endif
}


///Send a message to the BMSAIT App to request an update for a data variable
void PullRequest(byte var)
{
  if(SERIALCOM.available())  //make sure input buffer is empty   
  {
    delay(1);
    FastUpdate();
    ReadResponse();
  }
    
    SendMessage(datenfeld[var].request,2);
    
    byte x=0;
    while ((SERIALCOM.available()<3) && (x<PULLTIMEOUT)) //wait for answer, but no longer than 30ms
    {
      #ifdef PRIORITIZE_OUTPUT
        UpdateOutput(); //throw in another update if outputs are priorized
        x+=10;
      #endif
      #ifdef PRIORITIZE_INPUT
        UpdateInput();   //throw in another update if inputs are priorized
        x+=10;
      #endif
      delay(1);
      FastUpdate();
      x++;  
    }
    while(SERIALCOM.available()>1)  //read incoming data     
    {
      delay(1);
      FastUpdate();
      ReadResponse();
    }
}


void Reset()
{
  while (SERIALCOM.available()){SERIALCOM.read();}
  SendMessage("",5); // reqest new data
  state=0;
}


bool CheckForSysCommand(byte SysCmd)
{
  switch (SysCmd)
  {
    case HANDSHAKE:
      {
        SERIALCOM.flush();
        SendSysCommand(ID); //Send ID to identify this board
        break;
      }
    case STARTPULL:
      {
        //confirm start of PULL requests
        pull=true;
        SERIALCOM.flush();
        SendSysCommand("on");
        break;
      }  
    case ENDPULL:
      {
        //confirm termination of PULL requests
        pull=false;
        SERIALCOM.flush();
        SendSysCommand("off");
        break;
      } 
    case CALIBRATE:
      {
        //reset motor position to zero
        SERIALCOM.flush();
        SendSysCommand("ok");
        ResetMotors(true);
        break;
      }    
    case ZEROIZE:
        {
        //reset motor position to zero
        SERIALCOM.flush();
        SendSysCommand("ok");
        ResetMotors(false);
        break;
      }        
    case TESTON:
      {
        //confirm debugmode
        debugmode=true;
        SERIALCOM.flush();
        SendSysCommand("on");
        break;
      } 
    case READBACKON:
      {
        //confirm readbackmode
        readbackmode=true;
        SERIALCOM.flush();
        SendSysCommand("on");
        break;
      }   
    case TESTOFF:
      {
        //confirm end of debugmode
        debugmode=false;
        readbackmode=false;
        SERIALCOM.flush();
        SendSysCommand("off");
        break;
      }
    case SWITCHPOSITION:
      {
        //send current switch positions
        SERIALCOM.flush();
        UpdateInput(true);
        break;
      }
    default:
      {
        return false;
      }
  }
  Reset();
  return true;
}

/// Check incoming serial data for data. Expects structured messages --> CommandBit VariableID {VariableType}<Data>
void ReadResponse()       
{
  if (!SERIALCOM.available())
  {}
  else
  {
    lastPoll=0;
    if (state==0)
    {
      while (SERIALCOM.available() && (state==0))
      {
        inputByte_0=SERIALCOM.read();
        if (inputByte_0 == MESSAGEBEGIN) //Check for start of Message - byte (255)
         {state=1;}
        FastUpdate();
      }
    }
     
    if ((state==1) && SERIALCOM.available())
    {
      inputByte_1=SERIALCOM.read();
      if (!CheckForSysCommand(inputByte_1))  //check if a system command was recieved. If not, continue to check if valid data was recieved
      {
        if (((int)inputByte_1 < variableCount) || ((int)inputByte_1 >100)) //check if ID is valid
        {
          neuer_wert.varNr=(int)inputByte_1;
          state=2;
          FastUpdate();
        }
        else 
        { 
          state=0;    //unexpected value. discard data and start over.  
          if (debugmode){SendMessage("Fehler State 1",1);}
        }
      }
    }  
       
    if (state==2) 
    {
      if (!SERIALCOM.available())
      {
        delay(1);
        FastUpdate();
        if (!SERIALCOM.available()) 
        {state=0;}
      }
      else
      { 
        if (SERIALCOM.read()==VAR_BEGIN) //if the message is in the correct format, the defined char VAR_BEGIN can be found here
        {
          Uebertragung_pos=0;
          neuer_wert.wert[0]='\0';
          state=3;
        }
        else
        {
          if (debugmode){SendMessage("Fehler State 2",1);}
          state=0; //unexpected value. discard data and start over.
        }  
      }
    }
      
    if (state==3)
    {
      if (!SERIALCOM.available())
      {
        delay(1);
        FastUpdate();
        if (!SERIALCOM.available())
          {state=0;}
      }
      else
      {
        while (SERIALCOM.available() && (state==3))
        {
          byte c = SERIALCOM.read();
          FastUpdate();
          if (c==VAR_ENDE)  //the termination character arrived. Save the data.
          {
            // end of data found. Validate the buffer before writing the new data into the data container
        
            int laenge=sizeof(neuer_wert.wert);            
            if (neuer_wert.varNr<100)     //only compute the data if a valid data position is found (everything above 99 is invalid)
            {
              if (strcmp(datenfeld[neuer_wert.varNr].wert, neuer_wert.wert)!=0)  //check if the recieved data is different from the stored data
              {
                for (int lauf=DATENLAENGE-1;lauf>laenge;lauf--)
                  {datenfeld[neuer_wert.varNr].wert[lauf]='\0';}
                memcpy(datenfeld[neuer_wert.varNr].wert, neuer_wert.wert, sizeof(neuer_wert.wert)); //write the new data into the data container
              }
              lastInput=millis(); //store last transmission time
              if (readbackmode){DebugReadback(neuer_wert.varNr);}
            }  
            state=0;
          }
          else if (Uebertragung_pos>DATENLAENGE)  //end of variable missed. discard and start over.
          { 
            if (debugmode){SendMessage("Fehler State 3.1",1);}       
            state=0;
          }
          else if (c==MESSAGEBEGIN)    //end of variable missed. discard and start over.
          {
            if (debugmode){SendMessage("Fehler State 3.2",1);}
            state=1;
          }
          else                
          {
            //end of data not yet found. Add the current character to the buffer.
            neuer_wert.wert[Uebertragung_pos]=c;
            neuer_wert.wert[Uebertragung_pos + 1]='\0';
            Uebertragung_pos++;
          }
        }
      }
    }
  }
}

//readback of recieved data for verification
void DebugReadback(byte posID)
{
  char antwort[DATENLAENGE+3]="";
  char pos[3]="";
  itoa(posID,pos,10);
  if (posID<10)
  {
    antwort[0]=pos[0];
    antwort[1]=' ';
    antwort[2]=' ';
  }
  else
  {
    antwort[0]=pos[0];
    antwort[1]=pos[1];
    antwort[2]=' ';
  }
  for (int lauf=0;lauf<DATENLAENGE;lauf++)
    {antwort[lauf+3]=datenfeld[posID].wert[lauf];}
  antwort[DATENLAENGE+2]='\0';
  SendMessage(antwort,1);
}

///send a system command to the BMSAIT App 
void SendSysCommand(const char text[])  
{ 
  SERIALCOM.println(text);
  delay(5);
}

///Send a message to the BMSAIT App
void SendMessage(const char message[], byte option)
{
  SERIALCOM.print(VAR_BEGIN);
  switch (option)
  {
    case 1://send a text message
      {SERIALCOM.print('t'); break;}
    case 2: //send a data request
      {SERIALCOM.print('d');break;}
    case 3: //send a command
      {SERIALCOM.print('k');break;}
    case 4: //sends analog axis data value
      {SERIALCOM.print('a');break;}
    case 5: //report empty input buffer
      {SERIALCOM.print('g');break;}  
    case 6: //request DED/PFL Data
      {SERIALCOM.print('u');break;}  
    case 7: //send an internal command to BMSAIT App
      {SERIALCOM.print('s');break;}                        
    default:
      {return;/*do nothing*/}
  }
  SERIALCOM.print(message) ;
  SERIALCOM.println(VAR_ENDE);
}
