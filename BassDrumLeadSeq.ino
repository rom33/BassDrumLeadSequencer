////////////////////////////////////////////
//  3.5" TOUCH SCREEN Bass, Drum & lead   //
//          Sequencer Machine             //
//                                        //
//            rom3 03.2020                //
//                                        //
//                                        //
////////////////////////////////////////////

#include "Arduino.h"
#include "button.h"
#include "midi.h"
#include "TouchScreen.h"
#include <SPI.h>
#include <Scheduler.h>
#include <SdFat.h>

// *** display
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
Adafruit_GFX_Button mybutton = Adafruit_GFX_Button();

// *** card-reader
SdFatSoftSpi<12, 11, 13> SD; //Bit-Bang on the Shield pins
#define CS_PIN 10
File file;

// *** midi
vs1053_midi MIDI;

// *** touch
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin
int TS_MINX = 890;
int TS_MAXX = 90;
int TS_MINY = 890;
int TS_MAXY = 214;
#define MINPRESSURE 25
TouchScreen_kbv myTouch = TouchScreen_kbv(XP, YP, XM, YM, 300);
TSPoint_kbv tp;

// *** rotary encoders
const int CLK[] = {22, 26, 30}; // CLK PIN rotary encoder
const int DT[] = {24, 28, 32}; // DT PIN rotary encoder
const int rotButton[] = {23, 25, 27};
int lastPosition[3], currentPosition[3];
int reverb[] = {12 , 12, 12};
int pan[] = {64, 64, 64};
int Vol[] = {100, 100, 100};
int rotMode[3];

// *** some MACROS
#define DRAW(Colour) if ((tool) ? (color = TFT_BLUE) : (color = Colour))tft.fillRect(xDraw, yDraw, 5 , 7, color);
#define DrawValue(value,height) tft.fillRect(321,height,value,6,TFT_GREEN);tft.fillRect(321+value,height,127-value,6,TFT_BLUE);
#define Format(space) if(space<100&&space>=-9)tft.print(" ");if(space<10&&space>=0)tft.print(" ");
#define DrawValue(value,height) tft.fillRect(321,height,value,6,TFT_GREEN);tft.fillRect(321+value,height,127-value,6,TFT_BLUE);
#define WaitTouch do{tp=myTouch.getPoint();}while(tp.z<MINPRESSURE);
#define GetTouchPoints tp=myTouch.getPoint();xx=map(tp.x,TS_MINX,TS_MAXX,480,0);yy=map(tp.y,TS_MINY,TS_MAXY,320,0);

String drumSounds[] = {"High Q","Slap","Sc.Push","Sc.Pull","Sticks","Sq.Clic","Me.Clic",
                       "Me.Bell","Ac.B.D","Bs Drum","Si.Stic","Ac.Snar","Hd Clap","El.Snar",
                       "L.F.Tom","Cl.HH","H.F.Tom","P.HH","L.Tom","O.HHat","L.M.Tom",
                       "H.M.Tom","Cr.Cym","H.Tom","R.Cym 1","Ch.Cym","R.Bell","Tambou.",
                       "Sp.Cym.","Cowbel","Cr.Cy.2","Vi.slap","R.Cym 2","H.Bongo","L.Bongo",
                       "M.H.Con","O.H.Co","L.Conga","H.Timb.","L.Timb.","H.Agogo","L.Agogo",
                       "Cabasa","Marac.","S.Whis.","L.Whis.","S.Guir.","L.Guiro","Claves",
                       "H.Wd B.","L.Wd B.","M.Cuica","O.Cuica","M.Tria.","O.Tria.","Shaker",
                       "J.bell","B.Tree","Castan.","M.Surdo","O.Surdo"};

