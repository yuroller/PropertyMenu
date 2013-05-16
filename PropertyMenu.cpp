#include "PropertyMenu.h"

const char SEL_LEFT = '[';
const char SEL_RIGHT = ']';
const char PREV_MENU[] = "..";

static void pad00Print(LCDWin *lcd, uint8_t n)
{
	assert(lcd != NULL);
	assert(n < 100);
	if (n < 10) {
		lcd->print('0');
	}
	lcd->print(n);
}

static void padMulti0Print(LCDWin *lcd, uint16_t n, uint8_t width)
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

template<typename T>
static void clipValue(T *n, T low, T high)
{
	assert(n != NULL);
	assert(low < high);

	if (*n < low) {
		*n = low;
	} else if (*n > high) {
		*n = high;
	}
}

template<typename T>
static void wrapDecrease(T *n, T low, T high)
{
	assert(n != NULL);
	assert(low < high);

	if (*n == low) {
		*n = high;
	} else {
		(*n)--;
	}
}

template<typename T>
static void wrapIncrease(T *n, T low, T high)
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
// Screen
///////////////////////////////////////////////////////////////////////////

Screen::Screen(LCDWin *lcd, uint8_t cols, uint8_t rows)
: _lcd(lcd),
	_cols(cols),
	_rows(rows)
{
	assert(_lcd != NULL);
	assert(_cols > 0);
	assert(_rows > 0);
	_lcd->begin(cols, rows);
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
	if (_focusPart > _maxFocusParts) {
		_focusPart = 0;
		onExitEdit();
	}
}

void Property::paintLabel(LCDWin *lcd) const
{
	assert(lcd != NULL);
	lcd->print(_name);
}

void Property::enterEdit()
{
	onEnterEdit();
	_focusPart = 1;
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

void PropertyTime::paintEdit(LCDWin *lcd) const
{
	assert(lcd != NULL);
	assert(getFocusPart() <= 2);
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
			break;
	}
	pad00Print(lcd, _var->mins);
	lcd->print(focusPart == 2 ? SEL_RIGHT : ' ');
}

bool PropertyTime::processEditInput(ButtonPress button)
{
	uint8_t focusPart = getFocusPart();
	assert(0 < focusPart && focusPart <= 2);
	if (button == BUTTON_PRESS_DOWN) {
		if (focusPart == 1) {
			wrapDecrease<uint8_t>(&_var->hour, 0, 23);
		} else {
			wrapDecrease<uint8_t>(&_var->mins, 0, 59);
		}
		return true;
	} else if (button == BUTTON_PRESS_UP) {
		if (focusPart == 1) {
			wrapIncrease<uint8_t>(&_var->hour, 0, 23);
		} else {
			wrapIncrease<uint8_t>(&_var->mins, 0, 59);
		}
		return true;
	} else if (button == BUTTON_PRESS_ENTER) {
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
	clipValue<uint8_t>(&_var->day, 1, 31);
	clipValue<uint8_t>(&_var->month, 1, 12);
	if (_var->year2000 > 99) {
		_var->year2000 = 99;
	}
}

void PropertyDate::onExitEdit()
{
	// TODO: adjust date to a correct value
}

void PropertyDate::paintEdit(LCDWin *lcd) const
{
	assert(lcd != NULL);
	assert(getFocusPart() <= 3);
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
			break;
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
			break;
	}
	lcd->print("20");
	pad00Print(lcd, _var->year2000);
	lcd->print(focusPart == 3 ? SEL_RIGHT : ' ');
}

