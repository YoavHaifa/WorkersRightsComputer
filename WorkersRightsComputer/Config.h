#pragma once

class CConfig
{
public:
	CConfig(void);
	void InitFromXml();
	CString msVersion;
	int miVersion;
	int miLegacyVersion;
	bool mbBackwardCompatibilityMode;
	CString msSaveRoot;

	bool mb14DaysUnpaidVacation4SeveranceDefault;
	//int mNMonthsForFullVacation;
	CString msFilledBy;
	CString msFilledByHebrew;
	CString msContactPhone;
	CString msContactFax;
	CString msContactEmail;
	bool SaveState();
	bool RestoreState();


	static const int umnDaysInNormalYear = 365;
	static const int umnMaxDaysInYear = 366;
};

extern CConfig gConfig;