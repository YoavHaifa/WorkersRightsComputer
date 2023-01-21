#pragma once

class CConfig
{
public:
	CConfig(void);
	void InitFromXml();
	CString msVersion;
	CString msSaveRoot;
	bool mb14DaysUnpaidVacation4SeveranceDefault;
	int mNMonthsForFullVacation;
	CString msFilledBy;
	CString msFilledByHebrew;
	CString msContactPhone;
	CString msContactFax;
	CString msContactEmail;
	bool SaveState();
	bool RestoreState();
};

extern CConfig gConfig;