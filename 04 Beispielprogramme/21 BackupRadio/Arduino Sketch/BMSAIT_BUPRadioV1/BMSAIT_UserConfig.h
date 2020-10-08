//device definitions 
//uncomment the modules you want to use.
// Version: 1.0    3.10.20

//#define LED               //drive LEDs
//#define LCD               //drive LCD display
#define SSegMAX7219       //drive 7-Segment displays via MAX7219 controller
//#define SSegTM1637        //drive 7-Segment displays via TM1367 controller
//#define ServoMotor        //drive servo motors directly connected to the arduino
//#define ServoPWM          //drive multiple servo motors via pwm shield
//#define StepperBYJ        //drive stepper motor 28BYJ-48
//#define StepperX27        //drive stepper motor X27.168
//#define StepperVID        //drive multiple stepper motors X25.168 with a VID66-06 controller
//#define MotorPoti         //motor-driven poti control
#define Switches          //use the arduino to read switch positions and send keyboard commands
//#define ButtonMatrix      //use the arduino to read switch positions and send keyboard commands
//#define RotEncoder        //use the arduino to read rotary encoders and send keyboard commands
//#define AnalogAxis        //use the arduino to read analog resistors and sync this with a gamecontroller axis
//#define NewDevice         //placeholder. Use this line to activate your own code to drive other, specific hardware

//mod
#define BUPRadio           //activate special procedures to drive the Backup Radio Panel
//mod

//End if device definitions

//Set the ID for this arduino program. Use any string. The program will use this ID to check in with the BMSAIT windows application
  const char ID[]= "BMSAIT_BUPRadio";      

//Basic program definitions

  #define POLLTIME 200        // set time between PULL data requests

  
// This is the most important part of this sketch. You need to set the data that the Arduino will have to handle
// Make sure that you chose the definition of VARIABLENANZAHL matches the number of entries in this table
// Fill data as follows: 
// 1. Short description (max 5 characters)
// 2. Flightdata ID (check BMSAIT Variablen.csv)  as a 4-digit string (needed for PULL mode)
// 3. data type (check BMSAIT Variablen.csv: b=bool, y=byte, 1=byte[], i=integer, f=float, s=string, 2=string[]))  (needed for PULL mode)
// 4. Output type (10-LED, 20-LCD, 30-MAX7219, 31-TM1637, 40-Servo, 41-ServoPWM, 50-Stepper 28BYJ, 51-Stepper X27-168, 52-StepperBoard VID6606, 60-MotorPoti)
// 5. target PIN (used to define output PIN of LEDs, the line on LCD displays or a link to an entry in container in motor modules)
// 6. number of characters to display (control display length on 7-segment or LCD displays)
// 7. start position (control position of data on 7-segment or LCD displays) 
// 8. decimal point (will add a decimal point on 7-segment displays after the given position
// 9. Initial value as string (i.e. "00")

Datenfeld datenfeld[]=
  {
   //Description  ID     DT    OT  PIN  C   SP  DP  IV
   {  "UHFCH",  "1130", 's',  30,   0,  2,  6,  99, "99"}       //Variable 0 - UHF Preset
  ,{  "UHFFQ",  "1120", 's',  30,   0,  6,  0,   3, "123456"}   //Variable 1 - UHF Frequenz
  ,{  "PWRST",  "1260", 'i',   0,   0,  0,  0,   0, "1"}        //Variable 2 - Status Power
  };  
const int VARIABLENANZAHL = sizeof(datenfeld)/sizeof(datenfeld[0]); 
