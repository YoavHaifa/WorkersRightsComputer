#pragma once

class CYearlyRates
{
public:
	CYearlyRates(const wchar_t *zName, int firstYear);
	bool InitFromFile(void);
	bool PrintLog(void);
	double RatePerYear(int year);

	CString msName;

	static const int MAX_RATES = 101;
	const int mFirstYear;

	bool mbValid;
	double ma[MAX_RATES];
	bool mabDefined[MAX_RATES];
};