String Sounds[] = {"Ac.Gr.Piano","Br.Ac.Piano","El.Gr.Piano","Ho.To.Piano","Elec.Piano1","Elec.Piano2","Harpsichord",
                   "Clavi      ","Celesta    ","Glock.Spiel","Music Box  ","Vibraphone ","Marimba    ","Xylophone  ",
                   "Tub.Bells  ","Dulcimer   ","Dr.BarOrgan","Perc.Organ ","Rock Organ ","ChurchOrgan","Reed Organ ",
                   "Accordion  ","Harmonica  ","TangoAccor.","Ac.GuitarNy","Ac.GuitarSt","El.GuitarJa","El.GuitarCl",
                   "El.GuitarMu","OverdGuitar","Dist.Guitar","GuitarHarmo","Ac.Bass    ","El.BassFing","El.BassPick",
                   "Fretl.Bass ","Slap Bass 1","Slap Bass 2","Synth Bass1","Synth Bass2","Violin     ","Viola      ",
                   "Cello      ","Contrabass ","Tre.Strings","Piz.Strings","Orch.Harp  ","Timpani    ","Str.Ens.1  ",
                   "Str.Ens.2  ","Syn.String1","Syn.String2","Choir Aahs ","Voice Oohs ","Synth Voice","Orch.Hit   ",
                   "Trumpet    ","Trombone   ","Tuba       ","Mut.Trumpet","French Horn","BrasSection","SynthBrass1",
                   "SynthBrass2","Soprano Sax","Alto Sax   ","Tenor Sax  ","BaritoneSax","Oboe       ","EnglishHorn",
                   "Bassoon    ","Clarinet   ","Piccolo    ","Flute      ","Recorder   ","Pan Flute  ","BlownBottle",
                   "Shakuhachi ","Whistle    ","Ocarina    ","Square Lead","Saw Lead   ","CalliopLead","Chiff Lead ",
                   "CharangLead","Voice Lead ","Fifths Lead","Bass + Lead","New Age    ","Warm Pad   ","Polysynth  ",
                   "Choir      ","Bowed      ","Metallic   ","Halo       ","Sweep      ","Rain       ","Sound Track",
                   "Crystal    ","Atmosphere ","Brightness ","Goblins    ","Echoes     ","Sci-fi     ","Sitar      ",
                   "Banjo      ","Shamisen   ","Koto       ","Kalimba    ","Bag Pipe   ","Fiddle     ","Shanai     ",
                   "Tinkle Bell","Agogo      ","PitchedPerc","Woodblock  ","Taiko Drum ","Melodic Tom","Synth Drum ",
                   "Rev.Cymbal ","GuiFretNois","BreathNoise","Seashore   ","Bird Tweet ","Teleph.Ring","Helicopter ",
                   "Applause   ","Gunshot    "};

// *** button declaraions
button ButtPat[] = {
  button(123, 277, 30, 20, "1"),
  button(158, 277, 30, 20, "2"),
  button(193, 277, 30, 20, "3"),
  button(228, 277, 30, 20, "4"),
  button(123, 300, 30, 20, "A"),
  button(158, 300, 30, 20, "B"),
  button(193, 300, 30, 20, "C"),
  button(228, 300, 30, 20, "D")
};
button ButtInst[] = {
  button(265, 300, 32, 20, "Drum"),
  button(265, 277, 32, 20, "Bass"),
  button(265, 254, 32, 20, "Leed"),
};
button ButtInstPlay[] = {
  button(300, 300, 15, 20, "P"),
  button(300, 277, 15, 20, "P"),
  button(300, 254, 15, 20, "P"),
};
button LoopLen[] = {
  button(425, 295, 55, 25, "1 - 2"),
  button(425, 295, 55, 25, "1 - 3"),
  button(425, 295, 55, 25, "1 - 4"),
};
button NoteLen[] = {
  button(0,0,0,0,""),
  button(320, 300, 35, 20,  "1/16"),
  button(320, 277, 35, 20,  "1/8"),
  button(320, 254, 35, 20,  "1/4"),
  button(320, 231, 35, 20,  "1/2"),
  button(320, 208, 35, 20,  " 1"),
};

button InstPlus         = button(460, 163, 20, 20, "+");
button InstMinus        = button(365, 163, 20, 20, "-");
button Rewind           = button(0, 280, 30, 35, "<<");
button StartStopButton  = button(35, 280, 80, 35, "Start/Stop");
button LoopButton       = button(365, 295, 55, 25, "Loop");
button SongButton       = button(365, 280, 115, 25, "Song  Mode");
button PatButton        = button(365, 280, 115, 25, "Pat.  Mode");
button CopyButton       = button(365, 265, 55, 25,  "CP");
button PasteButton      = button(425, 265, 55, 25,  "PA");
button ClearButton      = button(365, 235, 55, 25,  "CL");
button SaveButton       = button(425, 235, 55, 25,  "SV");
button TempMinusButton  = button(365, 205, 55, 25,  "--");
button TempPlusButton   = button(425, 205, 55, 25,  "++");
button Setup            = button(325, 3, 115, 20,  "Setup");
button OctaveUp         = button(320, 148, 35, 25, "Up");
button OctaveDown       = button(320, 178, 35, 25, "Dn");
button BackButton       = button(380, 270, 55, 40, "Back");
button SaveSetup        = button(320, 270, 55, 40, "Save");
button ScreenCalibrate  = button(40, 40, 400, 40,  "Screen Calibrate");
button DrumSetup        = button(40, 140, 400, 40, "Setup Drums");
button SoundSetup       = button(40, 190, 400, 40, "Setup Sound");
button LeadSetup        = button(40, 240, 400, 40, "Setup Lead");
button Calibrate        = button(240, 180, 15, 15, "");

