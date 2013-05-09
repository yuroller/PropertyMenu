#ifndef PROPERTY_MENU_H_
#define PROPERTY_MENU_H_

//#define NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "WString.h"
#include "LCDWin.h"

typedef void (*Callback)(void);

enum ButtonPress {
	BUTTON_PRESS_NONE = -1,

	BUTTON_PRESS_DOWN,
	BUTTON_PRESS_UP,
	BUTTON_PRESS_ENTER,

	BUTTON_PRESS_COUNT
};


///////////////////////////////////////////////////////////////////////////
// Screen
///////////////////////////////////////////////////////////////////////////

class Screen
{
public:
	Screen(LCDWin *lcd, uint8_t cols, uint8_t rows);
	
	LCDWin *getLcd() const { return _lcd; }
	uint8_t getCols() const { return _cols; }
	uint8_t getRows() const { return _rows; }

private:
	LCDWin *_lcd;
	uint8_t _cols;
	uint8_t _rows;
};

///////////////////////////////////////////////////////////////////////////
// Property
///////////////////////////////////////////////////////////////////////////

class Property
{
public:
	enum {
		INVALID_ROW = 0xff
	};
	const __FlashStringHelper *getName() const { return _name; }
	uint8_t getFocusPart() const { return _focusPart; }
	void nextFocusPart();
	void paintLabel(LCDWin *lcd) const;
	void enterEdit();

	virtual void onEnterEdit();
	virtual void onExitEdit();
	virtual void paintEdit(LCDWin *lcd) const = 0;
	virtual bool processEditInput(ButtonPress button) = 0; // true if it needs redraw

protected:
	Property(const __FlashStringHelper *name, uint8_t maxFocusParts);

private:
	const __FlashStringHelper *_name;
	uint8_t _focusPart;
	uint8_t _maxFocusParts;
};


///////////////////////////////////////////////////////////////////////////
// PropertyTime
///////////////////////////////////////////////////////////////////////////

class PropertyTime: public Property
{
public:
	struct Time {
		uint8_t hour;
		uint8_t mins;
	};
	PropertyTime(const __FlashStringHelper *name, Time *var);
	void onEnterEdit();
	void paintEdit(LCDWin *lcd) const;
	bool processEditInput(ButtonPress button);

private:
	Time *_var;
};


///////////////////////////////////////////////////////////////////////////
// PropertyDate
///////////////////////////////////////////////////////////////////////////

class PropertyDate: public Property
{
public:
	struct Date {
		uint8_t year2000;
		uint8_t month;
		uint8_t day;
	};
	PropertyDate(const __FlashStringHelper *name, Date *var);
	void onEnterEdit();
	void onExitEdit();
	void paintEdit(LCDWin *lcd) const;
	bool processEditInput(ButtonPress button);

private:
	Date *_var;
};


///////////////////////////////////////////////////////////////////////////
// PropertyU16
///////////////////////////////////////////////////////////////////////////

class PropertyU16: public Property
{
public:
	PropertyU16(const __FlashStringHelper *name, uint16_t *var, uint16_t limitMin, uint16_t limitMax);
	void onEnterEdit();
	void paintEdit(LCDWin *lcd) const;
	bool processEditInput(ButtonPress button);

private:
	uint16_t *_var;
	uint16_t _limitMin;
	uint16_t _limitMax;
	uint8_t _displayWidth;
};

///////////////////////////////////////////////////////////////////////////
// PropertyBool
///////////////////////////////////////////////////////////////////////////

class PropertyBool: public Property
{
public:
	PropertyBool(const __FlashStringHelper *name, bool *var);
	void paintEdit(LCDWin *lcd) const;
	bool processEditInput(ButtonPress button);
private:
	bool *_var;
};


///////////////////////////////////////////////////////////////////////////
// PropertyAction
///////////////////////////////////////////////////////////////////////////

class PropertyAction: public Property
{
public:
	PropertyAction(const __FlashStringHelper *name, Callback callback);
	void paintEdit(LCDWin *lcd) const;
	bool processEditInput(ButtonPress button);
private:
	Callback _callback;
};




///////////////////////////////////////////////////////////////////////////
// Page
///////////////////////////////////////////////////////////////////////////

class Page
{
public:
	virtual void paint(Screen *screen) const = 0;
	virtual void buttonInput(ButtonPress button, Screen *screen) = 0;
};


///////////////////////////////////////////////////////////////////////////
// PropertyPage
///////////////////////////////////////////////////////////////////////////

class PropertyPage: public Page
{
public:
	PropertyPage(uint8_t rows, Property *propertiesAry[], Callback beforeShowing=NULL);
	void paint(Screen *screen) const;
	void buttonInput(ButtonPress button, Screen *screen);
	void paintCursor(Screen *screen) const;

private:
	uint8_t _rows;
	uint8_t _topIndex;
	uint8_t _cursorRow;
	Property **_propertiesAry;
	Callback _beforeShowing;
	uint8_t _maxPropNameLen;
};


///////////////////////////////////////////////////////////////////////////
// MenuItem
///////////////////////////////////////////////////////////////////////////

class MenuItem
{
public:
	MenuItem(const __FlashStringHelper *name, PropertyPage *propPage);
private:
	const __FlashStringHelper *_name;
	PropertyPage *_propPage;
};

#endif // PROPERTY_MENU_H_
