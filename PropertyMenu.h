#ifndef PROPERTY_MENU_H_
#define PROPERTY_MENU_H_

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "WString.h"
#include "LiquidCrystal.h"

typedef void (*Callback)(void);

enum Button {
	BUTTON_DOWN,
	BUTTON_UP,
	BUTTON_ENTER,

	BUTTON_COUNT
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
	struct PaintPos {
		uint8_t row;
		uint8_t lblCol;
		uint8_t edtCol;
	};
	uint8_t getFocusPart() const { return _focusPart; }
	void nextFocusPart();
	void paintLabel(LCD *lcd, const PaintPos *pos) const;
	void enterEdit(LCD *lcd, const PaintPos *pos);

	virtual void onEnterEdit();
	virtual void onExitEdit();
	virtual void paintEdit(LCD *lcd, const PaintPos *pos) const = 0;
	virtual bool processEditInput(Button button) = 0; // true if it needs redraw

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
	PropertyTime(const __FlashStringHelper *name, Time *var, uint8_t step=1);
	void onEnterEdit();
	void paintEdit(LCD *lcd, const PaintPos *pos) const;
	bool processEditInput(Button button);

private:
	Time *_var;
	uint8_t _step;
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
	void paintEdit(LCD *lcd, const PaintPos *pos) const;
	bool processEditInput(Button button);

private:
	Date *_var;
};


///////////////////////////////////////////////////////////////////////////
// PropertyNum
///////////////////////////////////////////////////////////////////////////

template <typename T>
class PropertyNum: public Property
{
public:
	PropertyNum(const __FlashStringHelper *name, T *var, T limitMin, T limitMax, T step=1);
	void paintEdit(LCD *lcd, const PaintPos *pos) const;
	bool processEditInput(Button button);

private:
	T *_var;
	T _limitMin;
	T _limitMax;
	T _step;
};

template <typename T>
PropertyNum<T>::PropertyNum(const __FlashStringHelper *name, T *var, T limitMin, T limitMax, T step)
: Property(name, 1),
	_var(var),
	_limitMin(limitMin),
	_limitMax(limitMax),
	_step(step)
{
	assert(var != NULL);
	assert(step > 0);
	assert(limitMin < limitMax);
}

template <typename T>
void PropertyNum<T>::paintEdit(LCD *lcd, const PaintPos *pos) const
{
	assert(0);
}

template <typename T>
bool PropertyNum<T>::processEditInput(Button button)
{
	assert(0);
	return false;
}

typedef PropertyNum<uint8_t> PropertyU8;
typedef PropertyNum<uint16_t> PropertyU16;


///////////////////////////////////////////////////////////////////////////
// PropertyBool
///////////////////////////////////////////////////////////////////////////

class PropertyBool: public Property
{
public:
	PropertyBool(const __FlashStringHelper *name, bool *var);
	void paintEdit(LCD *lcd, const PaintPos *pos) const;
	bool processEditInput(Button button);
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
	void paintEdit(LCD *lcd, const PaintPos *pos) const;
	bool processEditInput(Button button);
private:
	Callback _callback;
};


///////////////////////////////////////////////////////////////////////////
// Page
///////////////////////////////////////////////////////////////////////////

class Page
{
public:
	virtual void paint(LCD *lcd) const = 0;
	virtual bool buttonInput(Button button) = 0;
};


///////////////////////////////////////////////////////////////////////////
// PropertyPage
///////////////////////////////////////////////////////////////////////////

class PropertyPage: public Page
{
public:
	PropertyPage(Property *propertiesAry[], Callback beforeShowing=NULL);
	void paint(LCD *lcd) const;
	bool buttonInput(Button button);

private:
	Property *_propertiesAry;
	Callback _beforeShowing;
	uint8_t _editCol;
	uint8_t _topIndex;
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