// *** Key declrations
button keys[] =
{
  button (TFT_WHITE, 235),
  button (TFT_BLACK, 216),
  button (TFT_WHITE, 197),
  button (TFT_BLACK, 178),
  button (TFT_WHITE, 159),
  button (TFT_BLACK, 140),
  button (TFT_WHITE, 121),
  button (TFT_WHITE, 102),
  button (TFT_BLACK, 83),
  button (TFT_WHITE, 64),
  button (TFT_BLACK, 45),
  button (TFT_WHITE, 26)
};
// *** variable
int potRead1, potRead2, potRead3, val, rX[8], rY[8], Sound[3];
int32_t clx, crx, cty, cby;
float px, py;
bool pressed;
unsigned long buttonColor[] = {TFT_BLACK, TFT_RED, TFT_MAGENTA, TFT_YELLOW, TFT_CYAN, TFT_GREEN};
byte toggle, patRow1, patRow2, patRow1Old, patRow2Old, instSelect, instSelectOld, loopLen, playtrack, noteLen, noteTouched;
bool play, tool, loopMode, bassPlay, drumPlay, leadPlay;
unsigned short instrument[4][13][17];
unsigned short instNoteLen[4][13][16][16];
short octave[3];
unsigned long instNoteOff[4][13][16];
unsigned short interval = 200, rotInterval = 600, xDraw, yDraw, xx, yy, note, pat, nextPat, copyPat, stp, slope, slope2, slope3, touched;
unsigned short tick, tempo = 120;
unsigned short drumSet[13] = {35, 38, 44, 42, 43, 48, 47, 49, 56, 60, 61, 83};
unsigned long color, SpiSpeed = 50000;
unsigned long currentMillis, previousMillis, currTime, prevTime;
String printInst;
String noteName[] = {"- C","-#C","- D","-#D","- E","-#E","- F","- G","-#G","- A","-#A","- B"};

void setup()
{
/*
  //"15UL", tells the PLL what multiplier value to use
  //13UL = 84MHz,14UL = 90MHZ,15UL = 96MHz,16UL = 102MHz,17UL = 108MHz,18UL = 114MHz, 19UL = 120MHz
  #define SYS_BOARD_PLLAR (CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(13UL) | CKGR_PLLAR_PLLACOUNT(0x3fUL) | CKGR_PLLAR_DIVA(1UL))
  #define SYS_BOARD_MCKR ( PMC_MCKR_PRES_CLK_2 | PMC_MCKR_CSS_PLLA_CLK)
  //Set FWS according to SYS_BOARD_MCKR configuration
  EFC0->EEFC_FMR = EEFC_FMR_FWS(4); //4 waitstate flash access
  EFC1->EEFC_FMR = EEFC_FMR_FWS(4);
  // Initialize PLLA to 114MHz
  PMC->CKGR_PLLAR = SYS_BOARD_PLLAR;
  while (!(PMC->PMC_SR & PMC_SR_LOCKA)) {}
  PMC->PMC_MCKR = SYS_BOARD_MCKR;
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) {}
  // Re-initialize some stuff with the new speed
  SystemCoreClockUpdate();*/

// *** rotary encoders
for (slope = 0; slope < 3; slope++) {
  pinMode (CLK[slope], INPUT);
  pinMode (DT[slope], INPUT);
  pinMode (rotButton[slope], INPUT);
  digitalWrite(rotButton[slope], true);
  digitalWrite(CLK[slope], true);
  digitalWrite(DT[slope], true);
}
// *** touch
pinMode(A8, INPUT);
pinMode(A9, INPUT);
pinMode(A10, INPUT);
pinMode(A11, INPUT);
// *** vs1053
pinMode(VS_DREQ, INPUT);
pinMode(VS_XCS, OUTPUT);
pinMode(VS_XDCS, OUTPUT);
pinMode(VS_RESET, OUTPUT);
// *** vs1053
digitalWrite(VS_XCS, true); //Deselect Control
digitalWrite(VS_XDCS, true); //Deselect Data
digitalWrite(VS_RESET, false);
delayMicroseconds(10);
digitalWrite(VS_RESET, true);
SPI.begin();
SPI.setBitOrder(MSBFIRST);
SPI.setDataMode(SPI_MODE0);
SPI.setClockDivider(SpiSpeed); //Set SPI bus speed to 50K

// *** Use serial for debugging
//  Serial.begin(9600);
//  Serial.println("VS1053 Shield Example");

// *** bank select
//  VSLoadUserCode();
//  VSWriteRegister(0x1e03, 0xff, 0xff);
MIDI.talkMIDI(0xB9, 0, 0x7F);  //Bank select drums. midi cannel 10
MIDI.talkMIDI(0xB9, 0x07, Vol[2]);//0x07 is channel message, set channel volume to near max (127)

MIDI.talkMIDI(0xB8, 0, 0x00); //Default bank GM1
MIDI.talkMIDI(0xC8, Sound[1], 0); //Set instrument number. 0xC8 is a 1 data byte command
MIDI.talkMIDI(0xB8, 0x07, Vol[1]);//0x07 is channel message, set channel volume to near max (127)

MIDI.talkMIDI(0xB7, 0, 0x00); //Default bank GM1
MIDI.talkMIDI(0xC7, Sound[2], 0); //Set instrument number. 0xC7 is a 1 data byte command
MIDI.talkMIDI(0xB7, 0x07, Vol[0]);//0x07 is channel message, set channel volume to near max (127)

// *** display begin
tft.begin(tft.readID());
tft.setRotation(1);
tft.setTextColor(TFT_WHITE, TFT_BLUE);
tft.fillScreen(TFT_BLUE);
welcomeScreen();

// *** card-reader begin
SD.begin(CS_PIN);

readCalibrate();
patternScreen();
for (pat = 0; pat < 16; pat++) {
  readPat();
}
pat = 16;
nextPat = 0;
drawPattern();
pat = nextPat;
drumPlay = true;
loopLen = 1;
noteLen = 1;
Sound[1] = 32;
Sound[2] = 1;
octave[1] = 36;
octave[2] = 48;
// *** scheduler begin
Scheduler.startLoop(readTouch);
}

