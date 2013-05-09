#include "PropertyMenu.h"
#include "LCDWin.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>

#define PROGMEM

#define MakeFlashString(name, value) \
  static const char __##name[] PROGMEM = value; \
  const __FlashStringHelper *name = reinterpret_cast<const __FlashStringHelper *>(__##name);

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

PropertyPage settingsPropPage(2, settingsProperties);

// recording
uint16_t id;
bool active;
uint16_t program;
PropertyDate::Date dateStart;
PropertyTime::Time timeStart;
PropertyTime::Time timeEnd;
bool weekly;

static void recordingApply() {
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
//	&activeProp,
	&programProp,
	&dateStartProp,
	&timeStartProp,
	&timeEndProp,
//	&weeklyProp,
	NULL
};

PropertyPage recordingPropPage(2, recordingProperties);

// menu
PropertyPage *propertyPages[] = {
	&settingsPropPage,
	&recordingPropPage,
	NULL
};


//Menu menu(propertyPages, lcd);

enum Key {
	KEY_UP = 97,
	KEY_DOWN = 122,
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

int main(int /*argc*/, char* /*argv*/[])
{
	LCDWin lcd;
	Screen screen(&lcd, 24, 2);
	//PropertyPage *propPage = &settingsPropPage;
	PropertyPage *propPage = &recordingPropPage;
	propPage->paint(&screen);

	while (true) {
		if (_kbhit()) {
			int k = _getch();
			if (k == KEY_ESC) {
				break;
			}
			ButtonPress b = translateKey(k);
			propPage->buttonInput(b, &screen);
		}
		Sleep(50);
	}
	return 0;
}

