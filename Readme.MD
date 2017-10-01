### Oscilloscope based on PIC18F4550 Microcontroller and graphic LCD 128x64 pixels


This project is a simple oscilloscope which uses only two main components: a PIC18F4550 and a graphic LCD. The oscilloscope exploites the PIC's analog to digital module and outputs the sampled signal on the LCD. Other two ADC channels are used to change the amplitude of the output signal and also the time scale through two different potentiometers. 

This was the project of the course Computer Science Laboratory at The University of Padova

Author:

Davide Lucchi

### How to compile
The source code is available under the folder MPLAB IDE. That folder can be opened directly with MPLAB IDE and the program can be compiled

The folder Proteus Labcenter Electronics contains a file that can be loaded in that Software to emulate the behavior of the Oscilloscope

### Results
The results are available in presentazione.odp and tesina.pdf The system performs as expected and it shows the limits of the PIC's Analog to digital converter. The maximum input frequency in fact is limited at about 10-15KHz but nonetheless the experience from this project can be used as a good starting point towards a more advanced oscilloscope which may exploit a dedicated ADC, a more powerful microcontroller and a TFT LCD.
### Documentation
The project Doxygen documentation is provided in the Doc folder
