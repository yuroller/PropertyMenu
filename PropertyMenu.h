#ifndef PROPERTY_MENU_H_
#define PROPERTY_MENU_H_

//#define NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "WString.h"
#include "LCDWin.h"

#ifdef _WIN32
#define PROGMEM
#endif

#define MakeFlashString(name, value) \
  static const char __##name[] PROGMEM = value; \
  const __FlashStringHelper *name = reinterpret_cast<const __FlashStringHelper *>(__##name);

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
	void onEnterEdit();
private:
	Callback _callback;
	bool _confirm;
};

///////////////////////////////////////////////////////////////////////////
// Page
///////////////////////////////////////////////////////////////////////////

class Page
{
public:
	enum {
		INVALID_LINE = 0xff
	};
	virtual void paint(Screen *screen) const;
	// INVALID_LINE if staying in the same page, else the number of last line selected when returning to parent
	virtual uint8_t buttonInput(ButtonPress button, Screen *screen);
};


///////////////////////////////////////////////////////////////////////////
// ScrollablePage
///////////////////////////////////////////////////////////////////////////

class ScrollablePage: public Page
{
public:
	enum {
		COL_CURSOR = 0,
		COL_CONTENTS = 1
	};
	ScrollablePage();
	uint8_t getCursorRow() const { return _cursorRow; }
	uint8_t getMaxLines() const { return _maxLines; }
	void setMaxLines(uint8_t maxLines);
	void paint(Screen *screen) const;
	uint8_t buttonInput(ButtonPress button, Screen *screen);
	void paintCursor(Screen *screen) const;
	virtual void paintLine(uint8_t line, uint8_t row, Screen *screen) const = 0;
	virtual void focusLine(uint8_t line) = 0;

private:
	uint8_t _maxLines;
	uint8_t _topIndex;
	uint8_t _cursorRow;
};

///////////////////////////////////////////////////////////////////////////
// PropertyPage
///////////////////////////////////////////////////////////////////////////

class PropertyPage: public ScrollablePage
{
public:
	explicit PropertyPage(Property *propertiesAry[], Callback beforeShowing=NULL);
	uint8_t buttonInput(ButtonPress button, Screen *screen);
	void paintLine(uint8_t line, uint8_t row, Screen *screen) const;
	void focusLine(uint8_t line);

private:
	Property **_propertiesAry;
	Callback _beforeShowing;
	uint8_t _maxPropNameLen;
	uint8_t _focusLine;
};


///////////////////////////////////////////////////////////////////////////
// MenuItem
///////////////////////////////////////////////////////////////////////////

class MenuItem
{
public:
	MenuItem(const __FlashStringHelper *name, Page *page);
private:
	const __FlashStringHelper *_name;
	Page *_page;
};

#endif // PROPERTY_MENU_H_
