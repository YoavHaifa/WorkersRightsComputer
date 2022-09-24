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
};

extern CConfig gConfig;