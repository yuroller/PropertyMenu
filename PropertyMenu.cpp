#include "PropertyMenu.h"

const char SEL_LEFT = '[';
const char SEL_RIGHT = ']';
const __FlashStringHelper *PREV_MENU = F("..");

static void pad00Print(LCD *lcd, uint8_t n)
{
	assert(lcd != NULL);
	assert(n < 100);
	if (n < 10) {
		lcd->print('0');
	}
	lcd->print(n);
}

static void padMulti0Print(LCD *lcd, uint16_t n, uint8_t width)
{
	assert(lcd != NULL);
	uint16_t m = 10;
	for (uint8_t i = 1; i < width; ++i) {
		if (n < m) {
			lcd->print('0');
		}
		m *= 10;
	}
	assert(n < m);
	lcd->print(n);
}

static void clipU8(uint8_t *n, uint8_t low, uint8_t high)
{
	assert(n != NULL);
	assert(low < high);

	if (*n < low) {
		*n = low;
	} else if (*n > high) {
		*n = high;
	}
}

static void wrapDecreaseU8(uint8_t *n, uint8_t low, uint8_t high)
{
	assert(n != NULL);
	assert(low < high);

	if (*n == low) {
		*n = high;
	} else {
		(*n)--;
	}
}

static void wrapIncreaseU8(uint8_t *n, uint8_t low, uint8_t high)
{
	assert(n != NULL);
	assert(low < high);

	if (*n == high) {
		*n = low;
	} else {
		(*n)++;
	}
}

///////////////////////////////////////////////////////////////////////////
// Property
///////////////////////////////////////////////////////////////////////////

Property::Property(const __FlashStringHelper *name, uint8_t maxFocusParts)
: _name(name),
	_focusPart(0),
	_maxFocusParts(maxFocusParts)
{
	assert(name != NULL);
	assert(maxFocusParts > 0);
}

void Property::nextFocusPart()
{
	_focusPart++;
	if (_focusPart == _maxFocusParts) {
		_focusPart = 0;
		onExitEdit();
	}
}

void Property::paintLabel(LCD *lcd, const PaintPos *pos) const
{
	assert(lcd != NULL);
	assert(pos != NULL);
	lcd->setCursor(pos->lblCol, pos->row);
	lcd->print(_name);
}

void Property::enterEdit(LCD *lcd, const PaintPos *pos)
{
	assert(lcd != NULL);
	assert(pos != NULL);
	onEnterEdit();
	_focusPart = 1;
	paintEdit(lcd, pos);
}

void Property::onEnterEdit()
{
}

void Property::onExitEdit()
{
}

///////////////////////////////////////////////////////////////////////////
// PropertyTime
///////////////////////////////////////////////////////////////////////////

PropertyTime::PropertyTime(const __FlashStringHelper *name, PropertyTime::Time *var)
: Property(name, 2),
	_var(var)
{
	assert(var != NULL);
}

void PropertyTime::onEnterEdit()
{
	if (_var->hour > 23) {
		_var->hour = 23;
	}
	if (_var->mins > 59) {
		_var->hour = 59;
	}
}

void PropertyTime::paintEdit(LCD *lcd, const PaintPos *pos) const
{
	assert(lcd != NULL);
	assert(pos != NULL);
	assert(getFocusPart() < 2);
	uint8_t focusPart = getFocusPart();
	lcd->print(focusPart == 1 ? SEL_LEFT : ' ' );
	pad00Print(lcd, _var->hour);
	switch (focusPart) {
		case 1:
			lcd->print(SEL_RIGHT);
			break;
		case 2:
			lcd->print(SEL_LEFT);
			break;
		default:
			lcd->print(':');
	}
	pad00Print(lcd, _var->mins);
	lcd->print(focusPart == 2 ? SEL_RIGHT : ' ');
}

