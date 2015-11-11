# FA58GHZ Firmware

The firmware is based in Arduino, you can use Arduino or Atmel Studio with the Arduino pluging.

It uses the Arduino Library TVOut with minor modifications (https://code.google.com/p/arduino-tvout/) which is included in this folder. To install, copy the content of the ZIP file into (Arduino Folder)\libraries. The TVOut library has been modified to add the Pause and Resume functions, therefore It's recommended to use this freakyattic version instead of the original.

Right now the software it is in Beta version, you could use it but not all the functions are fully tested.

Current Firmware Version: 1.01 Beta

### TODO List
- Power control of RF Module, will be implemented with hardware PCB
- Diversity Mode: Add 2 RF receivers and use the RSSI to switch between them.

### DONE List
- Fan menu and options
- Manual Selection of RF Video Channel.
- RF Spectrum analyser.
- Handling the RF module and SPI functions.
- Starting coding ;)

# ---PROJECT UNDER DEVELOPMENT---