// The loop function is called in an endless loop
void loop()
{
	  while (1) {
	    MIDI.VSWriteRegister(0x0B, analogRead(A8) / 16, analogRead(A9) / 16); // Master Vol control left right
	    MIDI.talkMIDI(0xB0, 0x0c, 127); // effect control 1 (sets global reverb decay)
	    MIDI.talkMIDI(0xB0, 0x26, analogRead(A10) / 8); // RPN LSB: 0 = bend range
	    playNotes();
	    delay(250 * 60 / tempo);
	  }
}
void playNotes() {
  if (!play) {
    MIDI.talkMIDI(0xB7, 0x7b, 127); //all notes channel 1 off
    MIDI.talkMIDI(0xB8, 0x7b, 127); //all notes channel 2 off
    DrawOrNot();
    return;
  }
  if (tick > 15) {
    if (loopMode) {
      nextPat += 1;
      patRow1Old = patRow1;
      patRow1 += 1;
      if (patRow2 != patRow2Old) {
        nextPat = 0 + patRow2 * 4;
        patRow1 = 0;
        ButtPat[patRow2Old + 4].draw(tft, buttonColor[1]);
        ButtPat[patRow2 + 4].draw(tft, buttonColor[0]);
        patRow2Old = patRow2;
      }
      if (nextPat > loopLen + patRow2 * 4) {
        nextPat = patRow2 * 4;
        patRow1 = 0;
      }
      ButtPat[patRow1Old].draw(tft, buttonColor[1]);
      ButtPat[patRow1].draw(tft, buttonColor[0]);
    }
    DrawOrNot();
    tick = 0;
    stp = 15;
  }
  if (tick > 0)
  {
    stp = tick - 1;
  }
  drawRec();
  for (slope = 0; slope < 12; slope++) {
    if (drumPlay && (instrument[0][slope][pat] >> tick) & (1)) {
      MIDI.noteOn(9, drumSet[slope], 40 + (((instrument[0][12][pat] >> tick) & (1)) * 20));
    }
    if((instNoteOff[1][slope][pat] >> tick) & (1)||(instNoteOff[1][slope][pat] >> 16 + tick) & (1)){
      MIDI.noteOff(8, slope + octave[1], 0);
    }
    if (bassPlay && (instrument[1][slope][pat] >> tick) & (1)) {
      MIDI.noteOn(8, slope + octave[1], 40 + (((instrument[1][12][pat] >> tick) & (1)) * 20));
    }
    if((instNoteOff[2][slope][pat] >> tick) & (1)||(instNoteOff[2][slope][pat] >> 16 + tick) & (1)){
      MIDI.noteOff(7, slope + octave[2], 0);
    }
    if (leadPlay && (instrument[2][slope][pat] >> tick) & (1)) {
      MIDI.noteOn(7, slope + octave[2], 40 + (((instrument[2][12][pat] >> tick) & (1)) * 20));
    }
  }
  tick += 1;
}
