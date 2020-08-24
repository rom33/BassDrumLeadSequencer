/*
 * button.h
 *
 *  Created on: 29.03.2020
 *      Author: rom3
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include"Arduino.h"
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

class button {
public:
	button();
	  String m_caption; // captions for buttons
	  int m_color;
	  int m_x; // x and y coordinates are start point for drawing octave buttons
	  int m_y;
	  int m_width; // width of octave button
	  int m_height; // height of octave button
	  int color;
	  String m_instrument;
	  button(int xcoord, int ycoord, int width, int height,String buttonCaption); // prototype for constructor of Button object
	  button(int color, int keyy); // prototype of constructor for 'Key' data type

	  boolean contains(int tx, int ty);
	  // This is a method (or "member function") declaration.
	  // Its implementation (in Button.cpp) will check to see if one of the octave buttons has been pressed.

	  void draw(MCUFRIEND_kbv tft, int color);
	  // A second method declaration, it passes the parameter "TFT_HX8357_Due"
	  // to draw() and gives it the local name "tft".
	  void drawKey(MCUFRIEND_kbv tft, String Instrument);
};

#endif /* BUTTON_H_ */
