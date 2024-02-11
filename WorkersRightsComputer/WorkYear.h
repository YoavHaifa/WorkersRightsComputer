#pragma once
#include "MyTime.h"
#include "WorkSpan.h"

class CWorkYear : public CWorkSpan
{
public:
	CWorkYear(void);
	~CWorkYear(void);

	CWorkYear* mpPrev;
	int mSeniority;
	double mFraction; // Only the last (this) year may be < 1
	int mnUnpaidVacationCalendarDaysForSeverance;
	static const int MIN_MONTHS_FOR_UNPAID_VACATION_DAYS_FOR_SEVERANCE = 10;
	static const int MAX_14_UNPAID_VACATION_DAYS_FOR_SEVERANCE = 14;

	void InitFirst(void);
	void InitNext(CWorkYear &prev);
	void InitInternals(CMyTime &firtsDay);
	int GetNFullMonths(int* pnExtraDays, double* pExtraDaysFraction);
	bool Contains(class CHoliday& holiday);
	bool Contains(CMyTime &time);
	bool HasFullYearUntil(CMyTime& lastDay);

	int GetUnpaidVacationCalendarDaysForSeverance(void);
	double GetVacationFraction();

	void Log(FILE* pfLog);
	int mDebug;
};

