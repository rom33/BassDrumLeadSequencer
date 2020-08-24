/*
 * midi.cpp
 *
 *  Created on: 29.03.2020
 *      Author: rom3
 */

#include "midi.h"
#include <SPI.h>

void vs1053_midi::sendMIDI(byte data)
	{
	  delayMicroseconds(10);
	  SPI.transfer(0);
	  delayMicroseconds(10);
	  SPI.transfer(data);
	}
	//Plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that data values are less than 127
	void vs1053_midi::talkMIDI(byte cmd, byte data1, byte data2) {
	  //
	  // Wait for chip to be ready (Unlikely to be an issue with real time MIDI)
	  //
	  while (!digitalRead(VS_DREQ));
	  delayMicroseconds(10);
	  digitalWrite(VS_XDCS, LOW);
	  vs1053_midi::sendMIDI(cmd);
	  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes
	  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
	  if ( (cmd & 0xF0) <= 0xB0 || (cmd & 0xF0) >= 0xE0) {
	    sendMIDI(data1);
	    sendMIDI(data2);
	  } else {
	    sendMIDI(data1);
	  }
	  digitalWrite(VS_XDCS, HIGH);
	  delayMicroseconds(10);
	}
	//Send a MIDI note-on message.  Like pressing a piano key
	//channel ranges from 0-15
	void vs1053_midi::noteOn(byte channel, byte note, byte attack_velocity) {
	  vs1053_midi::talkMIDI( (0x90 | channel), note, attack_velocity);
	}
	//Send a MIDI note-off message.  Like releasing a piano key
	void vs1053_midi::noteOff(byte channel, byte note, byte release_velocity) {
	  vs1053_midi::talkMIDI( (0x80 | channel), note, release_velocity);
	}
	//Write to VS10xx register
	//SCI: Data transfers are always 16bit. When a new SCI operation comes in
	//DREQ goes low. We then have to wait for DREQ to go high again.
	//XCS should be low for the full duration of operation.
	void vs1053_midi::VSWriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte) {
	  while (!digitalRead(VS_DREQ)) ; //Wait for DREQ to go high indicating IC is available
	  digitalWrite(VS_XCS, LOW); //Select control

	  //SCI consists of instruction byte, address byte, and 16-bit data word.
	  SPI.transfer(0x02); //Write instruction
	  SPI.transfer(addressbyte);
	  SPI.transfer(highbyte);
	  SPI.transfer(lowbyte);
	  while (!digitalRead(VS_DREQ)) ; //Wait for DREQ to go high indicating command is complete
	  digitalWrite(VS_XCS, HIGH); //Deselect Control
	}

	//
	// Plugin to put VS10XX into realtime MIDI mode
	// Originally from http://www.vlsi.fi/fileadmin/software/VS10XX/vs1053b-rtmidistart.zip
	// Permission to reproduce here granted by VLSI solution.
	//
	const unsigned short sVS1053b_Realtime_MIDI_Plugin[28] = { /* Compressed plugin */
	  0x0007, 0x0001, 0x8050, 0x0006, 0x0014, 0x0030, 0x0715, 0xb080, /*    0 */
	  0x3400, 0x0007, 0x9255, 0x3d00, 0x0024, 0x0030, 0x0295, 0x6890, /*    8 */
	  0x3400, 0x0030, 0x0495, 0x3d00, 0x0024, 0x2908, 0x4d40, 0x0030, /*   10 */
	  0x0200, 0x000a, 0x0001, 0x0050,
	};

	void vs1053_midi::VSLoadUserCode(void) {
	  int i = 0;

	  while (i < sizeof(sVS1053b_Realtime_MIDI_Plugin) / sizeof(sVS1053b_Realtime_MIDI_Plugin[0])) {
	    unsigned short addr, n, val;
	    addr = sVS1053b_Realtime_MIDI_Plugin[i++];
	    n = sVS1053b_Realtime_MIDI_Plugin[i++];
	    while (n--) {
	      val = sVS1053b_Realtime_MIDI_Plugin[i++];
	      vs1053_midi::VSWriteRegister(addr, val >> 8, val & 0xFF);
	    }
	  }
	}
