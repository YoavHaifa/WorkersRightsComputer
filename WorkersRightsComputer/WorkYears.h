#pragma once
#include "WorkYear.h"

class CWorkYears
{
public:
	CWorkYears(void);

	void Compute(void);
	void Clear(void);
	void Log();

	CMyTime& GetLastYearStart(void);
	double GetLastYearAsFraction(void);
	CString PrepareSpanString(void);
	bool WorkedAtLeastNMonths(int nMonths);
	bool LastYearDoContains(class CHoliday& holiday);

	int mn;
	int mnMonthsInLastYear;
	int mnDaysInLastYear;
	int mnFullWorkYears;
	int mnPrevYears;
	int mnWorkingDays; // Only relevant to compute vacation for short period
	double mYearsForSeverance;

private:
	static const int MAX_YEARS = 50;
	CWorkYear maYears[MAX_YEARS];

	void ComputeWorkDays(void); // WARNING: This procedure dont take into account vacations (as yet)!
};

extern CWorkYears gWorkYears;