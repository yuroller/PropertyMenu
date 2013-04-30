#include "PropertyMenu.h"

// labels
const __FlashStringHelper *LBL_TIME = F("Time");
const __FlashStringHelper *LBL_DATE = F("Date");
const __FlashStringHelper *LBL_ID = F("Id");
const __FlashStringHelper *LBL_ACTIVE = F("Active");
const __FlashStringHelper *LBL_PROGRAM = F("Program");
const __FlashStringHelper *LBL_TIME_START = F("Time start");
const __FlashStringHelper *LBL_TIME_END = F("Time end");
const __FlashStringHelper *LBL_WEEKLY = F("Weekly");
const __FlashStringHelper *LBL_APPLY = F("Apply");

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
	&activeProp,
	&programProp,
	&dateStartProp,
	&timeStartProp,
	&timeEndProp,
	&weeklyProp,
	NULL
};

PropertyPage recordingPropPage(recordingProperties);

// menu
PropertyPage *propertyPages[] = {
	&settingsPropPage,
	&recordingPropPage,
	NULL
};

//void *lcd = NULL;
//Menu menu(propertyPages, lcd);


int main(int /*argc*/, char* /*argv*/[])
{
	return 0;
}