bool PropertyDate::processEditInput(ButtonPress button)
{
	uint8_t focusPart = getFocusPart();
	assert(0 < focusPart && focusPart <= 3);
	if (button == BUTTON_PRESS_DOWN) {
		if (focusPart == 1) {
			wrapDecrease<uint8_t>(&_var->day, 1, 31);
		} else if (focusPart == 2) {
			wrapDecrease<uint8_t>(&_var->month, 1, 12);
		} else {
			wrapDecrease<uint8_t>(&_var->year2000, 0, 99);
		}
		return true;
	} else if (button == BUTTON_PRESS_UP) {
		if (focusPart == 1) {
			wrapIncrease<uint8_t>(&_var->day, 1, 31);
		} else if (focusPart == 2) {
			wrapIncrease<uint8_t>(&_var->month, 1, 12);
		} else {
			wrapIncrease<uint8_t>(&_var->year2000, 0, 99);
		}
		return true;
	} else if (button == BUTTON_PRESS_ENTER) {
		nextFocusPart();
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////
// PropertyU16
///////////////////////////////////////////////////////////////////////////

struct LimitWidth {
	uint16_t moreThan;
	uint8_t width;
};

static const LimitWidth limitWidth[] = {
	{ 9999, 5 },
	{ 999, 4 }, 
	{ 99, 3 },
	{ 9, 2 },
	{ 0, 1 }
};

PropertyU16::PropertyU16(const __FlashStringHelper *name, uint16_t *var, uint16_t limitMin, uint16_t limitMax)
: Property(name, 1),
	_var(var),
	_limitMin(limitMin),
	_limitMax(limitMax)
{
	assert(var != NULL);
	assert(limitMax > limitMin);

	for (const LimitWidth *lw = &limitWidth[0]; ; ++lw) {
		if (lw->moreThan < limitMax) {
			_displayWidth = lw->width;
			break;
		}
	}
}

void PropertyU16::onEnterEdit()
{
	clipValue(_var, _limitMin, _limitMax);
}

void PropertyU16::paintEdit(LCDWin *lcd) const
{
	assert(lcd != NULL);
	assert(getFocusPart() <= 1);

	uint8_t focusPart = getFocusPart();
	lcd->print(focusPart == 1 ? SEL_LEFT : ' ' );
	padMulti0Print(lcd, *_var, _displayWidth);
	lcd->print(focusPart == 1 ? SEL_RIGHT : ' ');
}

bool PropertyU16::processEditInput(ButtonPress button)
{
	assert(getFocusPart() == 1);
	if (button == BUTTON_PRESS_DOWN) {
		wrapDecrease(_var, _limitMin, _limitMax);
		return true;
	} else if (button == BUTTON_PRESS_UP) {
		wrapIncrease(_var, _limitMin, _limitMax);
		return true;
	} else if (button == BUTTON_PRESS_ENTER) {
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

void PropertyBool::paintEdit(LCDWin *lcd) const
{
	assert(0);
}

bool PropertyBool::processEditInput(ButtonPress button)
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

void PropertyAction::paintEdit(LCDWin *lcd) const
{
	assert(0);
}

bool PropertyAction::processEditInput(ButtonPress button)
{
	assert(0);
	return false;
}

///////////////////////////////////////////////////////////////////////////
// PropertyPage
///////////////////////////////////////////////////////////////////////////

PropertyPage::PropertyPage(Property **propertiesAry, Callback beforeShowing)
: _topIndex(0),
	_cursorRow(0),
	_propertiesAry(propertiesAry),
	_beforeShowing(beforeShowing),
	_maxPropNameLen(0)
{
	assert(propertiesAry != NULL);
	assert(propertiesAry[0] != NULL);
	size_t maxLen = 0;
	for (uint8_t i = 0; ; ++i) {
		const Property *p = _propertiesAry[i];
		if (p == NULL) {
			break;
		}
		size_t l = strlen(reinterpret_cast<const char *>(p->getName()));
		if (l > maxLen) {
			maxLen = l;
		}
	}
	_maxPropNameLen = static_cast<uint8_t>(maxLen);
}

void PropertyPage::paint(Screen *screen) const
{
	assert(screen != NULL);
	LCDWin *lcd = screen->getLcd();
	lcd->clear();
	for (uint8_t i = 0; i < screen->getRows(); ++i) {
		Property *p = _propertiesAry[_topIndex + i];
		if (p != NULL) {
			lcd->setCursor(1, i);
			p->paintLabel(lcd);
			lcd->setCursor(_maxPropNameLen + 2, i);
			p->paintEdit(lcd);
		}
	}
	paintCursor(screen);
}

bool PropertyPage::buttonInput(ButtonPress button, Screen *screen)
{
	assert(screen != NULL);
	Property *p = _propertiesAry[_topIndex + _cursorRow];
	LCDWin *lcd = screen->getLcd();
	bool res = false;
	bool editMode = p->getFocusPart() != 0;
	if (editMode) {
		if (p->processEditInput(button)) {
			lcd->setCursor(_maxPropNameLen + 2, _cursorRow);
			p->paintEdit(lcd);
		}
		res = true;
	} else {
		switch (button) {
			case BUTTON_PRESS_ENTER:
				p->enterEdit();
				lcd->setCursor(_maxPropNameLen + 2, _cursorRow);
				p->paintEdit(lcd);
				res = true;
				break;
			case BUTTON_PRESS_DOWN:
				if ( _propertiesAry[_topIndex + _cursorRow + 1] != NULL) {
					if (_cursorRow < screen->getRows() - 1) {
						_cursorRow++;
						paintCursor(screen);
					} else {
						_topIndex++;
						paint(screen);
					}
				}
				res = true;
				break;
			case BUTTON_PRESS_UP:
				if (_topIndex + _cursorRow > 0) {
					if (_cursorRow < screen->getRows() - 1) {
						_topIndex--;
						paint(screen);
					} else {
						_cursorRow--;
						paintCursor(screen);
					}
				}
				res = true;
				break;
			default:
				break;
		}
	}
	return res;
}

void PropertyPage::paintCursor(Screen *screen) const
{
	assert(screen != NULL);
	LCDWin *lcd = screen->getLcd();
	for (uint8_t i = 0; i < screen->getRows(); ++i) {
		lcd->setCursor(0, i);
		lcd->print(_cursorRow == i ? '>' : ' ');
	}
}