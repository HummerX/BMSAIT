# BMSAIT-Project
Falcon BMS to Arduino Interface Tool

BMSAIT is an interface tool for cockpit builders.
It allows to relay output data from the shared memory of Falcon BMS to output devices like LED, LCD, Servo motors and many others.
BMSAIT also allows to register input commands from digital and analog sources like switches, encoders and potentiometers to simulate keyboard and 
joystick commands that will be send to Falcon BMS.</p>

In order to establish a data transfer via BMSAIT a specific arduino sketch needs to be customized and loaded on an arduino compatible board. A windows app will 
orchestrate the communication between the arduino boards and the flight simulation.</p>

The folder "01 Dokumentation" contains a documentation of the software suite (so far only available in german language)</p>
The folder "02 Windows App" contains the needed files to run the windows application of BMSAIT</p>
The folder "03 Arduino Code" contains a basic, unedited set of arduino code to be used for individual customizations</p>
The folder "04 Beispielprogramme" contains pre-edited arduino sketches, matching configurations for the windows app and a documentation about how to run the examples.</p>

tutorial videos:
BMSAIT setup/basic settings:         https://www.youtube.com/watch?v=HeJZFDz4m_k
BMSAIT introduction/tutial by Mihi:  https://www.youtube.com/watch?v=6cLPfiHxl_U
