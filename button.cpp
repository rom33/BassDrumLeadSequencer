/*
 * button.cpp
 *
 *  Created on: 29.03.2020
 *      Author: rom3
 */

#include "button.h"
#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

	button::button(int xcoord, int ycoord, int width, int height, String buttonCaption)
	{
	  m_caption = buttonCaption;
	  m_x = xcoord;
	  m_y = ycoord;
	  m_width = width;
	  m_height = height;
	}

	button::button(int color, int keyy)
	{
	  m_color  = color;
	  m_y = keyy;
	  m_x = 265;
	  m_width = 50 ;
	  m_height = 17 ;
	}

	// The member function 'boolean contains(int tx, int ty)' checks to see if a given point is within
	// one of the buttons has been pressed.
	boolean button::contains(int tx, int ty)
	{
	  if ((tx > m_x) && (tx < m_x + m_width))
	  {
	    if ((ty > m_y) && (ty < m_y + m_height))
	    {
	      return true;  // point lies inside the button
	    }
	  }
	  return false;  // point lies outside the button
	}

	// The member function 'draw(Adafruit_IL9341 tft' draws the buttons
	void button::draw(MCUFRIEND_kbv tft, int color)
	{
	  tft.fillRoundRect(m_x, m_y, m_width, m_height, 5, color);
	  tft.drawRoundRect(m_x, m_y, m_width, m_height, 5, TFT_WHITE);
	  tft.setCursor(m_x + 4 , m_y + m_height / 2 - 4);
	  tft.setTextColor(TFT_WHITE);
	  tft.println(m_caption);
	}

	// the 'draw' method checks the key colour and draws the key
	void button::drawKey(MCUFRIEND_kbv tft, String Instrument)
	{
	  tft.fillRect(m_x, m_y, m_width, m_height, m_color);
	  tft.setCursor(m_x + 4 , m_y + m_height / 2 - 4);
	  if(m_color==TFT_BLACK?color = TFT_WHITE:color = TFT_BLACK);
	  tft.setTextColor(color);
	  tft.println(Instrument);
	}

