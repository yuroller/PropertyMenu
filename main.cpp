#include "PropertyMenu.h"
#include "LCDWin.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>


// labels
MakeFlashString(LBL_TIME, "Time");
MakeFlashString(LBL_DATE, "Date");
MakeFlashString(LBL_ID, "Id");
MakeFlashString(LBL_ACTIVE, "Active");
MakeFlashString(LBL_PROGRAM, "Program");
MakeFlashString(LBL_TIME_START, "Time start");
MakeFlashString(LBL_TIME_END, "Time end");
MakeFlashString(LBL_WEEKLY, "Weekly");
MakeFlashString(LBL_APPLY, "Apply");
MakeFlashString(LBL_CLOCK, "Clock");
MakeFlashString(LBL_SETTINGS, "Settings");

// settings
PropertyTime::Time clockTime;
PropertyDate::Date clockDate;
PropertyTime clockProp(LBL_TIME, &clockTime);
PropertyDate dateProp(LBL_DATE, &clockDate); 

Property *settingsProperties[] = {
	&clockProp,
	&dateProp,
	NULL
};

PropertyPage settingsPropPage(settingsProperties);

// recording
uint16_t id;
bool active;
uint16_t program;
PropertyDate::Date dateStart;
PropertyTime::Time timeStart;
PropertyTime::Time timeEnd;
bool weekly;

static void recordingApply()
{
};

PropertyU16 idProp(LBL_ID, &id, 0, 9);
PropertyBool activeProp(LBL_ACTIVE, &active);
PropertyU16 programProp(LBL_PROGRAM, &program, 1, 999);
PropertyDate dateStartProp(LBL_DATE, &dateStart);
PropertyTime timeStartProp(LBL_TIME_START, &timeStart);
PropertyTime timeEndProp(LBL_TIME_END, &timeEnd);
PropertyBool weeklyProp(LBL_WEEKLY, &weekly);
PropertyAction recordingApplyProp(LBL_APPLY, recordingApply);

Property *recordingProperties[] = {
	&idProp,
	&activeProp,
	&programProp,
	&dateStartProp,
	&timeStartProp,
	&timeEndProp,
	&weeklyProp,
	&recordingApplyProp,
	NULL
};

PropertyPage recordingPropPage(recordingProperties);

//RecListPage recListPage(&recordingPropPage);

// menu
PropertyPage *propertyPages[] = {
	&settingsPropPage,
	//&recListPage,
	&recordingPropPage,
	NULL
};


//Menu menu(propertyPages);

enum Key {
	KEY_UP = 'w',
	KEY_DOWN = 's',
	KEY_ENTER = 13,
	KEY_ESC = 27
};

ButtonPress translateKey(int k)
{
	ButtonPress b = BUTTON_PRESS_NONE;
	switch (k) {
	case KEY_UP:
		b = BUTTON_PRESS_UP;
		break;
	case KEY_DOWN:
		b = BUTTON_PRESS_DOWN;
		break;
	case KEY_ENTER:
		b = BUTTON_PRESS_ENTER;
		break;
	default:
		break;
	}
	return b;
}

class NumberedPage : public ScrollablePage
{
public:
	explicit NumberedPage(uint8_t maxLines);
	void paintLine(uint8_t line, uint8_t row, Screen *screen) const;
	void focusLine(uint8_t line);
	// TODO: lastSelectedEntry()
};

NumberedPage::NumberedPage(uint8_t maxLines)
{
	setMaxLines(maxLines);
}

void NumberedPage::paintLine(uint8_t line, uint8_t row, Screen *screen) const
{
	assert(screen != NULL);
	LCDWin *lcd = screen->getLcd();
	lcd->setCursor(1, row);
	lcd->print(line);
}

void NumberedPage::focusLine(uint8_t line)
{
}


int main(int /*argc*/, char* /*argv*/[])
{
	LCDWin lcd;
	Screen screen(&lcd, 24, 4);
	//PropertyPage *propPage = &settingsPropPage;
	Page *propPage = &recordingPropPage;
	propPage->paint(&screen);
	//NumberedPage numPage(10);
	//numPage.paint(&screen);

	for (;;) {
		if (_kbhit()) {
			int k = _getch();
			if (k == KEY_ESC) {
				break;
			}
			ButtonPress b = translateKey(k);
			bool ret = propPage->buttonInput(b, &screen);
			//numPage.buttonInput(b, &screen);
			lcd.setCursor(0, 10);
			lcd.print(ret ? "T" : "F");
		}
		Sleep(50);
	}
	return 0;
}