bool PropertyTime::processEditInput(Button button)
{
	uint8_t focusPart = getFocusPart();
	assert(0 < focusPart && focusPart < 2);
	if (button == BUTTON_DOWN) {
		if (focusPart == 1) {
			wrapDecreaseU8(&_var->hour, 0, 23);
		} else {
			wrapDecreaseU8(&_var->mins, 0, 59);
		}
		return true;
	} else if (button == BUTTON_UP) {
		if (focusPart == 1) {
			wrapIncreaseU8(&_var->hour, 0, 23);
		} else {
			wrapIncreaseU8(&_var->mins, 0, 59);
		}
		return true;
	} else if (button == BUTTON_ENTER) {
		nextFocusPart();
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////
// PropertyDate
///////////////////////////////////////////////////////////////////////////

PropertyDate::PropertyDate(const __FlashStringHelper *name, PropertyDate::Date *var)
: Property(name, 3),
	_var(var)
{
	assert(var != NULL);
}

void PropertyDate::onEnterEdit()
{
	clipU8(&_var->day, 1, 31);
	clipU8(&_var->month, 1, 12);
	if (_var->year2000 > 99) {
		_var->year2000 = 99;
	}
}

void PropertyDate::onExitEdit()
{
	// TODO: adjust date to a correct value
}

void PropertyDate::paintEdit(LCD *lcd, const PaintPos *pos) const
{
	assert(lcd != NULL);
	assert(pos != NULL);
	assert(getFocusPart() < 3);
	uint8_t focusPart = getFocusPart();
	lcd->print(focusPart == 1 ? SEL_LEFT : ' ' );
	pad00Print(lcd, _var->day);
	switch (focusPart) {
		case 1:
			lcd->print(SEL_RIGHT);
			break;
		case 2:
			lcd->print(SEL_LEFT);
			break;
		default:
			lcd->print('/');
	}
	pad00Print(lcd, _var->month);
	switch (focusPart) {
		case 2:
			lcd->print(SEL_RIGHT);
			break;
		case 3:
			lcd->print(SEL_LEFT);
			break;
		default:
			lcd->print('/');
	}
	lcd->print("20");
	pad00Print(lcd, _var->year2000);
	lcd->print(focusPart == 3 ? SEL_RIGHT : ' ');
}

bool PropertyDate::processEditInput(Button button)
{
	uint8_t focusPart = getFocusPart();
	assert(0 < focusPart && focusPart < 4);
	if (button == BUTTON_DOWN) {
		if (focusPart == 1) {
			wrapDecreaseU8(&_var->day, 1, 31);
		} else if (focusPart == 2) {
			wrapDecreaseU8(&_var->month, 1, 12);
		} else {
			wrapDecreaseU8(&_var->year2000, 0, 99);
		}
		return true;
	} else if (button == BUTTON_UP) {
		if (focusPart == 1) {
			wrapIncreaseU8(&_var->day, 1, 31);
		} else if (focusPart == 2) {
			wrapIncreaseU8(&_var->month, 1, 12);
		} else {
			wrapIncreaseU8(&_var->year2000, 0, 99);
		}
		return true;
	} else if (button == BUTTON_ENTER) {
		nextFocusPart();
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////
// PropertyU8
///////////////////////////////////////////////////////////////////////////

PropertyU8::PropertyU8(const __FlashStringHelper *name, uint8_t *var, uint8_t limitMin, uint8_t limitMax)
: Property(name, 1),
	_var(var),
	_limitMin(limitMin),
	_limitMax(limitMax)
{
	assert(var != NULL);
	assert(limitMax > limitMin);
}

void PropertyU8::onEnterEdit()
{
	clipU8(_var, _limitMin, _limitMax);
}

void PropertyU8::paintEdit(LCD *lcd, const PaintPos *pos) const
{
	assert(lcd != NULL);
	assert(pos != NULL);
	assert(getFocusPart() < 3);

	uint8_t focusPart = getFocusPart();
	lcd->print(focusPart == 1 ? SEL_LEFT : ' ' );
	padMulti0Print(lcd, *_var, 3);
	lcd->print(focusPart == 1 ? SEL_RIGHT : ' ');
}

bool PropertyU8::processEditInput(Button button)
{
	uint8_t focusPart = getFocusPart();
	assert(focusPart == 1);
	if (button == BUTTON_DOWN) {
		wrapDecreaseU8(_var, _limitMin, _limitMax);
		return true;
	} else if (button == BUTTON_UP) {
		wrapIncreaseU8(_var, _limitMin, _limitMax);
		return true;
	} else if (button == BUTTON_ENTER) {
		nextFocusPart();
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////
// PropertyBool
///////////////////////////////////////////////////////////////////////////

PropertyBool::PropertyBool(const __FlashStringHelper *name, bool *var)
: Property(name, 1),
	_var(var)
{
	assert(var != NULL);
}

void PropertyBool::paintEdit(LCD *lcd, const PaintPos *pos) const
{
	assert(0);
}

bool PropertyBool::processEditInput(Button button)
{
	assert(0);
	return false;
}

///////////////////////////////////////////////////////////////////////////
// PropertyAction
///////////////////////////////////////////////////////////////////////////

PropertyAction::PropertyAction(const __FlashStringHelper *name, Callback callback)
: Property(name, 1),
	_callback(callback)
{
	assert(callback != NULL);
}

void PropertyAction::paintEdit(LCD *lcd, const PaintPos *pos) const
{
	assert(0);
}

bool PropertyAction::processEditInput(Button button)
{
	assert(0);
	return false;
}

///////////////////////////////////////////////////////////////////////////
// PropertyPage
///////////////////////////////////////////////////////////////////////////

PropertyPage::PropertyPage(Property *propertiesAry[], Callback beforeShowing)
: _propertiesAry(propertiesAry[0]),
	_beforeShowing(beforeShowing),
	_topIndex(0)
{
	assert(propertiesAry != NULL);		
}

void PropertyPage::paint(LCD *lcd) const
{
	assert(0);
}

bool PropertyPage::buttonInput(Button button)
{
	assert(0);
	return false;
}

