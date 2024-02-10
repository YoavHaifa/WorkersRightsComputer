#pragma once

class CYearlyRates
{
public:
	CYearlyRates(const wchar_t *zName, int firstYear);
	bool InitFromTextFile(void);
	void SaveMonthlyXmlFile(void);
	bool PrintLog(void);
	double RatePerYear(int year);

	CString msName;

	static const int MAX_RATES = 101;
	const int mFirstYear;

	bool mbValid;
	double ma[MAX_RATES];
	bool mabDefined[MAX_RATES];

	// Remember definitions for convert to monthly rates
	int mnDefs;
	int maDefYear[MAX_RATES];
	double maDefRate[MAX_RATES];
};

