#pragma once


class CMonthlyRates
{
public:
	CMonthlyRates(const wchar_t *zName, int firstYear);
	bool InitFromFile(void);
	bool PrintLog(void);
	double RatePerMonth(int year, int month);

	CString msName;

	static const int MAX_RATES = 1212; // 101 years * 12 months
	const int mFirstYear;

	bool mbValid;
	double ma[MAX_RATES]; 
	bool mabDefined[MAX_RATES]; 
};

