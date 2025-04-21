// Version: 1.3.12    18.4.25



//MODULE SELECTION - uncomment the modules you want to use.
   
  //#define LED               //drive LEDs
  //#define LEDMatrix         //drive LED Matrix using a MAX7219 controller
  //#define LCD               //drive LCD display
  //#define SSegMAX7219       //drive 7-Segment displays via MAX7219 controller
  //#define SSegTM1637        //drive 7-Segment displays via TM1367 controller
  #define SLx2016           //drive 4-digit 5x7 dotmatrix modules
  //#define ServoMotor        //drive servo motors directly connected to the arduino
  //#define ServoPWM          //drive multiple servo motors via pwm shield
  //#define StepperBYJ        //drive stepper motor 28BYJ-48
  //#define StepperX27        //drive stepper motor X27.168
  //#define CompassX27        //drive a compass with a Xxx.xxx -class stepper motor
  //#define StepperVID        //drive multiple stepper motors X25.168 with a VID66-06 controller
  //#define MotorPoti         //motor-driven poti control
  //#define OLED              //display data on an OLED display
  //#define SpeedBrake        //Enable display of the SpeedBrake indicator on an 128x64 OLED display (DEDunino)
  //#define FuelFlowIndicator //Enable display of the FuelFlow indicator on an 128x64 OLED display (DEDunino)
  //#define DED_PFL           //Enable display of DED or PFL on an 254x64 OLED display (DEDunino)
  //#define Switches          //use the arduino to read switch positions and send keyboard commands
  //#define MagSwitch         //enable magnetic held switches														   
  //#define ButtonMatrix      //use the arduino to read switch positions and send keyboard commands
  //#define RotEncoder        //use the arduino to read rotary encoders and send keyboard commands
  //#define AnalogAxis        //use the arduino to read analog resistors and sync this with a gamecontroller axis
  //#define Lighting          //software controlled backlighting
  //#define NewDevice         //placeholder. Use this line to activate your own code to drive other, specific hardware



//BASIC SETTINGS
  #define BAUDRATE 57600      // serial connection speed
  #define POLLTIME 200           // set time between PULL data requests
  #define PULLTIMEOUT 30      // set time to wait for a requested data update defaut: 30ms
  //#define PRIORITIZE_OUTPUT    //uncomment this to put a stress on fast update of outputs (should be used for motors to allow smoother movements)
  //#define PRIORITIZE_INPUT     //uncomment this to put a stress on fast er poll of inputs (switches/Buttons) 
  const char ID[]= "BMSAIT_DotMatrixTest"; //Set the ID for this arduino program. Use any string. The program will use this ID to check in with the BMSAIT windows application

  
//BOARD SELECTION

  //#define UNO         //uncomment this if this sketch will be loaded on an UNO
  #define NANO        //uncomment this if this sketch will be loaded on an NANO
  //#define MICRO       //uncomment this if this sketch will be loaded on an MICRO
  //#define LEONARDO    //uncomment this if this sketch will be loaded on an LEONARDO
  //#define MEGA        //uncomment this if this sketch will be loaded on an MEGA
  //#define DUE         //uncomment this if this sketch will be loaded on an DUE (connected via programming port)
  //#define DUE_NATIVE  //uncomment this if this sketch will be loaded on an DUE (connected via native port)
  //#define ESP         //uncomment this if this sketch will be loaded on an ESP32 or ESP8266

//DATA VARIABLES
  
  // This is the most important part of this sketch. You need to set the data that the Arduino will have to handle
  // Make sure that you chose the definition of VARIABLENANZAHL matches the number of entries in this table
  // Fill data as follows: 
  // 1. Short description (max 5 characters)
  // 2. Flightdata ID (check BMSAIT Variablen.csv)  as a 4-digit string (needed for PULL mode)
  // 3. data type (check BMSAIT Variablen.csv: b=bool, y=byte, 1=byte[], i=integer, f=float, s=string, 2=string[]))  (needed for PULL mode)
  // 4. Output type (10-LED, 20-LCD, 30-MAX7219, 31-TM1637, 40-Servo, 41-ServoPWM, 50-Stepper 28BYJ, 51-Stepper X27-168, 52-StepperBoard VID6606, 60-MotorPoti)
  // 5. Target - i.e. Output PIN of LEDs, a reference to an entry in a module specific data table (i.e. motors)
  // 6. Reference2 - i.e. the line on LCD displays or a link to an entry in container in motor modules
  // 7. Reference3 - i.e. number of characters to display (control display length on 7-segment or LCD displays)
  // 8  Reference4 - i.e. start position (control position of data on 7-segment or LCD displays) 
  // 9. Reference5 - i.e. decimal point (will add a decimal point on 7-segment displays after the given position)
  // 10. Initial value as string (i.e. "00")
  
  
  Datenfeld datenfeld[]=
  {
   //Description ID     DT    OT    target Ref2 Ref3 Ref4 Ref5 RQ,  IV
   {  "POWER",  "1243", 'b',  99,     0,    0,   0,   0,    0, "",  "T"}     //Variable 0 - Status of electrical system of the a/c
  ,{  "CHAFF",  "0150", 'f',  32,     0,    0,   2,   0,    0, "",  "42"}    //Variable 0 - Chaff Count
  }; 
  const byte variableCount = sizeof(datenfeld)/sizeof(datenfeld[0]); 
