# BMSAIT-Project
Falcon BMS to Arduino Interface Tool

BMSAIT ist an interface tool for cockpit builders.
It allows to relay output data from the shared memory of Falcon BMS to output devices like LED, LCD, Servo motors and many others.
BMSAIT also allows to register input commands from digital and analog sources like switches, encoders and potentiometers to simulate keyboard and 
joystick commands that will be send to Falcon BMS.

In order to establish a data transfer via BMSAIT a specific arduino sketch needs to be customized and loaded on an arduino compatible board. A windows app will 
orchestrate the communication between then arduino boards and the flight simulation.

The folder "01 Dokumentation" contains a documentation of the softeare suite (so far only available in german language)
The folder "02 Windows App" contains the needed files to run the windows application of BMSAIT
The folder "03 Arduino Code" contains a basic, unedited set of arduino code to be used for individual customizations
The folder "04 Beispielprogramme" contains pre-edited arduino sketches, matching configurations for the windows app and a documation how to run the examples.

