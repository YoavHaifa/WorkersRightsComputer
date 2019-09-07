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
	CMyTime& GetPrevYearEnd(int iPrev);
	double GetLastYearAsFraction(void);
	CString PrepareSpanString(void);
	bool WorkedAtLeastNMonths(int nMonths);
	bool LastYearDoContains(class CHoliday& holiday);

	int mn;
	int mnMonthsInLastYear;
	int mnDaysInLastYear;
	double mDaysInLastYearAsFraction;
	int mnFullWorkYears;
	int mnPrevYears;
	int mnWorkingDays; // Only relevant to compute vacation for short period
	double mYearsForSeverance;
	int mnDaysForSeveranceAddedForUnpaidVacations;
	bool mbAllowPartYearSeverance;
	static const int N_MIN_MONTHS_FOR_SPECIAL_CASE = 10;

private:
	static const int MAX_YEARS = 50;
	CWorkYear maYears[MAX_YEARS];

	void ComputeWorkDays(void); // WARNING: This procedure dont take into account vacations (as yet)!
};

extern CWorkYears gWorkYears;