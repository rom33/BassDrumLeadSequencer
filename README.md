# BassDrumLeadSequencer

A simple sequencer for Arduino Due with 3.5" 8-bit tft-display(e.g.MCUfriends) and vs1053.


# Needed libraries:
```sh
Adafruit_GFX,
MCUFRIEND_kbv,
Scheduler,
SDfat
```
Install these libraries from the library manager in the Arduino IDE.

Now we must configure some on the SdFat libray. Edit the SdFatConfig.h, and change
```sh
#define ENABLE_SOFTWARE_SPI_CLASS 0
```
to
```sh
#define ENABLE_SOFTWARE_SPI_CLASS 1
```

# Wiring

for the vs1053:
```sh
XCS to 46
XDCS to 48
DREQ to 50
XRST to 52
MOSI to A.26
MISO to A.25
SCK to A.27
5V to 3.3V
```

for the rotary encoders:
```sh
all GNDs to GND
all + to 3.3 volts
SW from encoder1  to 23 (drums: vol, delay, pan)  
CLK from encoder1 to 22 
DT from encoder1 to 24 
SW from encoder2 to 25 (bass sound: vol, delay, pan)
CLK from encoder2 to 26 
DT from encoder2 to 28
SW from encoder3 to 27 (lead sound: vol, delay, pan)
CLK from encoder3 to 30 
DT from encoder3 to 32 
```

the potis:
```sh
one outer pin from all potis to gnd 
the other outer pin to 3.3V
and the middle pins to:
```
```sh
poti 1 to A8 (Master Vol control left)
poti 2 to A9 (Master Vol control right)
poti 3 to A10 (sets global reverb decay)
poti 4 to A11 (bend range)
```
