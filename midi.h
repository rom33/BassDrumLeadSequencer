/*
 * midi.h
 *
 *  Created on: 29.03.2020
 *      Author: rom3
 */

#ifndef MIDI_H_
#define MIDI_H_
#include <SPI.h>

#define VS_XCS    46 // Control Chip Select Pin (for accessing SPI Control/Status registers)
#define VS_XDCS   48 // Data Chip Select / BSYNC Pin
#define VS_DREQ   50 // Data Request Pin: Player asks for more data
#define VS_RESET  53 // Reset is active low

class vs1053_midi {
public:
	void sendMIDI(byte data);
	void talkMIDI(byte cmd, byte data1, byte data2);
	void noteOn(byte channel, byte note, byte attack_velocity);
	void noteOff(byte channel, byte note, byte release_velocity);
	void VSWriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte);
	void VSLoadUserCode(void);
};

#endif /* MIDI_H_ */
