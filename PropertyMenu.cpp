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

PropertyTime::PropertyTime(const __FlashStringHelper *name, PropertyTime::Time *var, uint8_t step)
: Property(name, 2),
	_var(var),
	_step(step)
{
	assert(var != NULL);
	assert(step > 0);
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
	assert(0 <= getFocusPart() && getFocusPart() < 2);
	uint8_t focusPart = getFocusPart();
	lcd->print(focusPart == 1 ? SEL_LEFT : ' ' );
	pad00Print(lcd, _var->hour);
	lcd->print(_var->hour);
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
	assert(0 < getFocusPart() && getFocusPart() < 2);
	uint8_t focusPart = getFocusPart();
	if (button == BUTTON_DOWN) {
		if (focusPart == 1) {
			if (_var->hour == 0) {
				_var->hour = 23;
			} else {
				_var->hour--;
			}
		} else {
			if (_var->mins < _step) {
				_var->mins += 60 - _step;
			} else {
				_var->mins += _step;
			}
		}
		return true;
	} else if (button == BUTTON_UP) {
		if (focusPart == 1) {
			_var->hour++;
			if (_var->hour == 24) {
				_var->hour = 0;
			}
		} else {
			_var->mins += _step;
			if (_var->mins >= 60) {
				_var->mins = _var->mins - 60;
			}
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
: Property(name, 2),
	_var(var)
{
	assert(var != NULL);
}

void PropertyDate::paintEdit(LCD *lcd, const PaintPos *pos) const
{
	assert(0);
}

bool PropertyDate::processEditInput(Button button)
{
	assert(0);
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

