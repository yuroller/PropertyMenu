#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "LCDWin.h"
#include "ConsoleCore.h"

LCD::LCD() 
{
	_core = ConsoleCore::GetInstance();
}

void LCD::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) 
{
   if (lines > 1) 
   {
      _displayfunction |= LCD_2LINE;
   }
   _numlines = lines;
   _cols = cols;
   
   // for some 1 line displays you can select a 10 pixel high font
   // ------------------------------------------------------------
   if ((dotsize != LCD_5x8DOTS) && (lines == 1)) 
   {
      _displayfunction |= LCD_5x10DOTS;
   }
        
   // turn the display on with no cursor or blinking default
   _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
   display();
   
   // clear the LCD
   clear();
   
   // Initialize to default text direction (for romance languages)
   _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

   backlight();
}

// Common LCD Commands
// ---------------------------------------------------------------------------
void LCD::clear()
{
	_core->ClearScreen();
}

void LCD::home()
{
	setCursor(0, 0);
}

void LCD::setCursor(uint8_t col, uint8_t row)
{
	_core->CursorPosition(col, row);
}

// Turn the display on/off
void LCD::noDisplay() 
{
   _displaycontrol &= ~LCD_DISPLAYON;
}

void LCD::display() 
{
   _displaycontrol |= LCD_DISPLAYON;
}

// Turns the underline cursor on/off
void LCD::noCursor() 
{
   _displaycontrol &= ~LCD_CURSORON;
}
void LCD::cursor() 
{
   _displaycontrol |= LCD_CURSORON;
}

// Turns on/off the blinking cursor
void LCD::noBlink() 
{
   _displaycontrol &= ~LCD_BLINKON;
}

void LCD::blink() 
{
   _displaycontrol |= LCD_BLINKON;
}

// These commands scroll the display without changing the RAM
void LCD::scrollDisplayLeft(void) 
{
}

void LCD::scrollDisplayRight(void) 
{
}

// This is for text that flows Left to Right
void LCD::leftToRight(void) 
{
   _displaymode |= LCD_ENTRYLEFT;
}

// This is for text that flows Right to Left
void LCD::rightToLeft(void) 
{
   _displaymode &= ~LCD_ENTRYLEFT;
}

// This method moves the cursor one space to the right
void LCD::moveCursorRight(void)
{
}

// This method moves the cursor one space to the left
void LCD::moveCursorLeft(void)
{
}


// This will 'right justify' text from the cursor
void LCD::autoscroll(void) 
{
   _displaymode |= LCD_ENTRYSHIFTINCREMENT;
}

// This will 'left justify' text from the cursor
void LCD::noAutoscroll(void) 
{
   _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
}

// Write to CGRAM of new characters
void LCD::createChar(uint8_t /* location */, uint8_t /* charmap */[]) 
{
}

//
// Switch on the backlight
void LCD::backlight ( void )
{
}

//
// Switch off the backlight
void LCD::noBacklight ( void )
{
}

//
// Switch fully on the LCD (backlight and LCD)
void LCD::on ( void )
{
   display();
   backlight();
}

//
// Switch fully off the LCD (backlight and LCD) 
void LCD::off ( void )
{
   noBacklight();
   noDisplay();
}

#if (ARDUINO <  100)
void LCD::write(uint8_t value)
{
}
#else
size_t LCD::write(uint8_t value) 
{
	char s[2] = {value, 0};
	_core->Prints(s);
   return 1;             // assume OK
}
#endif
